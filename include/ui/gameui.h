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
#ifndef GAMEUI_H_INCLUDED
#define GAMEUI_H_INCLUDED

#include "ui/ui.h"
#include "platform/platform.h"
#include "util/math_constants.h"
#include "ui/image.h"
#include "texture/texture_atlas.h"
#include "ui/dynamic_label.h"
#include <mutex>
#include <sstream>
#include <deque>
#include <vector>
#include "platform/audio.h"
#include "util/tls.h"
#include "subgame/subgame.h"
#include "game_state/game_state.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace ui
{
class MainMenu;

class GameUi : public Ui
{
    friend class MainMenu;
    GameUi(const GameUi &) = delete;
    GameUi &operator=(const GameUi &) = delete;

private:
    std::shared_ptr<PlayingAudio> playingAudio;
    std::shared_ptr<Ui> playingAudioUi;
    bool addedUi = false;
    std::vector<std::shared_ptr<Ui>> dialogStack;
    std::vector<std::shared_ptr<Ui>> newDialogs;
    std::mutex newDialogsLock;
    std::shared_ptr<Ui> mainMenu;
    std::shared_ptr<Audio> mainMenuSong;
    std::shared_ptr<Subgame> mainGame;
    std::shared_ptr<GameState> gameState;
    void addUi();
    void startMainMenu();
    void clearGame(bool stopSound = true);
    void createNewGame();

public:
    GameUi();
    virtual void move(double deltaTime) override
    {
        {
            std::vector<std::shared_ptr<Ui>> newDialogs;
            std::unique_lock<std::mutex> lockIt(newDialogsLock);
            newDialogs.swap(this->newDialogs);
            lockIt.unlock();
            for(auto &newDialog : newDialogs)
            {
                if(!dialogStack.empty())
                    remove(dialogStack.back());
                dialogStack.push_back(newDialog);
                add(dialogStack.back());
                dialogStack.back()->reset();
            }
        }
        if(!dialogStack.empty())
        {
            setFocus(dialogStack.back());
        }
        Display::grabMouse(
            !dialogStack.empty() && dynamic_cast<const Subgame *>(dialogStack.back().get())
            && std::static_pointer_cast<Subgame>(dialogStack.back())->isMouseGrabbed());
        Ui::move(deltaTime);
        if(gameState)
            gameState->move(deltaTime);
        auto oldTopDialog = dialogStack.empty() ? std::shared_ptr<Ui>() : dialogStack.back();
        for(auto i = dialogStack.begin(); i != dialogStack.end();)
        {
            auto &dialog = *i;
            if(dialog->isDone())
            {
                dialog->handleFinish();
                remove(dialog);
                i = dialogStack.erase(i);
            }
            else
                ++i;
        }
        if(dialogStack.empty())
        {
            Display::grabMouse(false);
            clearGame();
            startMainMenu();
        }
        else if(oldTopDialog != dialogStack.back())
        {
            add(dialogStack.back());
        }
        if(playingAudio && !playingAudio->isPlaying())
        {
            playingAudio = nullptr;
            playingAudioUi = nullptr;
        }
        if(playingAudioUi != (dialogStack.empty() ? nullptr : dialogStack.back()))
        {
            if(!dialogStack.empty() && dynamic_cast<const Subgame *>(dialogStack.back().get()))
            {
                playingAudio =
                    std::static_pointer_cast<Subgame>(dialogStack.back())->startBackgroundMusic();
                playingAudioUi = dialogStack.back();
            }
            else
            {
                playingAudio = mainMenuSong->play();
                playingAudioUi = dialogStack.empty() ? nullptr : dialogStack.back();
            }
        }
    }
    void pushDialog(std::shared_ptr<Ui> newDialog)
    {
        assert(newDialog);
        std::unique_lock<std::mutex> lockIt(newDialogsLock);
        newDialogs.push_back(newDialog);
    }
    virtual bool handleKeyDown(KeyDownEvent &event) override
    {
        if(Ui::handleKeyDown(event))
            return true;
        switch(event.key)
        {
        case KeyboardKey::Escape:
        case KeyboardKey::P:
        {
            startMainMenu();
            return true;
        }
        default:
            return false;
        }
    }
    virtual void reset() override
    {
        if(!addedUi)
        {
            addedUi = true;
            addUi();
        }
        Ui::reset();
    }
    virtual bool handlePause(PauseEvent &event) override
    {
        if(gameState)
            gameState->paused(true);
        return Ui::handlePause(event);
    }
    virtual bool handleResume(ResumeEvent &event) override
    {
        bool retval = Ui::handleResume(event);
        if(gameState)
            gameState->paused(false);
        return retval;
    }

protected:
    virtual void clear(Renderer &renderer) override;
};
}
}
}

#endif // GAMEUI_H_INCLUDED
