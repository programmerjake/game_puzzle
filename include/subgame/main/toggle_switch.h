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

#ifndef SUBGAME_MAIN_TOGGLE_SWITCH_H_
#define SUBGAME_MAIN_TOGGLE_SWITCH_H_

#include "subgame/main/clickable_machine.h"
#include <utility>
#include "render/text.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class ToggleSwitch final : public ClickableMachine
{
private:
    bool powered;

public:
    static constexpr float xSize = 0.1, ySize = 0.1;
    ToggleSwitch(float x, float y, bool powered = false)
        : ClickableMachine(x - xSize * 0.5f,
                           x + xSize * 0.5f,
                           y - ySize * 0.5f,
                           y + ySize * 0.5f,
                           VectorF(x - xSize * 0.5f, y, 0),
                           powered),
          powered(powered)
    {
    }

protected:
    virtual void onClick(PlatformEvent &event) override
    {
        powered = !powered;
    }
    virtual bool transferFunction() override
    {
        return powered;
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
        constexpr auto textColor = RGBF(0, 0, 0);
        constexpr auto color = RGBF(0.5, 0.4, 0.4);
        constexpr auto selectedColor = scaleF(1.1, color);
        constexpr auto selectedTextColor = RGBF(0, 0, 0);
        const auto textProperties = Text::defaultTextProperties;
        std::wstring text = powered ? L"on" : L"off";
        float backgroundZ = 0.5f * (minZ + maxZ);
        float spacing = std::min<float>(0.15f * (maxX - minX), 0.25f * (maxY - minY));
        ColorF currentTextColor = textColor;
        ColorF topColor = color;
        if(hasFocus)
        {
            currentTextColor = selectedTextColor;
            topColor = selectedColor;
        }
        ColorF bottomColor = colorize(GrayscaleF(0.7f), topColor);
        float textWidth = Text::width(text, textProperties);
        float textHeight = Text::height(text, textProperties);
        if(textWidth == 0)
            textWidth = 1;
        if(textHeight == 0)
            textHeight = 1;
        float textScale = (maxY - minY - 2 * spacing) / textHeight;
        textScale = std::min<float>(textScale, (maxX - minX - 2 * spacing) / textWidth);
        if(powered)
        {
            std::swap(topColor, bottomColor);
            textScale *= 0.9f;
        }
        renderer << Generate::quadrilateral(
            whiteTexture(),
            VectorF(minX * backgroundZ, minY * backgroundZ, -backgroundZ),
            bottomColor,
            VectorF(maxX * backgroundZ, minY * backgroundZ, -backgroundZ),
            bottomColor,
            VectorF(maxX * backgroundZ, maxY * backgroundZ, -backgroundZ),
            topColor,
            VectorF(minX * backgroundZ, maxY * backgroundZ, -backgroundZ),
            topColor);
        float xOffset = -0.5f * textWidth, yOffset = -0.5f * textHeight;
        xOffset = textScale * xOffset + 0.5f * (minX + maxX);
        yOffset = textScale * yOffset + 0.5f * (minY + maxY);
        renderer << transform(Transform::scale(textScale)
                                  .concat(Transform::translate(xOffset, yOffset, -1))
                                  .concat(Transform::scale(minZ)),
                              Text::mesh(text, currentTextColor, textProperties));
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_TOGGLE_SWITCH_H_ */
