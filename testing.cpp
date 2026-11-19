#include <iostream>

#include "GOL.h"

int main() {

    GOL::Graphic GOLLogo_Graphic;
    GOL::Graphic letterG_Graphic;
    GOL::Graphic letterO_Graphic;
    GOL::Graphic letterL_Graphic;
    GOL::Graphic davidStar_Graphic;
    GOL::Graphic weirdTriangle_Graphic;
    GOL::Graphic polishFlag_Graphic;
    GOL::Graphic circle_Graphic = GOL::makeCircle({900.f, 900.f});

    GOLLogo_Graphic.loadFromFile("GOLLogo.ild");
    letterG_Graphic.loadFromFile("LetterG.ild");
    letterO_Graphic.loadFromFile("LetterO.ild");
    letterL_Graphic.loadFromFile("LetterL.ild");
    davidStar_Graphic.loadFromFile("davidStar.ild");
    weirdTriangle_Graphic.loadFromFile("weirdTriangle.ild");
    polishFlag_Graphic.loadFromFile("polishFlag.ild");

    GOL::Object GOLLogo(GOLLogo_Graphic);
    GOLLogo.setPosition(GOL::CanvasCenter);
    GOLLogo.setScale({0.f, 0.f});

    GOL::Object letterG(letterG_Graphic);
    GOL::Object letterO(letterO_Graphic);
    GOL::Object letterL(letterL_Graphic);
    letterG.setPosition({GOL::CanvasHalf - 200.f, GOL::CanvasHalf - 300.f});
    letterO.setPosition({GOL::CanvasHalf, GOL::CanvasHalf - 300.f});
    letterL.setPosition({GOL::CanvasHalf + 200.f, GOL::CanvasHalf - 300.f});

    GOL::Object davidStar(davidStar_Graphic);
    GOL::Object weirdTriangle(weirdTriangle_Graphic);
    GOL::Object polishFlag(polishFlag_Graphic);
    GOL::Object circle(circle_Graphic);

    GOL::Projector projector;
    if (!projector.setDevice(0)) {
        std::cout << "No devices found";
        return -1;
    }
    projector.setFramerate(50);
    projector.setScanrate(14000);

    GOL::Timer elapsedTimer;
    GOL::Timer dtTimer;
    float elapsed = 0.f, dt;

    while (elapsed < 10.f) {

        elapsed = elapsedTimer.getTimeAsSeconds();
        dt = dtTimer.getTimeAsSeconds();
        dtTimer.restart();

        if (elapsed >= 1.f && elapsed <= 10.f) {
            float scale = std::min((elapsed - 1.f) / 2.f, 1.f);
            GOLLogo.setScale({scale, scale});
            projector.draw(GOLLogo);

            if (elapsed >= 4.5f)
                projector.draw(letterG);
            if (elapsed >= 5.25f)
                projector.draw(letterO);
            if (elapsed >= 6.0f)
                projector.draw(letterL);
        }

        projector.pushFrame();
    }
};