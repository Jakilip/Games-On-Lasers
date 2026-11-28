#include <iostream>

#include "GOL.h"

/// OPEN WORLD WITH GOL ///

void doCollisions(GOL::Vec2F moveVec, GOL::Object& player, GOL::Object& map) {
    GOL::Vec2F mapPos = map.getPosition();

    map.setPosition({mapPos.x - moveVec.x, mapPos.y});
    //if(player.collidesWith(map))
        //map.setPosition(mapPos);

    mapPos = map.getPosition();

    map.setPosition({mapPos.x, mapPos.y - moveVec.y});
    //if(player.collidesWith(map))
        //map.setPosition(mapPos);
}

int main() {

    GOL::ControllerManager contrManager;
    if(contrManager.getControllerCount() <= 0) {
        std::cerr << "No controllers connected!";
        return 1;
    }

    // set up connection with the laser projector
    GOL::Projector projector;

    if(!projector.setDevice(0)) {
        std::cerr << "Device invalid!";
        return 1;
    }
    projector.setFramerate(50);
    projector.setScanrate(10000);

    GOL::Graphic mapGraphic;
    mapGraphic.loadFromFile("example.ild");
    //mapGraphic.logContents();
    //return 0;

    GOL::Object map(mapGraphic);
    map.setOrigin({0.1f, 0.1f});
    map.setScale({1.f, 1.f}); // make the map 20k x 20k size

    GOL::Object player(GOL::makeCircle({250.f, 250.f}, 12));
    player.getSourceGraphic().setColor(GOL::Red, 0);
    player.setPosition(GOL::CanvasCenter);

    float playerSpeed = 1000.f; // travels at up to 1000 units per second

    // create timer for measuring time between frames
    GOL::Timer deltaTimer;

    // game loop, pressing touchpad on controller 0 exits
    while(!contrManager.isButtonPressed(0, GOL::ControllerButton::TOUCHPAD)) {
        contrManager.updateInputs();

        float dt = std::min(deltaTimer.getTimeAsSeconds(), 1.f/projector.getFramerate());
        deltaTimer.restart();

        GOL::Vec2F moveVec;
        moveVec.x = playerSpeed * dt * contrManager.getAxisValue(0, GOL::ControllerAxis::LEFT_X);
        moveVec.y = playerSpeed * dt * -contrManager.getAxisValue(0, GOL::ControllerAxis::LEFT_Y);

        std::cout << moveVec << "\n";

        doCollisions(moveVec, player, map);

        projector.draw(map);
        //projector.draw(player);

        projector.pushFrame();
    }

    return 0;
}