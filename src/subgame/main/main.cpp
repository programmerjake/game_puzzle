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
#include "subgame/main/main.h"
#include "util/math_constants.h"
#include <algorithm>
#include "subgame/maze/maze.h"
#include "texture/texture_atlas.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
MainGame::MainGame(std::shared_ptr<GameState> gameState, ui::GameUi *gameUi)
    : Subgame(std::move(gameState), gameUi, false, RGBF(0.75, 0.75, 0.75)),
      backgroundMusic(std::make_shared<Audio>(L"main.ogg", true)),
      doorOpenSound(std::make_shared<Audio>(L"door_open.ogg")),
      door(std::make_shared<Door>()),
      instructionsLabel(std::make_shared<ui::Label>(
          L"Try to solve the puzzle by turning on the power to open the door.",
          -1.0f,
          1.0f,
          0,
          0.05f,
          RGBF(1, 0, 0)))
{
    addMachine(door);
    auto toggleSwitch1 = std::make_shared<ToggleSwitch>(0.6f, -0.3f);
    addMachine(toggleSwitch1);
    auto toggleSwitch2 = std::make_shared<RunGame>(
        0.6f, 0, std::make_shared<maze::MazeGameMaker>(), gameState, gameUi);
    addMachine(toggleSwitch2);
    auto toggleSwitch3 = std::make_shared<ToggleSwitch>(0.6f, 0.3);
    addMachine(toggleSwitch3);
    auto andGate = std::make_shared<AndGate>(0, 0);
    addMachine(andGate);
    auto orGate = std::make_shared<OrGate>(0.3f, 0);
    addMachine(orGate);
    door->addInput(andGate->output);
    andGate->addInput(toggleSwitch1->output);
    andGate->addInput(orGate->output);
    orGate->addInput(toggleSwitch2->output);
    orGate->addInput(toggleSwitch3->output);
}

void MainGame::clear(Renderer &renderer)
{
    Ui::clear(renderer);
    auto td = TextureAtlas::Steel.td();
    auto color = GrayscaleF(0.4f);
    auto lightDirection = VectorF(5, 5, 3);
    float lightIntensity = 0.5;
    float ambientIntensity = 0.5;
    renderer << lightMesh(
        reverse(colorize(color,
                         transform(Transform::scale(1.95f, 1.95f, 2)
                                       .concat(Transform::translate(-0.975f, -0.975f, -1)),
                                   Generate::unitBox(TextureAtlas::Blank.td(),
                                                     TextureAtlas::Blank.td(),
                                                     TextureAtlas::Blank.td(),
                                                     TextureAtlas::Blank.td(),
                                                     TextureAtlas::Blank.td(),
                                                     TextureAtlas::Blank.td())))),
        lightDirection,
        lightIntensity,
        ambientIntensity);
    auto steelBox = Generate::unitBox(td, td, td, td, td, td);
    auto horizontalSteelBox = transform(Transform::translate(VectorF(-0.5f))
                                            .concat(Transform::rotateZ(M_PI_2))
                                            .concat(Transform::translate(VectorF(0.5f))),
                                        steelBox);
    const float doorSideSize = 0.1;
    const float doorThickness = 0.02;
    float doorXSize = door->maxX - door->minX;
    float doorYSize = door->maxY - door->minY;
    renderer << lightMesh(
        colorize(RGBF(0.7f, 0.8f, 0.9f),
                 transform(Transform::scale(
                               doorXSize - doorSideSize, doorYSize - doorSideSize, doorThickness)
                               .concat(Transform::translate(0, 0, -doorThickness))
                               .concat(Transform::rotateY(std::max(0.0f, doorPosition) * -M_PI_2))
                               .concat(Transform::translate(door->minX + doorSideSize * 0.5f,
                                                            door->minY + doorSideSize * 0.5f,
                                                            0.5f * doorThickness - 1)),
                           steelBox)),
        lightDirection,
        lightIntensity,
        ambientIntensity);
    renderer << Generate::quadrilateral(TextureAtlas::Blank.td(),
                                        VectorF(door->minX, door->minY, doorThickness / 4 - 1),
                                        GrayscaleF(0),
                                        VectorF(door->maxX, door->minY, doorThickness / 4 - 1),
                                        GrayscaleF(0),
                                        VectorF(door->maxX, door->maxY, doorThickness / 4 - 1),
                                        GrayscaleF(0),
                                        VectorF(door->minX, door->maxY, doorThickness / 4 - 1),
                                        GrayscaleF(0));
    renderer << lightMesh(
        transform(Transform::scale(doorSideSize, doorYSize + doorSideSize, doorSideSize)
                      .concat(Transform::translate(door->minX - doorSideSize * 0.5f,
                                                   door->minY - doorSideSize * 0.5f,
                                                   -0.5f * doorSideSize - 1)),
                  steelBox),
        lightDirection,
        lightIntensity,
        ambientIntensity);
    renderer << lightMesh(
        transform(Transform::scale(doorSideSize, doorYSize + doorSideSize, doorSideSize)
                      .concat(Transform::translate(door->maxX - doorSideSize * 0.5f,
                                                   door->minY - doorSideSize * 0.5f,
                                                   -0.5f * doorSideSize - 1)),
                  steelBox),
        lightDirection,
        lightIntensity,
        ambientIntensity);
    renderer << lightMesh(
        transform(Transform::scale(doorXSize - doorSideSize, doorSideSize, doorSideSize)
                      .concat(Transform::translate(door->minX + doorSideSize * 0.5f,
                                                   door->minY - doorSideSize * 0.5f,
                                                   -0.5f * doorSideSize - 1)),
                  horizontalSteelBox),
        lightDirection,
        lightIntensity,
        ambientIntensity);
    renderer << lightMesh(
        transform(Transform::scale(doorXSize - doorSideSize, doorSideSize, doorSideSize)
                      .concat(Transform::translate(door->minX + doorSideSize * 0.5f,
                                                   door->maxY - doorSideSize * 0.5f,
                                                   -0.5f * doorSideSize - 1)),
                  horizontalSteelBox),
        lightDirection,
        lightIntensity,
        ambientIntensity);
    renderer << start_overlay << reset_render_layer;
}
}
}
}
}
