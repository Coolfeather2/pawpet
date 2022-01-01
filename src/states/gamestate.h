#pragma once
#include "../common.h"
#include "../global.h"

#include "../graphics/graphic_manager.h"
#include "../graphics/pet_display.h"
#include "../sounds/pitches.h"

class GameState
{
  public:
    uint32_t id = 0;              // state id for requesting next state
    bool redraw = true;           // redraw of screen requsted
    uint32_t tick = k_tickTime30; // requested tick time of state

    virtual ~GameState(){};

    virtual GameState *update() = 0;
    virtual void draw(PetDisplay *disp) = 0;
};

class MenuState : public GameState
{
  public:
    MenuState()
    {
        id = 1;
    }
    int8_t index = 0;
    const char *items[6] = {"Game 1", "item 2", "item 3", "item 4", "stats", "suspend"};

    GameState *update() override;

    void draw(PetDisplay *disp) override
    {
        if (!redraw)
        {
            return;
        }
        redraw = false;

        disp->setCursor(0, 8);
        disp->setTextSize(1);

        for (uint8_t s = 0; s < 6; s++)
        {
            if (index == s)
            {
                disp->setTextColor(PET_WHITE, PET_BLACK);
            }
            else
            {
                disp->setTextColor(PET_BLACK);
            }

            disp->println(items[s]);
        }
    }
};

class TestGame1 : public GameState
{
  public:
    uint8_t score;

    Point2D fallingObjs[8];
    uint8_t paddleX;

    TestGame1()
    {
        id = 2;

        score = 0;
        paddleX = 0;

        for (int s = 0; s < 8; s++)
        {
            fallingObjs[s].x = random(0, 8);

            fallingObjs[s].y = -random(0, 32);
        }
    }

    GameState *update() override
    {
        if (g::keyReleased & LEFT && paddleX > 0)
        {
            paddleX--;
        }
        else if (g::keyReleased & RIGHT && paddleX < 7)
        {
            paddleX++;
        }
        else if (g::keyReleased & BUTTON_A)
        {
            return new MenuState();
        }

        for (int s = 0; s < 8; s++)
        {
            fallingObjs[s].y += 4;

            if (fallingObjs[s].y > DISP_HEIGHT)
            {

                fallingObjs[s].y = 0;
                fallingObjs[s].x = random(0, 8);
            }
        }

        redraw = true;

        return this;
    }

    void draw(PetDisplay *disp) override
    {
        disp->setTextSize(1);
        disp->println("test game 1");
        for (int s = 0; s < 8; s++)
        {
            disp->drawFrame(PETPIC(icons), fallingObjs[s].x * 8, fallingObjs[s].y, 0);
        }

        disp->drawFrame(PETPIC(icons), paddleX * 8, 64, 11);
        disp->drawFrame(PETPIC(icons), (paddleX + 1) * 8, 64, 11);
    }
};

class StatsState : public GameState
{
  public:
    StatsState()
    {
        id = 3;
        tick = k_tickTime1;
        // g::g_cache->LoadGraphic("testface");
        // g::g_cache->GetGraphic("testface", &testMeta, &testImage);
    }

    // uint32_t* testImage;
    // uint16_t* testMeta;

    GameState *update() override
    {
        if (g::keyReleased & BUTTON_A)
        {
            return new MenuState();
        }

        redraw = true;

        return this;
    }

    void draw(PetDisplay *disp) override
    {
        disp->setCursor(0, 8);
        disp->setTextColor(PET_BLACK);

        int32_t ramUsage = ((int32_t)RAM_SIZE - Util::FreeRam()) * 100 / RAM_SIZE;
        disp->printf(" %02u%% %3uv\n", ramUsage, Util::batteryLevel());
        disp->printf(" %uMB %s\n", g::stats.flashSize / 1024 / 1024, g::stats.filesysFound ? "FAT" : "N/A");
        disp->printf(" %s\n", BUILD_STRING);
    }
};

class AnimationTest : public GameState
{
  public:
    AnimationTest()
    {
        id = 4;
        tick = k_tickTime15;
        // g::g_cache->LoadGraphic("testface");
        // g::g_cache->GetGraphic("testface", &testMeta, &testImage);
    }

    // uint32_t* testImage;
    // uint16_t* testMeta;

    GameState *update() override
    {
        if (g::keyReleased & BUTTON_A)
        {
            return new MenuState();
        }

        redraw = true;

        return this;
    }

    void draw(PetDisplay *disp) override
    {
        disp->setCursor(0, 8);
        disp->setTextColor(PET_BLACK);

        disp->drawImage(PETPIC(sleeptest), 0, 0);
        // disp->drawImage(testImage, testMeta, 20, 30);
        // disp->drawImage(testImage, testMeta, 0, 8 );
    }
};

GameState *MenuState::update()
{
    if (g::keyReleased & DOWN && index < 5)
    {
        index++;
        redraw = true;
    }
    else if (g::keyReleased & UP && index > 0)
    {
        index--;
        redraw = true;
    }
    else if (g::keyReleased & BUTTON_P)
    {
        switch (index)
        {
        case 0:
            return new TestGame1();
            break;

        case 4:
            return new StatsState();
            break;

        case 1:
            return new AnimationTest();
        default:
            break;
        }
    }

    return this;
}