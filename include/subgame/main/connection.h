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

#ifndef SUBGAME_MAIN_CONNECTION_H_
#define SUBGAME_MAIN_CONNECTION_H_

#include "util/monitored_variable.h"
#include "ui/element.h"
#include "util/vector.h"
#include "render/generate.h"
#include "texture/texture_atlas.h"
#include <vector>

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
struct Connection final : public ui::Element
{
    VectorF source;
    std::vector<std::shared_ptr<VectorF>> targets;
    std::shared_ptr<MonitoredBool> powered;
    struct MachineInput final
    {
        std::shared_ptr<VectorF> position;
        std::shared_ptr<const MonitoredBool> powered;
        MachineInput(std::shared_ptr<VectorF> position, std::shared_ptr<MonitoredBool> powered)
            : position(std::move(position)), powered(std::move(powered))
        {
        }
    };
    MachineInput addInput(VectorF position)
    {
        MachineInput retval(std::make_shared<VectorF>(position), powered);
        targets.push_back(retval.position);
        return retval;
    }
    static constexpr float lineWidth = 0.01f;
    virtual bool isPointInside(float x, float y) const override
    {
        return false;
    }
    Connection(VectorF source, bool powered = false)
        : Element(0, 0, 0, 0),
          source(source),
          targets(),
          powered(std::make_shared<MonitoredBool>(powered))
    {
    }
    virtual void layout() override
    {
        minX = source.x;
        maxX = source.x;
        minY = source.y;
        maxY = source.y;
        source.z = 0;
        for(auto &target : targets)
        {
            target->z = 0;
            if(minX > target->x)
                minX = target->x;
            if(maxX < target->x)
                maxX = target->x;
            if(minY > target->y)
                minY = target->y;
            if(maxY < target->y)
                maxY = target->y;
        }
        minX -= lineWidth;
        maxX += lineWidth;
        minY -= lineWidth;
        maxY += lineWidth;
    }
    static ColorF getLineColor(bool powered)
    {
        if(powered)
            return RGBF(0, 1, 0);
        return RGBF(0.1, 0.1, 0.1);
    }
    static void renderLine(
        Renderer &renderer, float minZ, float maxZ, VectorF source, VectorF target, ColorF color)
    {
        source.z = 0;
        target.z = 0;
        VectorF displacement = target - source;
        float displacementMagnitude = abs(displacement);
        if(displacementMagnitude < lineWidth * 0.1f)
        {
            renderer << Generate::quadrilateral(
                TextureAtlas::Blank.td(),
                VectorF((target.x - lineWidth) * minZ, (target.y - lineWidth) * minZ, -minZ),
                color,
                VectorF((target.x + lineWidth) * minZ, (target.y - lineWidth) * minZ, -minZ),
                color,
                VectorF((target.x + lineWidth) * minZ, (target.y + lineWidth) * minZ, -minZ),
                color,
                VectorF((target.x - lineWidth) * minZ, (target.y + lineWidth) * minZ, -minZ),
                color);
        }
        else
        {
            VectorF xDir =
                cross(displacement, VectorF(0, 0, 1)) * (lineWidth / displacementMagnitude);
            VectorF start = source - displacement * (lineWidth / displacementMagnitude);
            VectorF yDir =
                displacement * ((displacementMagnitude + 2.0f * lineWidth) / displacementMagnitude);
            auto p1 = start - xDir;
            auto p2 = start + xDir;
            auto p3 = start + xDir + yDir;
            auto p4 = start - xDir + yDir;
            renderer << Generate::quadrilateral(TextureAtlas::Blank.td(),
                                                VectorF(p1.x * minZ, p1.y * minZ, -minZ) * minZ,
                                                color,
                                                VectorF(p2.x * minZ, p2.y * minZ, -minZ) * minZ,
                                                color,
                                                VectorF(p3.x * minZ, p3.y * minZ, -minZ) * minZ,
                                                color,
                                                VectorF(p4.x * minZ, p4.y * minZ, -minZ) * minZ,
                                                color);
        }
    }

protected:
    /**
     * @brief render this element
     * @param renderer the renderer
     * @param minZ the minimum depth to render at
     * @param maxZ the maximum depth to render at (this element can render at minZ <= z < maxZ
     * @param hasFocus if this element currently has focus
     */
    virtual void render(Renderer &renderer, float minZ, float maxZ, bool hasFocus) override
    {
        auto color = getLineColor(powered->get());
        for(auto &target : targets)
        {
            renderLine(renderer, minZ, maxZ, source, *target, color);
        }
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_CONNECTION_H_ */
