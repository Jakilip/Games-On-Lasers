#pragma once

#include "../Utilities/Core.h"

#include <string>
#include <vector>

namespace GOL {

/// The fundamental class for storing pictures to use in Objects.
///
/// An Graphic contains an array of lines and an array of color channels. Each line maps to a single color channel and derives its color value from it. Its contents can be saved and loaded using ILDA files.
class Graphic {
private:

    friend class Object;
    std::vector<Line> lines;
    std::vector<unsigned> colorChannelMap; //index matches an index of a line. Value matches the index of a color channel.
    std::vector<Color> colors;

public:

    /// @return A copy of the lines inside this graphic
    std::vector<Line> getLines() const;

    /// @return The amount of color channels this graphic contains
    unsigned getColorCount() const;

    /// @return The color stored inside a specific color channel within this graphic
    /// @param channelIndex Index of a color channel
    Color getColor(unsigned channelIndex) const;

    /// @return Index of a color channel where the specific line derives a color from
    /// @param lineIndex Index of a line within this graphic
    unsigned getColorChannel(unsigned lineIndex) const;

    /// @return The rectangular size of this graphic
    Vec2F getSize() const;

    /// Adds a line to this graphic
    /// @param colorChannel The index of a color channel within this graphic. If the channel doesn't exist, new ones are created until reaching the requested index, with the default value of white.
    void addLine(Vec2F start, Vec2F end, unsigned colorChannel);

    /// Adds a line to this graphic
    /// @param colorChannel The index of a color channel within this graphic. If the channel doesn't exist, new ones are created until reaching the requested index, with the default value of white.
    void addLine(Line line, unsigned colorChannel);

    /// Modifies the color value of a specific color channel within this graphic
    /// @param colorChannel The index of a color channel within this graphic. If the channel doesn't exist, new ones are created until reaching the requested index.
    /// The specified channel is created with the color value that was passed, the ones in between will have the default color of white.
    void setColor(Color color, unsigned colorChannel);

    /// Loads the Graphic from an ILDA file (.ild). If the file contains multiple frames, you can pick a specific frame to load.
    /// @param frameIndex Optionally, which frame from the file to load as an Graphic.
    /// @return True on success, False on failure.
    bool loadFromFile(const std::string& filePath, unsigned frameIndex = 0);

    /// Saves the graphic into an ILDA file (.ild)
    /// @return True on success, False on failure.
    bool saveToFile(const std::string& filePath, unsigned ildaFormat = 5) const;

    /// Logs lines and colors in this graphic
    void logContents() const;

    Graphic() = default;
};

}