#include "../Utilities/Core.h"
#include "../Utilities/Math.h"

#include "../Rendering/Projector.h"

#include <iostream>
#include <cmath>

namespace GOL {

std::vector<Projector::AmbiguousPoint> Projector::buildOptimalOutput() {
    std::vector<Line> lines;
    std::vector<Color> colors;

    // build a collection of lines, throwing away all lines invisible to the canvas, cropping the lines that are partially visible to the canvas
    for(auto& graphic : this->graphics) {
        auto glines = graphic.getLines();

        for(int i = 0; i < glines.size(); ++i) {
            auto& line = glines[i];

            // cropping lines. Unfortunately gotta dupe the same code 4 times for 2 axes and start + end

            if(line.start.x < 0 && line.end.x < 0)
                continue;
            if(line.start.x > CanvasSize && line.end.x > CanvasSize)
                continue;
            if(line.start.y < 0 && line.end.y < 0)
                continue;
            if(line.start.y > CanvasSize && line.end.y > CanvasSize)
                continue;

            if(line.start.x < 0 || line.start.x > CanvasSize) {
                float xDiff;
                if(line.start.x < 0)
                    xDiff = line.start.x;
                else {
                    xDiff = line.start.x - CanvasSize;
                }

                float vecMulti = xDiff / (line.end.x - line.start.x);
                Vec2F lineVecToEdge = {vecMulti * (line.end.x - line.start.x), vecMulti * (line.end.y - line.start.y)};

                line.start -= lineVecToEdge;
            }
            if(line.end.x < 0 || line.end.x > CanvasSize) {
                float xDiff;
                if(line.end.x < 0)
                    xDiff = line.end.x;
                else
                    xDiff = line.end.x - CanvasSize;

                float vecMulti = xDiff / (line.start.x - line.end.x);
                Vec2F lineVecToEdge = {vecMulti * (line.start.x - line.end.x), vecMulti * (line.start.y - line.end.y)};

                line.end -= lineVecToEdge;
            }

            if(line.start.y < 0 || line.start.y > CanvasSize) {
                float yDiff;
                if(line.start.y < 0)
                    yDiff = line.start.y;
                else
                    yDiff = line.start.y - CanvasSize;

                float vecMulti = yDiff / (line.end.y - line.start.y);
                Vec2F lineVecToEdge = {vecMulti * (line.end.x - line.start.x), vecMulti * (line.end.y - line.start.y)};

                line.start -= lineVecToEdge;
            }
            if(line.end.y < 0 || line.end.y > CanvasSize) {
                float yDiff;
                if(line.end.y < 0)
                    yDiff = line.end.y;
                else
                    yDiff = line.end.y - CanvasSize;

                float vecMulti = yDiff / (line.start.y - line.end.y);
                Vec2F lineVecToEdge = {vecMulti * (line.start.x - line.end.x), vecMulti * (line.start.y - line.end.y)};

                line.end -= lineVecToEdge;
            }

            // check for pesky corners
            if(line.start.x < -Epsilon || line.start.x > CanvasSize + Epsilon || line.end.x < -Epsilon || line.end.x > CanvasSize + Epsilon
                || line.start.y < -Epsilon || line.start.y > CanvasSize + Epsilon || line.end.y < -Epsilon || line.end.y > CanvasSize + Epsilon)
                continue;

            lines.emplace_back(glines[i]);
            colors.emplace_back(graphic.getColor(graphic.getColorChannel(i)));
        }
    }
    if(lines.empty()) return {{CanvasCenter, CanvasCenter, Blank}};

    //place blank line in between laser pos and first point
    lines.insert(lines.begin(), {galvoState.currentPos, lines[0].start});
    colors.insert(colors.begin(), Blank);


    // generate blank lines in between line gaps
    for(int i = 0; i < lines.size()-1; ++i) {
        if(!pointsAreEqual(lines[i].end, lines[i+1].start)) {
            lines.insert(lines.begin()+i+1, {lines[i].end, lines[i+1].start});
            colors.insert(colors.begin()+i+1, Blank);
        }
    }


    // place blank point at the end
    Vec2F endPos = lines.back().end;
    lines.emplace_back(endPos, endPos);
    colors.emplace_back(Blank);

    // always ensure they add up to 1.f
    constexpr float accelRatio = 0.8f;
    constexpr float deccelRatio = 0.2f;

    float totalDistance = 0.f;
    float accelDistance = 0.f;
    float deccelDistance = 0.f;
    float blankDistance = 0.f;
    for(int i = 0; i < lines.size(); i++) {
        totalDistance += distance(lines[i].start, lines[i].end);

        if(colors[i].a == 0)
            blankDistance += distance(lines[i].start, lines[i].end);
        else {
            accelDistance += distance(lines[i].start, lines[i].end) * accelRatio;
            deccelDistance += distance(lines[i].start, lines[i].end) * deccelRatio;
        }
    }
    float distancePerAccelPoint = accelDistance / lines.size() / (optimizationConfig.accelerationControl);
    float distancePerDeccelPoint = deccelDistance / lines.size() / (optimizationConfig.corneringLevel);

    galvoState.currentPos = {lines.back().end.x, lines.back().end.y};


    // build a list of points using the list of lines. Apply the optimization parameters for accel and deaccel
    // accel - first 80% of a line ; deaccel - last 20% of a line
    std::vector<AmbiguousPoint> points;

    for(int i = 0; i < lines.size(); ++i) {
        auto& line = lines[i];

        Vec2F lineVec = line.end - line.start;
        float lineDist = distance(line.end, line.start);

        //int pointsInThisLine = static_cast<int>(std::max(2.f, std::floor(lineDist / distancePerPoint)));

        float angleToNextLine = 0.f;
        if (i != lines.size() - 1)
            angleToNextLine = angleTo(lines[i].end, lines[i+1].end);

    }

    return points;
}

void Projector::writeFrameToDAC(std::vector<AmbiguousPoint>& points, unsigned PPS) {
    Vec2F outputBounds; // x is lower bound, y is lower bound. Canvas size is a single number as it is a perfect square

    if(this->currentDeviceDAC == HELIOS || this->currentDeviceDAC == HELIOS_HQ) {
        auto start = std::chrono::steady_clock::now();
        while(dac.GetStatus(deviceID) != 1) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            if (std::chrono::steady_clock::now() - start >= std::chrono::seconds(1)) {
                std::cerr << "Renderer::pushFrame no response from set device (" + std::to_string(deviceID) + ") within 1 second of waiting to send output\n";
                return;
            }
        }

        std::vector<HeliosPoint> output;

        for(auto& point : points) {
            HeliosPoint p;

            if(this->currentDeviceDAC == HELIOS) {
                point.pos /= 16.f;
                outputBounds = {0.f, 4095.f};
            }
            if(this->currentDeviceDAC == HELIOS_HQ) {
                outputBounds = {INT16_MIN, INT16_MAX};
            }

            p.x = static_cast<uint16_t>(std::clamp(std::round(point.pos.x), outputBounds.x, outputBounds.y));
            p.y = static_cast<uint16_t>(std::clamp(std::round(point.pos.y), outputBounds.x, outputBounds.y));

            p.r = point.color.r;
            p.g = point.color.g;
            p.b = point.color.b;
            p.i = point.color.a;

            output.emplace_back(p);
        }

        this->dac.WriteFrame(deviceID, PPS, 0, output.data(), output.size());
    }
}

}

/*inline std::vector<std::vector<Line>> groupConnectedLines(std::vector<Graphic>& graphics) {
    std::vector<std::vector<Line>> groups;

    for(auto& graphic : graphics) {
        auto lines = graphic.getLines();

        while(!lines.empty()) {



        }
    }

    return groups;
}

inline std::vector<HeliosPoint> Renderer::buildOptimalOutput() {

    struct ConnectedComponent {
        std::vector<Line> lines;
        std::vector<Color> linesColors;
        std::vector<Vec2F> edges;
    };

    std::vector<ConnectedComponent> connectedComponents;
    std::vector<ConnectedComponent> connectedGroups;
    std::vector<HeliosPoint> output;

    // Step 1: create Connected Components
    for(auto& graphic : this->graphics) {
        auto lines = graphic.getLines();

        while(!lines.empty()) {
            ConnectedComponent newCC;

            //start from line 0, travel until reaching one end of the connected component
            bool keepsGoing = true;

            while(keepsGoing) {
                keepsGoing = false;
                for(int i = 0; i < lines.size(); i++) {
                    if (pointsAreEqual(lines[0].end, lines[i].start) || pointsAreEqual(lines[0].start, lines[i].end)) {
                        keepsGoing = true;
                        break;
                    }
                }

                newCC.lines.emplace_back(lines[0]);
                newCC.linesColors.emplace_back();
                lines.erase(lines.begin());
            }

            connectedComponents.emplace_back(newCC);
        }

        for(auto& comp : connectedComponents) {
            std::cout << "\nEdges:\n";
            for(auto& edge : comp.edges) {
                std::cout << edge << ", \n";
            }
            std::cout << "\nLines:\n";
            for(auto& line : comp.lines) {
                std::cout << line.start << " - " << line.end << "\n";
            }
        }

    }

    return output;
}*/
