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
#include "ui/gameui.h"
#include "render/generate.h"
#include "texture/texture_atlas.h"
#include "ui/main_menu.h"
#include <vector>
#include "stream/stream.h"
#include "util/logging.h"
#include "platform/thread_name.h"
#include "util/tls.h"
#include "ui/button.h"
#include "subgame/main.h"
#include <memory>

namespace programmerjake
{
namespace game_puzzle
{
namespace ui
{
void GameUi::clear(Renderer &renderer)
{
    if(dialogStack.empty())
    {
        background = GrayscaleF(0.4f);
        Ui::clear(renderer);
        return;
    }
    dialogStack.back()->clear(renderer);
}

void GameUi::clearGame(bool stopSound)
{
    if(!gameState)
        return;
    if(stopSound)
    {
        if(playingAudio)
            playingAudio->stop();
        playingAudio = nullptr;
    }
    for(auto &dialog : dialogStack)
        dialog->quit();
    gameState->paused(true);
    gameState = nullptr;
    mainGame = nullptr;
}

void GameUi::createNewGame()
{
    if(playingAudio)
        playingAudio->stop();
    playingAudio = nullptr;
    gameState = std::make_shared<GameState>();
    mainGame = std::make_shared<MainGame>(gameState);
    pushDialog(mainGame);
}

void GameUi::addUi()
{
    mainMenu = std::make_shared<MainMenu>();
    pushDialog(mainMenu);
}

GameUi::GameUi()
    : playingAudio(),
      newDialogLock(),
      mainMenu(),
      mainMenuSong(std::make_shared<Audio>(L"menu.ogg", true)),
      gameState()
{
}

void GameUi::startMainMenu()
{
    pushDialog(mainMenu);
}
}
}
}
