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

#ifndef SUBGAME_MAZE_MAZE_H_
#define SUBGAME_MAZE_MAZE_H_

#include "subgame/subgame.h"
#include "subgame/maze/maze_map.h"
#include "util/math_constants.h"
#include <algorithm>
#include "texture/texture_atlas.h"
#include "util/game_version.h"
#include "ui/label.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace maze
{
class MazeGame final : public Subgame
{
private:
    std::shared_ptr<Audio> backgroundMusic;
    std::shared_ptr<const MazeMap> mazeMap;
    std::shared_ptr<bool> won;
    VectorF position;
    bool leftPressed = false;
    bool rightPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    float deltaViewTheta = 0;
    float viewTheta = 0;
    float wonTimeLeft = 1;
    std::shared_ptr<ui::Label> instructionsLabel;

public:
    MazeGame(std::shared_ptr<GameState> gameState,
             ui::GameUi *gameUi,
             std::shared_ptr<const MazeMap> mazeMap,
             std::shared_ptr<bool> won)
        : Subgame(std::move(gameState), gameUi, !GameVersion::DEBUG),
          backgroundMusic(std::make_shared<Audio>(L"maze.ogg", true)),
          mazeMap(std::move(mazeMap)),
          won(std::move(won)),
          position(this->mazeMap->width / 2 + 0.5f, 0, this->mazeMap->height / 2 + 0.5f),
          instructionsLabel(std::make_shared<ui::Label>(
              L"Use the arrow keys to move. Press escape to return from subgame.",
              -1.0f,
              1.0f,
              0,
              0.05f,
              RGBF(1, 0, 0)))
    {
        for(std::size_t y = 0; y < this->mazeMap->height; y++)
        {
            for(std::size_t x = 0; x < this->mazeMap->width; x++)
            {
                if(this->mazeMap->get(x, y).type == Cell::Type::Start)
                {
                    position = VectorF(x + 0.5f, 0, y + 0.5f);
                    return;
                }
            }
        }
    }
    virtual bool handleKeyUp(KeyUpEvent &event) override
    {
        if(Subgame::handleKeyUp(event))
            return true;
        switch(event.key)
        {
        case KeyboardKey::Left:
            leftPressed = false;
            return true;
        case KeyboardKey::Right:
            rightPressed = false;
            return true;
        case KeyboardKey::Up:
            upPressed = false;
            return true;
        case KeyboardKey::Down:
            downPressed = false;
            return true;
        default:
            return false;
        }
        return false;
    }
    virtual bool handleKeyDown(KeyDownEvent &event) override
    {
        if(Subgame::handleKeyDown(event))
            return true;
        switch(event.key)
        {
        case KeyboardKey::Escape:
            quit();
            return true;
        case KeyboardKey::Left:
            leftPressed = true;
            return true;
        case KeyboardKey::Right:
            rightPressed = true;
            return true;
        case KeyboardKey::Up:
            upPressed = true;
            return true;
        case KeyboardKey::Down:
            downPressed = true;
            return true;
        default:
            return false;
        }
    }
    virtual bool handleMouseMove(MouseMoveEvent &event) override
    {
        if(Subgame::handleMouseMove(event))
            return true;
        if(!*won)
            deltaViewTheta += event.deltaX * -1.5f * Display::scaleX() / Display::width();
        return true;
    }
    static VectorF hit2DBox(VectorI boxPosition, VectorF position, float minDistance);
    virtual void move(double deltaTime) override;
    virtual void clear(Renderer &renderer) override;
    virtual std::shared_ptr<PlayingAudio> startBackgroundMusic() override
    {
        return backgroundMusic->play();
    }
    virtual void layout() override
    {
        instructionsLabel->moveBottomTo(minY);
        Subgame::layout();
    }
    virtual void reset() override
    {
        remove(instructionsLabel);
        add(instructionsLabel);
        Subgame::reset();
    }
};

class MazeGameMaker final : public SubgameMaker
{
public:
    const std::uint64_t mazeSeed;
    explicit MazeGameMaker(std::uint64_t mazeSeed = maze::MazeMap::makeRandomSeed())
        : SubgameMaker(TextureAtlas::MazeScreenshot.td(), L"maze"), mazeSeed(mazeSeed)
    {
    }
    virtual std::shared_ptr<Subgame> makeSubgame(std::shared_ptr<GameState> gameState,
                                                 ui::GameUi *gameUi,
                                                 std::shared_ptr<bool> result) const override
    {
        return std::make_shared<maze::MazeGame>(
            gameState,
            gameUi,
            std::make_shared<maze::MazeMap>(maze::MazeMap::makeRandom(8, mazeSeed)),
            result);
    }
};
}
}
}
}


#endif /* SUBGAME_MAZE_MAZE_H_ */
