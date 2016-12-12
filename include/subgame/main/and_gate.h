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

#ifndef SUBGAME_MAIN_AND_GATE_H_
#define SUBGAME_MAIN_AND_GATE_H_

#include "subgame/main/machine.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class AndGate final : public Machine
{
public:
    static constexpr float xSize = 0.1, ySize = 0.15;
    AndGate(float x, float y)
        : Machine(x - xSize * 0.5f,
                  x + xSize * 0.5f,
                  y - ySize * 0.5f,
                  y + ySize * 0.5f,
                  VectorF(x - 0.5f * xSize + Connection::lineWidth, y, 0))
    {
    }

protected:
    virtual bool transferFunction() override
    {
        for(auto &input : inputs)
        {
            if(!input.powered->get())
                return false;
        }
        return true;
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
        bool closed = false;
        if(!inputs.empty() && inputs.front().powered->get())
            closed = true;
        bool powered = true;
        for(std::size_t i = 1; i < inputs.size(); i++)
        {
            if(!inputs[i].powered->get())
            {
                powered = false;
                break;
            }
        }
        auto color = powered ? RGBF(0.9f, 0.3f, 0.3f) : RGBF(0.5f, 0.3f, 0.3f);
        auto source = VectorF(maxX - Connection::lineWidth, (minY + maxY) * 0.5f, 0);
        auto target = VectorF(minX + Connection::lineWidth, (minY + maxY) * 0.5f, 0);
        target = transform(Transform::translate(-source)
                               .concat(Transform::rotateZ(closed ? 0 : M_PI / 6))
                               .concat(Transform::translate(source)),
                           target);
        Connection::renderLine(renderer, minZ, maxZ, source, target, color);
    }
    virtual VectorF getInputPosition(std::size_t inputIndex)
    {
        if(inputIndex == 0)
            return VectorF((minX + maxX) * 0.5f, minY + Connection::lineWidth, 0);
        return VectorF(maxX - Connection::lineWidth, (minY + maxY) * 0.5f, 0);
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_AND_GATE_H_ */
