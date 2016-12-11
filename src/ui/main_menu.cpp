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
#include "ui/main_menu.h"
#include "ui/button.h"
#include "ui/checkbox.h"
#include "platform/platform.h"
#include "stream/stream.h"
#include "util/logging.h"
#include "ui/label.h"
#include "ui/shaded_container.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace ui
{
std::shared_ptr<Element> MainMenu::setupMainMenu()
{
    add(std::make_shared<BackgroundElement>());
    std::shared_ptr<GameUi> gameUi = std::dynamic_pointer_cast<GameUi>(get(shared_from_this()));
    std::shared_ptr<Button> newGameButton =
        std::make_shared<Button>(L"New Game", -0.8f, 0.8f, 0.7f, 0.95f);
    add(newGameButton);
    newGameButton->click.bind([this](EventArguments &) -> Event::ReturnType
                              {
                                  std::shared_ptr<GameUi> gameUi =
                                      std::dynamic_pointer_cast<GameUi>(get(shared_from_this()));
                                  quit();
                                  gameUi->clearGame();
                                  gameUi->createNewGame();
                                  return Event::ReturnType::Propagate;
                              });
    std::shared_ptr<Element> focusedElement = newGameButton;
    if(gameUi->gameState)
    {
        std::shared_ptr<Button> returnToGameButton =
            std::make_shared<Button>(L"Return To Game", -0.8f, 0.8f, 0.1f, 0.35f);
        add(returnToGameButton);
        returnToGameButton->click.bind([this](EventArguments &) -> Event::ReturnType
                                       {
                                           std::shared_ptr<GameUi> gameUi =
                                               std::dynamic_pointer_cast<GameUi>(
                                                   get(shared_from_this()));
                                           quit();
                                           return Event::ReturnType::Propagate;
                                       });
        focusedElement = returnToGameButton;
    }
    std::shared_ptr<Button> settingsButton =
        std::make_shared<Button>(L"Settings", -0.8f, 0.8f, -0.65f, -0.4f);
    add(settingsButton);
    settingsButton->click.bind([this](EventArguments &) -> Event::ReturnType
                               {
                                   menuState = MenuState::SettingsMenu;
                                   needChangeMenuState = true;
                                   return Event::ReturnType::Propagate;
                               });
    std::shared_ptr<Button> quitButton =
        std::make_shared<Button>(L"Quit", -0.8f, 0.8f, -0.95f, -0.7f);
    add(quitButton);
    quitButton->click.bind([this](EventArguments &) -> Event::ReturnType
                           {
                               get(shared_from_this())->quit();
                               return Event::ReturnType::Propagate;
                           });
    return focusedElement;
}

std::shared_ptr<Element> MainMenu::setupSettingsMenu()
{
    add(std::make_shared<BackgroundElement>());
    std::shared_ptr<GameUi> gameUi = std::dynamic_pointer_cast<GameUi>(get(shared_from_this()));
    std::shared_ptr<CheckBox> fullScreenCheckBox =
        std::make_shared<CheckBox>(L"Full Screen", -0.8f, 0.8f, -0.65f, -0.4f);
    add(fullScreenCheckBox);
    CheckBox &fullScreenCheckBoxR = *fullScreenCheckBox;
    fullScreenCheckBox->checked.set(Display::fullScreen());
    fullScreenCheckBox->checked.onChange.bind(
        [&fullScreenCheckBoxR](EventArguments &) -> Event::ReturnType
        {
            Display::fullScreen(fullScreenCheckBoxR.checked.get());
            return Event::ReturnType::Propagate;
        });
    std::shared_ptr<Button> backButton =
        std::make_shared<Button>(L"Back", -0.8f, 0.8f, -0.95f, -0.7f);
    add(backButton);
    backButton->click.bind([this](EventArguments &) -> Event::ReturnType
                           {
                               menuState = MenuState::MainMenu;
                               needChangeMenuState = true;
                               return Event::ReturnType::Propagate;
                           });
    return backButton;
}
}
}
}
