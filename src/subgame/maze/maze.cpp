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
        if(std::fabs(deltaPosition.x) > std::fabs(deltaPosition.z))
        {
            if(deltaPosition.x < 0)
                return VectorF(-minDistance - deltaPosition.x, 0, 0);
            return VectorF(minDistance - deltaPosition.x, 0, 0);
        }
        if(deltaPosition.z < 0)
            return VectorF(0, 0, -minDistance - deltaPosition.z);
        return VectorF(0, 0, minDistance - deltaPosition.z);
    }
    if(deltaPosition.x == 0)
    {
        if(deltaPosition.z < 0)
            return VectorF(0, 0, -minDistance - deltaPosition.z);
        return VectorF(0, 0, minDistance - deltaPosition.z);
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
    float turnSpeed = M_PI;
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
    background = RGBF(0, 0, 0);
    Ui::clear(renderer);
    // getDebugLog() << L"<" << position.x << L", " << position.y << L", " << position.z << L">"
    //               << postnl;
    auto tform = Transform::translate(-position).concat(Transform::rotateY(-viewTheta));
    auto positionI = static_cast<VectorI>(position);
    std::int32_t viewDistance = 16;
    VectorI minPosition = positionI - VectorI(viewDistance, 0, viewDistance);
    VectorI maxPosition = positionI + VectorI(viewDistance, 0, viewDistance);
    Mesh mesh, overlayMesh;
    float overlayScale = 0.05f;
    auto overlayTform =
        tform.concat(Transform::rotateX(M_PI / 2)).concat(Transform::scale(overlayScale));
    for(auto p = minPosition; p.z < maxPosition.z; p.z++)
    {
        for(p.x = minPosition.x; p.x < maxPosition.x; p.x++)
        {
            auto cell = mazeMap->get(p.x, p.z);
            TextureDescriptor td, overlayTd;
            TextureDescriptor groundTd = TextureAtlas::MazeWall2.td();
            TextureDescriptor ceilingTd = TextureAtlas::MazeWall2.td();
            switch(cell.type)
            {
            case Cell::Type::Empty:
            case Cell::Type::Start:
                break;
            case Cell::Type::Finish:
                overlayTd = groundTd = TextureAtlas::MazeFinish.td();
                break;
            case Cell::Type::Wall1:
                overlayTd = td = TextureAtlas::MazeWall1.td();
                break;
            case Cell::Type::Wall2:
                overlayTd = td = TextureAtlas::MazeWall2.td();
                break;
            case Cell::Type::Wall3:
                overlayTd = td = TextureAtlas::MazeWall3.td();
                break;
            case Cell::Type::Wall4:
                overlayTd = td = TextureAtlas::MazeWall4.td();
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
            if(overlayTd)
            {
                auto overlayColor = colorizeIdentity();
                overlayMesh.append(transform(overlayTform,
                                             Generate::quadrilateral(overlayTd,
                                                                     VectorF(p.x, 0, p.z + 1),
                                                                     overlayColor,
                                                                     VectorF(p.x + 1, 0, p.z + 1),
                                                                     overlayColor,
                                                                     VectorF(p.x + 1, 0, p.z),
                                                                     overlayColor,
                                                                     VectorF(p.x, 0, p.z),
                                                                     overlayColor)));
            }
        }
    }
    overlayMesh = cutAndGetBack(std::move(overlayMesh), VectorF(1, 0, 0), -1);
    overlayMesh = cutAndGetBack(std::move(overlayMesh), VectorF(-1, 0, 0), -1);
    overlayMesh = cutAndGetBack(std::move(overlayMesh), VectorF(0, 1, 0), -1);
    overlayMesh = cutAndGetBack(std::move(overlayMesh), VectorF(0, -1, 0), -1);
    mesh = lightMesh(std::move(mesh), VectorF(1, 1, 0.5f), 0.4f, 0.6f);
    renderer << transform(tform, mesh);
    float overlaySize = 0.6f;
    float overlayCenterX = minX + overlaySize * 0.5f;
    float overlayCenterY = maxY - overlaySize * 0.5f;
    auto overlayBackgroundColor = GrayscaleF(0);
    auto overlayCursorColor = RGBF(1, 1, 1);
    auto overlayCursorColor2 = RGBF(0, 0, 1);
    Mesh overlayCursorMesh = Generate::quadrilateral(TextureAtlas::Blank.td(),
                                                     VectorF(-0.7f, 0, 0),
                                                     overlayCursorColor,
                                                     VectorF(0, -0.7f, 0),
                                                     overlayCursorColor,
                                                     VectorF(0.7f, 0, 0),
                                                     overlayCursorColor,
                                                     VectorF(0, 0.7f, 0),
                                                     overlayCursorColor);
    overlayCursorMesh.append(Generate::quadrilateral(TextureAtlas::Blank.td(),
                                                     VectorF(-0.4f, 0, 0.05f),
                                                     overlayCursorColor2,
                                                     VectorF(0, -0.4f, 0.05f),
                                                     overlayCursorColor2,
                                                     VectorF(0.4f, 0, 0.05f),
                                                     overlayCursorColor2,
                                                     VectorF(0, 0.4f, 0.05f),
                                                     overlayCursorColor2));
    renderer << start_overlay << reset_render_layer;
    renderer << RenderLayer::Translucent
             << Generate::quadrilateral(TextureAtlas::Blank.td(),
                                        VectorF((overlayCenterX - overlaySize * 0.5f) * 2,
                                                (overlayCenterY - overlaySize * 0.5f) * 2,
                                                -2),
                                        overlayBackgroundColor,
                                        VectorF((overlayCenterX + overlaySize * 0.5f) * 2,
                                                (overlayCenterY - overlaySize * 0.5f) * 2,
                                                -2),
                                        overlayBackgroundColor,
                                        VectorF((overlayCenterX + overlaySize * 0.5f) * 2,
                                                (overlayCenterY + overlaySize * 0.5f) * 2,
                                                -2),
                                        overlayBackgroundColor,
                                        VectorF((overlayCenterX - overlaySize * 0.5f) * 2,
                                                (overlayCenterY + overlaySize * 0.5f) * 2,
                                                -2),
                                        overlayBackgroundColor)
             << transform(
                    Transform::scale(overlaySize * 0.5f)
                        .concat(Transform::translate(VectorF(overlayCenterX, overlayCenterY, -1))),
                    overlayMesh)
             << transform(
                    Transform::scale(overlayScale * overlaySize * 0.5f)
                        .concat(Transform::translate(VectorF(overlayCenterX, overlayCenterY, -1))
                                    .concat(Transform::scale(0.5f))),
                    overlayCursorMesh);
    renderer << start_overlay << reset_render_layer;
}
}
}
}
}

#if 0 // test hit2DBox
#include <iostream>
#include <sstream>
#include <cstdlib>

namespace
{
using namespace programmerjake::game_puzzle;
struct Test
{
    static constexpr int outputSize = 1000;
    static VectorI project(VectorF v)
    {
        return VectorI(VectorF(0.5f) + v * VectorF(outputSize * 0.4f) + VectorF(outputSize * 0.3f));
    }
    Test()
    {
        std::ostringstream ss;
        ss << R"a(<?xml version="1.0"?>
<svg width=")a" << outputSize << R"a(" height=")a" << outputSize
           << R"a(" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<defs>
<marker id="flx" viewBox="0 0 2 2" refX="0" refY="1" markerUnits="strokeWidth" markerWidth="5" markerHeight="5" orient="auto">
<polygon points="0,0 2,1 0,2"/>
</marker>
</defs>
<g fill="none" stroke-width="1" stroke="#000">
<rect x=")a" << project(VectorF(0)).x << R"a(" y=")a" << project(VectorF(0)).z << R"a(" width=")a"
           << (project(VectorF(1)).x - project(VectorF(0)).x) << R"a(" height=")a"
           << (project(VectorF(1)).z - project(VectorF(0)).z) << R"a("/>
<g id="qtr" marker-end="url(#flx)">
)a";
        const int xCount = 80;
        const int yCount = xCount;
        for(int x = 0; x <= xCount; x++)
        {
            for(int y = 0; y <= yCount; y++)
            {
                auto input =
                    VectorF(x * (1.0f / xCount), 0, y * (1.0f / yCount)) * 2.0f - VectorF(0.5f);
                const VectorI boxPosition(3, 4, 5);
                auto output = subgames::maze::MazeGame::hit2DBox(
                                  boxPosition, input + boxPosition, 0.3f) + input;
                auto projectedInput = project(input);
                auto projectedOutput = project(output) - projectedInput;
                ss << "<path d=\"M" << projectedInput.x << "," << projectedInput.z << "l"
                   << projectedOutput.x << "," << projectedOutput.z << "\"/>\n";
            }
        }
        ss << R"a(</g>
<use xlink:href="#qtr" transform="rotate(90)"/>
<use xlink:href="#qtr" transform="rotate(-90)"/>
<use xlink:href="#qtr" transform="scale(-1)"/>
</g>
</svg>)a";
        std::cout << "data:text/html,";
        for(unsigned char ch : ss.str())
        {
            if((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
               || ch == '-'
               || ch == '_'
               || ch == '.'
               || ch == '!'
               || ch == '~'
               || ch == '*'
               || ch == '\''
               || ch == '('
               || ch == ')')
            {
                std::cout << static_cast<char>(ch);
            }
            else
            {
                std::cout << '%' << std::hex << static_cast<int>(ch >> 4)
                          << static_cast<int>(ch & 0xF);
            }
        }
        std::cout << std::endl;
        std::exit(0);
    }
} test;
}
#endif
