#include "gamestate.h"
#include "../lib/PawPet_SleepyDog.h"

GameState *MenuState::update()
{
    if (g::g_keyReleased & DOWN && index < 5)
    {
        index++;
        redraw = true;
    }
    else if (g::g_keyReleased & UP && index > 0)
    {
        index--;
        redraw = true;
    }
    else if (g::g_keyReleased & BUTTON_P)
    {
        switch (index)
        {
        case 0:
            return new TestGame1();
            break;

        case 1:
            return new AnimationTest();
            break;

        case 2:
            return new SleepScreen();
            break;

        case 3:
            break;

        case 4:
            return new StatsState();
            break;

        default:
            break;
        }
    }

    return this;
}

void MenuState::draw(PetDisplay *disp)
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

TestGame1::TestGame1() : GameState(2), _icons(icons)
{
    score = 0;
    paddleX = 0;

    for (int s = 0; s < 8; s++)
    {
        fallingObjs[s].x = random(0, 8);

        fallingObjs[s].y = -random(0, 32);
    }
}

GameState *TestGame1::update()
{
    if (g::g_keyReleased & LEFT && paddleX > 0)
    {
        paddleX--;
    }
    else if (g::g_keyReleased & RIGHT && paddleX < 7)
    {
        paddleX++;
    }
    else if (g::g_keyReleased & BUTTON_A)
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

void TestGame1::draw(PetDisplay *disp)
{
    disp->setTextSize(1);
    disp->println("test game 1");
    for (int s = 0; s < 8; s++)
    {
        _icons.draw(disp, fallingObjs[s].x * 8, fallingObjs[s].y, 0);
    }

    _icons.draw(disp, paddleX * 8, 54, 11);
    _icons.draw(disp, (paddleX + 1) * 8, 54, 11);
}

StatsState::StatsState() : GameState(3)
{
    tick = k_tickTime1;
}

GameState *StatsState::update()
{
    if (g::g_keyReleased & BUTTON_A)
    {
        return new MenuState();
    }

    redraw = true;

    return this;
}

void StatsState::draw(PetDisplay *disp)
{
    disp->setCursor(0, 8);
    disp->setTextColor(PET_BLACK);

    int32_t ramUsage = ((int32_t)RAM_SIZE - Util::FreeRam()) * 100 / RAM_SIZE;
    disp->printf(" %02u%% %3uv\n", ramUsage, Util::batteryLevel());
    disp->printf(" %uMB %s\n", g::g_stats.flashSize / 1024 / 1024, g::g_stats.filesysFound ? "FAT" : "N/A");
    disp->printf(" %s\n", BUILD_STRING);

    uint8_t cause = Watchdog.resetCause();
    switch (cause)
    {
    case 0x1:
        disp->printf(" %s", "POR");
        break;
    case 0x2:
        disp->printf(" %s", "BOD12");
        break;
    case 0x4:
        disp->printf(" %s", "BOD33");
        break;
    case 0x8:
        disp->printf(" %s", "bit4");
        break;
    case 0x10:
        disp->printf(" %s", "EXT");
        break;
    case 0x20:
        disp->printf(" %s", "WDT");
        break;
    case 0x40:
        disp->printf(" %s", "SYST");
        break;
    case 0x80:
        disp->printf(" %s", "bit8");
        break;
    }
}

AnimationTest::AnimationTest() : GameState(4), petSit(pet_sit)
{
    tick = k_tickTime15;
    curFrame = 0;
    curTick = 0;
    dir = true;
    petSit.loopType = 2;
    petSit.ticksPerFrame = 15;
    // GraphicCache::LoadGraphic("pet_sit", &pm);
}

GameState *AnimationTest::update()
{
    if (g::g_keyReleased & BUTTON_A)
    {
        return new MenuState();
    }

    redraw |= petSit.update(tick);

    return this;
}

void AnimationTest::draw(PetDisplay *disp)
{
    disp->setCursor(0, 8);
    disp->setTextColor(PET_BLACK);

    petSit.draw(disp, 0, 8);
}

SleepScreen::SleepScreen() : GameState(5), pm("sleeptest")
{
    tick = k_tickTime30;
}

GameState *SleepScreen::update()
{
    if (g::g_keyReleased & BUTTON_A)
    {
        return new MenuState();
    }

    // TODO remove, being used to stress test drawing functions
    redraw = true;

    return this;
}

void SleepScreen::draw(PetDisplay *disp)
{
    pm.draw(disp, 0, 0);
}
