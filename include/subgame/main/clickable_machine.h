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

#ifndef SUBGAME_MAIN_CLICKABLE_MACHINE_H_
#define SUBGAME_MAIN_CLICKABLE_MACHINE_H_

#include "subgame/main/machine.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class ClickableMachine : public Machine
{
public:
    ClickableMachine(
        float minX, float maxX, float minY, float maxY, VectorF source, bool powered = false)
        : Machine(minX, maxX, minY, maxY, source, powered)
    {
    }
    virtual bool canHaveKeyboardFocus() const override
    {
        return true;
    }
    virtual bool handleTouchDown(TouchDownEvent &event) override
    {
        onClick(event);
        return true;
    }
    virtual bool handleTouchMove(TouchMoveEvent &event) override
    {
        return true;
    }
    virtual bool handleMouseUp(MouseUpEvent &event) override
    {
        return true;
    }
    virtual bool handleMouseDown(MouseDownEvent &event) override
    {
        onClick(event);
        return true;
    }
    virtual bool handleMouseMove(MouseMoveEvent &event) override
    {
        return true;
    }
    virtual bool handleKeyUp(KeyUpEvent &event) override
    {
        if(event.key == KeyboardKey::Space || event.key == KeyboardKey::Return)
            return true;
        return false;
    }
    virtual bool handleKeyDown(KeyDownEvent &event) override
    {
        if(event.key == KeyboardKey::Space || event.key == KeyboardKey::Return)
        {
            onClick(event);
            return true;
        }
        return false;
    }
    virtual bool handleMouseMoveOut(MouseEvent &event) override
    {
        return true;
    }
    virtual bool handleMouseMoveIn(MouseEvent &event) override
    {
        return true;
    }
    virtual bool handleTouchMoveOut(TouchEvent &event) override
    {
        return true;
    }
    virtual bool handleTouchMoveIn(TouchEvent &event) override
    {
        return true;
    }

protected:
    virtual void onClick(PlatformEvent &event) = 0;
};
}
}
}
}

#endif /* SUBGAME_MAIN_CLICKABLE_MACHINE_H_ */
