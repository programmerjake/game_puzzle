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
#include "subgame/maze/maze.h"
#include "texture/texture_atlas.h"
#include "render/generate.h"
#include "util/logging.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace maze
{
VectorF MazeGame::hit2DBox(VectorI boxPosition, VectorF position, float minDistance)
{
    boxPosition.y = 0;
    position.y = 0;
    VectorF closestPoint = position;
    if(closestPoint.x < boxPosition.x)
        closestPoint.x = boxPosition.x;
    else if(closestPoint.x > boxPosition.x + 1)
        closestPoint.x = boxPosition.x + 1;
    if(closestPoint.z < boxPosition.z)
        closestPoint.z = boxPosition.z;
    else if(closestPoint.z > boxPosition.z + 1)
        closestPoint.z = boxPosition.z + 1;
    VectorF deltaPosition = position - closestPoint;
    float deltaPositionAbsSquared = absSquared(deltaPosition);
    if(deltaPositionAbsSquared >= minDistance * minDistance)
        return VectorF(0);
    if(deltaPositionAbsSquared == 0)
    {
        deltaPosition = position - boxPosition - VectorF(0.5f, 0, 0.5f);
        minDistance += 0.5f;
        if(std::fabs(deltaPosition.x) < std::fabs(deltaPosition.z))
        {
            if(deltaPosition.x < 0)
                return VectorF(-minDistance - deltaPosition.x, 0, 0);
            return VectorF(minDistance - deltaPosition.x, 0, 0);
        }
        if(deltaPosition.z < 0)
            return VectorF(-minDistance - deltaPosition.z, 0, 0);
        return VectorF(minDistance - deltaPosition.z, 0, 0);
    }
    if(deltaPosition.x == 0)
    {
        if(deltaPosition.z < 0)
            return VectorF(-minDistance - deltaPosition.z, 0, 0);
        return VectorF(minDistance - deltaPosition.z, 0, 0);
    }
    if(deltaPosition.z == 0)
    {
        if(deltaPosition.x < 0)
            return VectorF(-minDistance - deltaPosition.x, 0, 0);
        return VectorF(minDistance - deltaPosition.x, 0, 0);
    }
    float deltaPositionAbs = std::sqrt(deltaPositionAbsSquared);
    return deltaPosition * ((minDistance - deltaPositionAbs) / deltaPositionAbs);
}
void MazeGame::move(double deltaTime)
{
    Subgame::move(deltaTime);
    deltaViewTheta *= 0.5f;
    viewTheta += deltaViewTheta;
    viewTheta = std::fmod(viewTheta, 2 * M_PI);
    if(*won)
    {
        wonTimeLeft -= deltaTime;
        if(wonTimeLeft <= 0)
            quit();
        return;
    }
    float turnSpeed = M_PI / 2;
    if(leftPressed)
        viewTheta += turnSpeed * deltaTime;
    if(rightPressed)
        viewTheta -= turnSpeed * deltaTime;
    viewTheta = std::fmod(viewTheta, 2 * M_PI);
    VectorF forward(0, 0, deltaTime * -2);
    forward = transform(Transform::rotateY(viewTheta), forward);
    VectorF left = cross(VectorF(0, 1, 0), forward);
    VectorF deltaPosition(0);
    if(upPressed)
        deltaPosition += forward;
    if(downPressed)
        deltaPosition -= forward;
    std::uint32_t stepCount = static_cast<std::uint32_t>(std::ceil(abs(deltaPosition) * 64));
    float minDistance = 0.2f;
    auto searchDistance = VectorF(minDistance + 0.5f, 0, minDistance + 0.5f);
    if(stepCount != 0)
    {
        deltaPosition *= 1.0f / stepCount;
        for(std::uint32_t i = 0; i < stepCount; i++)
        {
            position += deltaPosition;
            auto minPosition = static_cast<VectorI>(position - searchDistance);
            auto maxPosition = static_cast<VectorI>(position + searchDistance + VectorF(1, 0, 1));
            for(auto p = minPosition; p.z < maxPosition.z; p.z++)
            {
                for(p.x = minPosition.x; p.x < maxPosition.x; p.x++)
                {
                    auto cell = mazeMap->get(p.x, p.z);
                    if(!cell.canWalkThrough())
                    {
                        auto fixup = hit2DBox(p, position, minDistance);
                        position += fixup;
                    }
                }
            }
            auto positionI = static_cast<VectorI>(position);
            auto centerCell = mazeMap->get(positionI.x, positionI.z);
            if(centerCell.type == Cell::Type::Finish)
                *won = true;
        }
    }
}

void MazeGame::clear(Renderer &renderer)
{
    background = RGBF(1, 0, 0);
    Ui::clear(renderer);
    getDebugLog() << L"<" << position.x << L", " << position.y << L", " << position.z << L">"
                  << postnl;
    auto tform = Transform::translate(-position).concat(Transform::rotateY(-viewTheta));
    auto positionI = static_cast<VectorI>(position);
    std::int32_t viewDistance = 16;
    VectorI minPosition = positionI - VectorI(viewDistance, 0, viewDistance);
    VectorI maxPosition = positionI + VectorI(viewDistance, 0, viewDistance);
    Mesh mesh;
    for(auto p = minPosition; p.z < maxPosition.z; p.z++)
    {
        for(p.x = minPosition.x; p.x < maxPosition.x; p.x++)
        {
            auto cell = mazeMap->get(p.x, p.z);
            TextureDescriptor td;
            TextureDescriptor groundTd = TextureAtlas::MazeWall2.td();
            TextureDescriptor ceilingTd = TextureAtlas::MazeWall2.td();
            switch(cell.type)
            {
            case Cell::Type::Empty:
            case Cell::Type::Start:
                break;
            case Cell::Type::Finish:
                groundTd = TextureAtlas::MazeFinish.td();
                break;
            case Cell::Type::Wall1:
                td = TextureAtlas::MazeWall1.td();
                break;
            case Cell::Type::Wall2:
                td = TextureAtlas::MazeWall2.td();
                break;
            case Cell::Type::Wall3:
                td = TextureAtlas::MazeWall3.td();
                break;
            case Cell::Type::Wall4:
                td = TextureAtlas::MazeWall4.td();
                break;
            }
            if(td)
            {
                TextureDescriptor nxtd, pxtd, nztd, pztd;
                if(!mazeMap->get(p.x - 1, p.z).isSolid())
                    nxtd = td;
                if(!mazeMap->get(p.x + 1, p.z).isSolid())
                    pxtd = td;
                if(!mazeMap->get(p.x, p.z - 1).isSolid())
                    nztd = td;
                if(!mazeMap->get(p.x, p.z + 1).isSolid())
                    pztd = td;
                mesh.append(transform(
                    Transform::translate(p - VectorF(0, 0.5f, 0)),
                    Generate::unitBox(
                        nxtd, pxtd, TextureDescriptor(), TextureDescriptor(), nztd, pztd)));
            }
            else
            {
                mesh.append(transform(Transform::translate(p - VectorF(0, 0.5f, 0)),
                                      reverse(Generate::unitBox(TextureDescriptor(),
                                                                TextureDescriptor(),
                                                                groundTd,
                                                                ceilingTd,
                                                                TextureDescriptor(),
                                                                TextureDescriptor()))));
            }
        }
    }
    mesh = lightMesh(std::move(mesh), VectorF(1, 1, 0.5f), 0.4f, 0.6f);
    renderer << transform(tform, mesh);
    renderer << start_overlay << reset_render_layer;
}
}
}
}
}
