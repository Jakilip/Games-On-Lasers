#include "Graphic.h"

#include <iostream>
#include <algorithm>

namespace GOL {

std::vector<Line> Graphic::getLines() const { return this->lines; }

unsigned Graphic::getColorChannel(unsigned lineIndex) const { return this->colorChannelMap[lineIndex]; }

unsigned Graphic::getColorCount() const { return this->colors.size(); }

Color Graphic::getColor(unsigned channelIndex) const {
    if(channelIndex >= this->colors.size())
        throw std::runtime_error("Graphic::getColor access out of bounds. Requested index: "+std::to_string(channelIndex)+" but channel count: "+std::to_string(colors.size()));
    return this->colors[channelIndex];
}

Vec2F Graphic::getSize() const {
    Vec2F size = {0.f, 0.f};

    if(lines.empty()) {
        return size;
    }

    constexpr float inf = std::numeric_limits<float>::infinity();

    Vec2F lo = {0, 0}, hi = {0, 0};

    for(const auto& l : lines) {
        lo.x = std::min({lo.x, l.start.x, l.end.x});
        lo.y = std::min({lo.y, l.start.y, l.end.y});
        hi.x = std::max({hi.x, l.start.x, l.end.x});
        hi.y = std::max({hi.y, l.start.y, l.end.y});
    }

    size = {hi.x - lo.x, hi.y - lo.y};

    return size;
}

void Graphic::addLine(Vec2F start, Vec2F end, unsigned colorChannel) {
    this->lines.emplace_back(start, end);
    this->colorChannelMap.push_back(colorChannel);
    if(colorChannel >= this->colors.size()) {
        this->colors.resize(colorChannel+1);
    }
}

void Graphic::addLine(Line line, unsigned colorChannel) {
    this->lines.emplace_back(line);
    this->colorChannelMap.push_back(colorChannel);
    if(colorChannel >= this->colors.size()) {
        this->colors.resize(colorChannel+1);
    }
}

void Graphic::setColor(Color color, unsigned colorChannel) {
    if(colorChannel >= this->getColorCount()) {
        this->colors.resize(colorChannel+1);
    }
    this->colors[colorChannel] = color;
}

void Graphic::logContents() const {
    std::cout << "| Graphic contents |\n\n";
    std::cout << this->lines.size() << " lines: \n\n";
    for(int i = 0; i < this->lines.size(); ++i) {
        std::cout << "Line " << i << " | CC " << this->colorChannelMap[i] << " | " << lines[i].start << " -> " << lines[i].end << "\n";
    }
    std::cout << "\n" << this->colors.size() << " color channels: \n\n";
    for(int i = 0; i < this->colors.size(); ++i) {
        std::cout << "Color Channel " << i << " : " << this->colors[i] << "\n";
    }
}

Graphic::Graphic(const std::vector<Line>& lines) {
    for (auto& l : lines) {
        this->addLine(l, 0);
    }
}

};
