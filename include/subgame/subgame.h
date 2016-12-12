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
#ifndef SUBGAME_H_
#define SUBGAME_H_

#include "ui/ui.h"
#include "game_state/game_state.h"
#include "platform/audio.h"
#include "texture/texture_descriptor.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace ui
{
class GameUi;
}

class Subgame : public ui::Ui
{
private:
    bool mouseGrabbed;

protected:
    const std::shared_ptr<GameState> gameState;
    ui::GameUi *const gameUi;

public:
    Subgame(std::shared_ptr<GameState> gameState,
            ui::GameUi *gameUi,
            bool mouseGrabbed = true,
            ColorF background = GrayscaleF(0.4))
        : Ui(background),
          mouseGrabbed(mouseGrabbed),
          gameState(std::move(gameState)),
          gameUi(gameUi)
    {
    }
    bool isMouseGrabbed() const
    {
        return mouseGrabbed;
    }
    virtual std::shared_ptr<PlayingAudio> startBackgroundMusic() = 0;
};

class SubgameMaker
{
public:
    const TextureDescriptor screenshot;
    const std::wstring name;
    SubgameMaker(TextureDescriptor screenshot, std::wstring name)
        : screenshot(screenshot), name(name)
    {
    }
    virtual ~SubgameMaker() = default;
    virtual std::shared_ptr<Subgame> makeSubgame(std::shared_ptr<GameState> gameState,
                                                 ui::GameUi *gameUi,
                                                 std::shared_ptr<bool> result) const
    {
        return nullptr;
    }
};
}
}

#endif /* SUBGAME_H_ */
