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

#ifndef SUBGAME_MAIN_DOOR_H_
#define SUBGAME_MAIN_DOOR_H_

#include "subgame/main/machine.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class Door final : public Machine
{
public:
    Door() : Machine(-0.9, -0.2, -0.8, 0.8, VectorF(-0.2, 0, 0), false)
    {
    }
    MonitoredBool open;

    virtual void move(double deltaTime) override
    {
        bool powered = transferFunction();
        if(powered != open.get())
            open.set(powered);
    }

protected:
    virtual bool transferFunction() override
    {
        if(inputs.size() > 0)
        {
            return inputs.back().powered->get();
        }
        return false;
    }
    /**
     * @brief render this element
     * @param renderer the renderer
     * @param minZ the minimum depth to render at
     * @param maxZ the maximum depth to render at (this element can render at minZ <= z < maxZ
     * @param hasFocus if this element currently has focus
     */
    virtual void render(Renderer &renderer, float minZ, float maxZ, bool hasFocus) override
    {
        // rendered by MainGame::clear
    }
    virtual VectorF getInputPosition(std::size_t inputIndex)
    {
        return VectorF(maxX, (minY + maxY) * 0.5f, 0);
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_DOOR_H_ */
