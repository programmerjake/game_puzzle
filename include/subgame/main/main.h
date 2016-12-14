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
#ifndef SUBGAME_MAIN_MAIN_H_
#define SUBGAME_MAIN_MAIN_H_

#include "subgame/subgame.h"
#include "subgame/main/connection.h"
#include "subgame/main/machine.h"
#include "subgame/main/door.h"
#include "subgame/main/toggle_switch.h"
#include "subgame/main/and_gate.h"
#include "subgame/main/or_gate.h"
#include "subgame/main/run_game.h"
#include "ui/label.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class MainGame final : public Subgame
{
private:
    std::shared_ptr<Audio> backgroundMusic;
    std::shared_ptr<Audio> doorOpenSound;
    std::shared_ptr<Door> door;
    std::vector<std::shared_ptr<Machine>> machines;
    std::shared_ptr<ui::Label> instructionsLabel;
    bool didReset = false;
    float doorPosition = -0.4;
    float afterOpenTimeLeft = 1;
    bool doorOpen = false;

public:
    MainGame(std::shared_ptr<GameState> gameState, ui::GameUi *gameUi);
    virtual std::shared_ptr<PlayingAudio> startBackgroundMusic() override
    {
        return backgroundMusic->play();
    }
    void addMachine(std::shared_ptr<Machine> machine)
    {
        machines.push_back(machine);
        if(didReset)
        {
            add(machine);
            add(machine->output);
            machine->output->layout();
        }
    }
    virtual void reset() override
    {
        didReset = true;
        std::vector<std::shared_ptr<Element>> elements(begin(), end());
        for(std::shared_ptr<Element> e : elements)
        {
            remove(e);
        }
        for(auto &machine : machines)
        {
            add(machine);
            add(machine->output);
            machine->output->layout();
        }
        add(instructionsLabel);
        Subgame::reset();
    }
    virtual void layout() override
    {
        instructionsLabel->moveBottomTo(minY);
        Subgame::layout();
    }
    virtual void move(double deltaTime) override
    {
        Subgame::move(deltaTime);
        if(door->open.get())
        {
            if(!doorOpen)
                doorOpenSound->play()->detach();
            doorOpen = true;
        }
        if(doorOpen)
        {
            if(doorPosition < 1)
            {
                doorPosition += deltaTime;
                if(doorPosition > 1)
                    doorPosition = 1;
            }
            else if(afterOpenTimeLeft > 0)
            {
                afterOpenTimeLeft -= deltaTime;
            }
            else
            {
                quit();
            }
        }
    }
    virtual void clear(Renderer &renderer) override;
};
}
}
}
}

#endif /* SUBGAME_MAIN_MAIN_H_ */
