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

#ifndef SUBGAME_MAIN_RUN_GAME_H_
#define SUBGAME_MAIN_RUN_GAME_H_

#include "subgame/main/clickable_machine.h"
#include "subgame/subgame.h"
#include "ui/gameui.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class RunGame final : public ClickableMachine
{
private:
    const std::shared_ptr<bool> won = std::make_shared<bool>(false);

public:
    static constexpr float reservedTextHeight = 0.07;
    static constexpr float xSize = 0.2, ySize = xSize + reservedTextHeight;
    const std::shared_ptr<SubgameMaker> subgameMaker;
    std::shared_ptr<GameState> gameState;
    ui::GameUi *const gameUi;
    float eventDelay = 0;
    RunGame(float x,
            float y,
            std::shared_ptr<SubgameMaker> subgameMaker,
            std::shared_ptr<GameState> gameState,
            ui::GameUi *gameUi)
        : ClickableMachine(x - xSize * 0.5f,
                           x + xSize * 0.5f,
                           y - ySize * 0.5f,
                           y + ySize * 0.5f,
                           VectorF(x - xSize * 0.5f, y, 0)),
          subgameMaker(std::move(subgameMaker)),
          gameState(std::move(gameState)),
          gameUi(gameUi)
    {
    }

    virtual void move(double deltaTime) override
    {
        ClickableMachine::move(deltaTime);
        if(eventDelay > 0)
        {
            eventDelay -= deltaTime;
            if(eventDelay <= 0)
            {
                eventDelay = 0;
                auto subgame = subgameMaker->makeSubgame(
                    gameState, gameUi, *won ? std::make_shared<bool>(false) : won);
                if(subgame)
                    gameUi->pushDialog(subgame);
            }
        }
    }

protected:
    virtual void onClick(PlatformEvent &event) override
    {
        eventDelay = 0.2;
    }
    virtual bool transferFunction() override
    {
        return *won;
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
        auto color = RGBF(0.5, 0.4, 0.4);
        auto selectedColor = scaleF(1.1, color);
        const auto textProperties = Text::defaultTextProperties;
        float backgroundZ = 0.5f * (minZ + maxZ);
        if(hasFocus)
            color = selectedColor;
        auto &text = subgameMaker->name;
        float textWidth = Text::width(text, textProperties);
        float textHeight = Text::height(text, textProperties);
        if(textWidth == 0)
            textWidth = 1;
        if(textHeight == 0)
            textHeight = 1;
        float textScale = reservedTextHeight / textHeight;
        textScale = std::min<float>(textScale, (maxX - minX) / textWidth);
        renderer << Generate::quadrilateral(
            subgameMaker->screenshot,
            VectorF(minX * backgroundZ, (minY + reservedTextHeight) * backgroundZ, -backgroundZ),
            color,
            VectorF(maxX * backgroundZ, (minY + reservedTextHeight) * backgroundZ, -backgroundZ),
            color,
            VectorF(maxX * backgroundZ, maxY * backgroundZ, -backgroundZ),
            color,
            VectorF(minX * backgroundZ, maxY * backgroundZ, -backgroundZ),
            color);
        float xOffset = -0.5f * textWidth, yOffset = -0.5f * textHeight;
        xOffset = textScale * xOffset + 0.5f * (minX + maxX);
        yOffset = textScale * yOffset + 0.5f * (minY + minY + reservedTextHeight);
        renderer << transform(Transform::scale(textScale)
                                  .concat(Transform::translate(xOffset, yOffset, -1))
                                  .concat(Transform::scale(minZ)),
                              Text::mesh(text, textColor, textProperties));
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_RUN_GAME_H_ */
