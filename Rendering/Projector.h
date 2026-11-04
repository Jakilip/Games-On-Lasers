#pragma once

#include "Graphic.h"
#include "Object.h"

#include "../DACs/HeliosDAC/HeliosDac.h"

namespace GOL {

/// The all-in-one class for bringing the rendering system's data into real laser display.
/// Provides an easy interface for DAC connection which is universal for any DAC model supported by the library,
/// and a trivial drawing model through draw() and pushFrame().
/// The output is optimized behind the scenes to squeeze the most performance out of your galvos, and it has a few adjustable options so you can find
/// a perfect optimization config for your hardware if you wish.
///
/// At the time of writing this, the supported DACs are as follows:
/// Helios, HeliosPro, HeliosIdn
class Projector{
private:
    // DAC handlers

    enum DAC {
        HELIOS,
        HELIOS_HQ,
        NO_CONNECTION
    };
    HeliosDac dac;
    int validDevicesAmount = 0;
    DAC currentDeviceDAC = NO_CONNECTION;

    // rendering pipeline

    struct GalvoState {
        Vec2F currentPos;
        float angleMoving; // degrees
    };
    struct AmbiguousPoint {
        Vec2F pos;
        Color color;
    };
    std::vector<Graphic> graphics; // stores geometry and colors from draw calls
    GalvoState galvoState = {{0, 0}, 0}; // stores the current state of the galvo between frames
    void writeFrameToDAC(std::vector<AmbiguousPoint>& points, unsigned PPS); // picks the equipped DAC, converts the data into correct format and sends the frame. POINTS MAY GET ALTERED
    std::vector<AmbiguousPoint> buildOptimalOutput();

    // device params
    unsigned scanrate = 15000;
    unsigned framerate = 50;
    unsigned deviceID = UINT_MAX;

    // optimization params
    struct OptimizationConfig {
        float corneringLevel = 4.f;
        float accelerationControl = 2.f;
        float blankingLevel = 2.f;
        bool dynamicFramerate = true;
    };
public:

    /// The parameters for behind the scenes optimization. The values should generally be positive and single digit before the decimal point.
    /// Other values will certainly do weird stuff.
    /// The default values are in order: 4, 2, 2, true
    OptimizationConfig optimizationConfig;

    /// Sets the device to connect with using a device number. If you're connected to a single laser, the correct number for it will almost certainly be 0.
    /// @param deviceID Device number
    /// @returns True if the device is valid, False otherwise
    bool setDevice(unsigned deviceID);

    /// Sets the framerate of the laser projector. Beware, values below 50 will likely result in flickering visible to the human eye.
    /// @param framerate Frames Per Second
    void setFramerate(unsigned framerate);

    /// Sets the Points Per Second scanrate for the laser projector. Generally I'd discourage you from putting high values unless you're sure your galvos are rated for them.
    /// @param scanrate Points Per Second
    void setScanrate(unsigned scanrate);

    /// Closes connection with the DACs. Automatically called when this object is destroyed.
    void closeDAC();

    /// @returns Selected device number if device is valid, UINT_MAX otherwise.
    unsigned getDeviceID();

    /// @returns Selected scanrate in Points Per Second
    unsigned getScanrate();

    /// @returns Selected framerate in Frames Per Second
    unsigned getFramerate();

    /// Adds a copy of an Object to the rendering stack for next frame. Calling this multiple times with the same object will result in it being drawn multiple times.
    /// @param object Object to draw.
    void draw(Object object);

    /// Generates output based on drawn Objects and pushes a physical frame for the DAC to be displayed.
    void pushFrame();

    /// Debug, do not use.
    void DEBUG_optimal_output() {
        buildOptimalOutput();
    }

    Projector();
    ~Projector();
};

}