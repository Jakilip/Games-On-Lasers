#include "../Utilities/Core.h"
#include "../Utilities/Math.h"

#include "../Rendering/Projector.h"

#include <iostream>
#include <cmath>
#include <cassert>

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
    if(lines.empty()) return {{CanvasCenter, Blank}};

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



    ///
    /// optimize output based on the lines we got
    ///

    // accel - first 80% of a line ; deccel - last 20% of a line
    constexpr float accelRatio = 0.8f;
    constexpr float deccelRatio = 0.2f;

    // easing strengths used by the accel / deccel curves below
    constexpr float accelStrength = 2.5f;
    constexpr float deccelStrength = 4.0f;

    // Share of a line's points that goes to accel so the galvo speed matches where accel ends and deccel starts.
    // speed (line fractions per point) at accel exit  = accelRatio  * accelStrength  / pointsForAccel
    // speed (line fractions per point) at deccel start = deccelRatio * deccelStrength / pointsForDeccel
    // setting them equal gives pointsForAccel / pointsForDeccel = (accelRatio * accelStrength) / (deccelRatio * deccelStrength)
    constexpr float accelPointsShare = (accelRatio * accelStrength) / (accelRatio * accelStrength + deccelRatio * deccelStrength);

    unsigned pointsBudget = this->scanrate / this->framerate; // for 15000 pps 50 fps its 300


    // 1st pass - get distances to traverse
    float accelDistance = 0.f;
    float deccelDistance = 0.f;
    float blankDistance = 0.f;

    for (int i = 0; i < lines.size(); ++i) {
        float lineDist = distance(lines[i].start, lines[i].end);
        if (colors[i].a == 0)
            blankDistance += lineDist;
        else {
            accelDistance += lineDist * accelRatio;
            deccelDistance += lineDist * deccelRatio;
        }
    }


    // 2nd pass - generate ratios based on distances, and modify them based on cornering and optimization config
    float totalDistance = accelDistance + deccelDistance + blankDistance;

    float accelAlloc = accelDistance / totalDistance;
    float deccelAlloc = deccelDistance / totalDistance;
    float blankAlloc = blankDistance / totalDistance;

    auto angleDiff = [](Line& line1, Line& line2) {
        // zero length lines (like the final blank point) have no direction, so they don't count as a turn
        if (distance(line1.start, line1.end) <= Epsilon || distance(line2.start, line2.end) <= Epsilon)
            return 0.f;

        float diff = std::abs(angleTo(line1.start, line1.end) - angleTo(line2.start, line2.end));
        if (diff >= 180.f)
            diff = 360.f - diff;
        return diff;
    };

    float totalTurnsDegrees = 0.f;
    for (int i = 0; i < lines.size()-1; ++i) {
        totalTurnsDegrees += angleDiff(lines[i], lines[i+1]);
    }
    float averageTurn = totalTurnsDegrees / static_cast<float>(lines.size()-1);

    deccelAlloc += (averageTurn / 180.f * 2.f); // allocate from 0% to 200% more, proportionally to the average turn

    accelAlloc += (this->optimizationConfig.accelerationControl);
    deccelAlloc += (this->optimizationConfig.corneringLevel);
    blankAlloc += (this->optimizationConfig.blankingLevel);

    float totalAlloc = accelAlloc + deccelAlloc + blankAlloc;

    accelAlloc = accelAlloc / totalAlloc;
    deccelAlloc = deccelAlloc / totalAlloc;
    blankAlloc = blankAlloc / totalAlloc;

    assert(accelAlloc + deccelAlloc + blankAlloc + GOL::Epsilon >= 1.0f);

    // 3rd pass - distribute points for each line
    std::vector<unsigned> pointsAllocated(lines.size());

    auto accelPoints = static_cast<unsigned>(accelAlloc * static_cast<float>(pointsBudget));
    auto deccelPoints = static_cast<unsigned>(deccelAlloc * static_cast<float>(pointsBudget)) / 2;
    auto deccelTurnsPoints = static_cast<unsigned>(deccelAlloc * static_cast<float>(pointsBudget)) / 2;
    auto blankPoints = static_cast<unsigned>(blankAlloc * static_cast<float>(pointsBudget));

    float roundingLeftovers = 0.f;

    for (int i = 0; i < lines.size(); ++i) {
        float lineDist = distance(lines[i].start, lines[i].end);

        if (colors[i].a == 0) {
            float lineRatio = blankDistance > 0.f ? lineDist / blankDistance : 0.f;
            float pointsForThisLine = lineRatio * static_cast<float>(blankPoints);
            pointsAllocated[i] = std::floor(pointsForThisLine);

            roundingLeftovers += pointsForThisLine - static_cast<float>(pointsAllocated[i]);
            if (roundingLeftovers >= 1.f) {
                pointsAllocated[i] += static_cast<unsigned>(std::floor(roundingLeftovers));
                roundingLeftovers -= std::floor(roundingLeftovers);
            }
        }
        else {
            // this line's share of all the lit distance (accel and deccel are both a fixed fraction of the line, so the share is the same)
            float accelRatio = (accelDistance + deccelDistance) > 0.f ? lineDist / (accelDistance + deccelDistance) : 0.f;
            float deccelRatio = accelRatio;

            float turnRatio;
            if (i + 1 >= lines.size() || totalTurnsDegrees <= 0.f)
                turnRatio = 0.f;
            else
                turnRatio = angleDiff(lines[i], lines[i+1]) / totalTurnsDegrees;

            float pointsForThisAccel = accelRatio * static_cast<float>(accelPoints);
            float pointsForThisDecel = deccelRatio * static_cast<float>(deccelPoints);
            float pointsForThisTurn = turnRatio * static_cast<float>(deccelTurnsPoints);

            pointsAllocated[i] = static_cast<unsigned>(std::floor(pointsForThisAccel) + std::floor(pointsForThisDecel) + std::floor(pointsForThisTurn));
            roundingLeftovers += pointsForThisAccel + pointsForThisDecel + pointsForThisTurn - static_cast<float>(pointsAllocated[i]);
            if (roundingLeftovers >= 1.f) {
                pointsAllocated[i] += static_cast<unsigned>(std::floor(roundingLeftovers));
                roundingLeftovers -= std::floor(roundingLeftovers);
            }
        }
    }

    // the final blank point has zero length so the distribution above gives it nothing, but it must be output to turn the laser off
    if (pointsAllocated.back() == 0)
        pointsAllocated.back() = 1;

    // build a list of points ready for output using the list of lines. Apply the optimization parameters
    // accel - first 80% of a line ; deaccel - last 20% of a line
    std::vector<AmbiguousPoint> points;

    auto easeOut = [](int stepToReturn, int amountOfSteps, float strength) -> float
    {
        if (amountOfSteps <= 0) return 1.0f;
        float t = static_cast<float>(stepToReturn) / static_cast<float>(amountOfSteps);
        return 1.0f - std::pow(1.0f - t, strength);
    };

    auto easeIn = [](int stepToReturn, int amountOfSteps, float strength) -> float
    {
        if (amountOfSteps <= 0) return 1.0f;
        float t = static_cast<float>(stepToReturn) / static_cast<float>(amountOfSteps);
        return std::pow(t, strength);
    };

    constexpr int blankDwellPoints = 3;

    for (int i = 0; i < lines.size(); ++i) {
        if (colors[i].a == 0) {
            Vec2F lineVec = lines[i].end - lines[i].start;

            // zero length blank lines (the final blank point) are not travel, so they don't get dwell
            bool isTravel = distance(lines[i].start, lines[i].end) > Epsilon;

            // park at the start of the blank line, so the laser is off before the galvo moves
            if (isTravel)
                for (int d = 0; d < blankDwellPoints; ++d)
                    points.emplace_back(lines[i].start, colors[i]);

            for (int p = 0; p < pointsAllocated[i]; ++p) {
                points.emplace_back(lines[i].start + lineVec * (static_cast<float>(p) / static_cast<float>(pointsAllocated[i])), colors[i]);
            }

            // park at the end of the blank line, so the galvo has arrived before the next line lights up
            if (isTravel)
                for (int d = 0; d < blankDwellPoints; ++d)
                    points.emplace_back(lines[i].end, colors[i]);
        }
        else {
            // gotta refactor later to save points allocated saparate for accel and deccel. splitting 50/50 for now
            // (now split by accelPointsShare so galvo speed at accel exit == speed at deccel start)
            int totalPointsForLine = static_cast<int>(pointsAllocated[i]);
            int pointsForAccel = static_cast<int>(std::round(static_cast<float>(totalPointsForLine) * accelPointsShare));

            // keep at least 1 accel point so the line start (the corner) is always hit, and at least 1 deccel point when there's room
            if (totalPointsForLine >= 2) {
                if (pointsForAccel < 1) pointsForAccel = 1;
                if (pointsForAccel > totalPointsForLine - 1) pointsForAccel = totalPointsForLine - 1;
            }
            int pointsForDeccel = totalPointsForLine - pointsForAccel;

            Vec2F lineVec = lines[i].end - lines[i].start;
            for (int p = 0; p < pointsForAccel; p++) {
                Vec2F pos = lines[i].start + lineVec * (easeIn(p, pointsForAccel, accelStrength) * accelRatio);
                points.emplace_back(pos, colors[i]);
            }

            for (int p = 0; p < pointsForDeccel; p++) {
                Vec2F pos = lines[i].start + lineVec * ((easeOut(p, pointsForDeccel, deccelStrength) * (deccelRatio)) + accelRatio);
                points.emplace_back(pos, colors[i]);
            }

            // the deccel curve stops just short of the line end, so when a blank line follows (end of a shape),
            // place two lit points exactly on the corner. This makes the shape's last corner complete
            if (i + 1 < lines.size() && colors[i+1].a == 0) {
                points.emplace_back(lines[i].end, colors[i]);
                points.emplace_back(lines[i].end, colors[i]);
            }
        }
    }

    if (points.empty()) return {{galvoState.currentPos, Blank}};

    galvoState.currentPos = {points.back().pos.x, points.back().pos.y};

    return points;
}

void Projector::writeFrameToDAC(std::vector<AmbiguousPoint>& points, unsigned PPS) {
    Vec2F outputBounds; // x is lower bound, y is lower bound. Canvas size is a single number as it is a perfect square

    if(this->currentDeviceDAC == HELIOS || this->currentDeviceDAC == HELIOS_HQ) {
        auto start = std::chrono::steady_clock::now();
        while(heliosDac.GetStatus(deviceID) != 1) {
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
                outputBounds = {0.f, 4095.f};
                point.pos /= CanvasSize / (outputBounds.y - outputBounds.x);
            }
            if(this->currentDeviceDAC == HELIOS_HQ) {
                outputBounds = {INT16_MIN, INT16_MAX};
                point.pos /= CanvasSize / (outputBounds.y - outputBounds.x);
            }

            p.x = static_cast<uint16_t>(std::clamp(std::round(point.pos.x), outputBounds.x, outputBounds.y));
            p.y = static_cast<uint16_t>(std::clamp(std::round(point.pos.y), outputBounds.x, outputBounds.y));

            p.r = point.color.r;
            p.g = point.color.g;
            p.b = point.color.b;
            p.i = point.color.a;

            output.emplace_back(p);
        }

        this->heliosDac.WriteFrame(deviceID, PPS, 0, output.data(), output.size());
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
