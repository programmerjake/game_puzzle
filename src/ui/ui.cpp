/*
 * Copyright (C) 2012-2016 Jacob R. Lifshay
 * This file is part of GamePuzzle.
 *
 * GamePuzzle is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GamePuzzle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with GamePuzzle; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */
#include "ui/ui.h"
#include "platform/platform.h"
#include "util/logging.h"
#include <thread>
#include <chrono>

namespace programmerjake
{
namespace game_puzzle
{
namespace ui
{
void Ui::clear(Renderer &renderer)
{
    renderer.flush();
    Display::clear(background);
}

void Ui::run(Renderer &renderer)
{
    Display::initFrame();
    reset();
    double doneTime = 0.2;
    while(doneTime > 0)
    {
        Display::handleEvents(shared_from_this());
        move(Display::frameDeltaTime());
        if(isDone())
            doneTime -= Display::frameDeltaTime();
        if(Display::paused())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        Display::initFrame();
        layout();
        clear(renderer);
        render(renderer, 1, 32, true);
        renderer.flush();
        Display::flip(-1);
    }
    handleFinish();
}

bool Ui::handlePause(PauseEvent &event)
{
    bool retval = Container::handlePause(event);
    if(getParent() == nullptr)
    {
        getDebugLog() << L"Pausing graphics" << postnl;
        pauseGraphics();
    }
    return retval;
}

bool Ui::handleResume(ResumeEvent &event)
{
    bool retval = Container::handleResume(event);
    if(getParent() == nullptr)
    {
        getDebugLog() << L"Resuming graphics" << postnl;
        resumeGraphics();
    }
    return retval;
}
}
}
}
