#include "Projector.h"

#include <iostream>

namespace GOL {

unsigned Projector::getDeviceID() { return this->deviceID; }

unsigned Projector::getFramerate() { return this->framerate; }

unsigned Projector::getScanrate() { return this->scanrate; }

void Projector::setFramerate(unsigned framerate) { this->framerate = framerate; }

void Projector::setScanrate(unsigned scanrate) { this->scanrate = scanrate; }

bool Projector::setDevice(unsigned deviceID) {
    heliosDac.CloseDevices();
    heliosDac.OpenDevices();

    if(validDevicesAmount >= deviceID+1) {
        this->deviceID = deviceID;
        this->currentDeviceDAC = HELIOS;
        if(heliosDac.GetSupportsHigherResolutions(this->deviceID))
            this->currentDeviceDAC = HELIOS_HQ;
    }
    else
        this->deviceID = UINT_MAX;
    return this->deviceID != UINT_MAX;
}

void Projector::draw(Object object) {
    graphics.emplace_back(object.getTransformedGraphic());
}

//LsRenderer::buildOptimalOutput and writeFrameToDAC implemented in isolated implementation because of complexicity

void Projector::pushFrame() {
    if(deviceID == UINT_MAX) {
        std::cerr << "Attempted to push frame with no valid device set\n";
        return;
    }

    //sophisticated bullshit to build the optimal points configuration
    std::vector<AmbiguousPoint> points = this->buildOptimalOutput();

    unsigned PPS = this->framerate * static_cast<unsigned>(points.size());
    if(PPS > scanrate)
        PPS = scanrate;

    this->writeFrameToDAC(points, PPS);

    graphics.clear();
}


Projector::Projector() {
    heliosDac.CloseDevices();
    validDevicesAmount = heliosDac.OpenDevices();
}

Projector::~Projector() {
    heliosDac.CloseDevices();
    std::cout << "Cleaning up helios\n";
}

void Projector::closeDAC() {
    heliosDac.CloseDevices();
    std::cout << "Cleaning up helios\n";
}

}