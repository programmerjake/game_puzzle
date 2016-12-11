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

public:
    explicit MainGame(std::shared_ptr<GameState> gameState, ui::GameUi *gameUi)
        : Subgame(std::move(gameState), gameUi),
          backgroundMusic(std::make_shared<Audio>(L"main.ogg", true))
    {
    }
    virtual std::shared_ptr<PlayingAudio> startBackgroundMusic() override
    {
        return backgroundMusic->play();
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_MAIN_H_ */