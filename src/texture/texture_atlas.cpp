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
#include "texture/texture_atlas.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include "util/string_cast.h"
#include "util/logging.h"

using namespace std;

namespace programmerjake
{
namespace game_puzzle
{
namespace
{
Image loadImage(std::wstring name)
{
    try
    {
        getDebugLog() << L"loading '" << name << "'..." << postnl;
        Image retval = Image(name);
        return retval;
    }
    catch(exception &e)
    {
        getDebugLog() << L"error: " << string_cast<std::wstring>(e.what()) << postnl;
        exit(1);
    }
}
}

checked_array<TextureAtlas::ImageDescriptor, TextureAtlas::textureCount> &TextureAtlas::textures()
{
    static checked_array<TextureAtlas::ImageDescriptor, textureCount> retval = {
        TextureAtlas::ImageDescriptor(L"textures.png", 256, 256),
        TextureAtlas::ImageDescriptor(L"steel.png", 256, 256),
    };
    return retval;
}

Image TextureAtlas::texture(std::size_t textureIndex)
{
    auto &textures_array = textures();
    ImageDescriptor &t = textures_array[textureIndex];
    if(t.image == nullptr)
    {
        for(std::size_t i = textures_array.size() - 1, j = i + 1; j > 0; i--, j--)
        {
            textures_array[i].image = new Image(loadImage(textures_array[i].fileName));
        }
    }
    return *t.image;
}

const TextureAtlas TextureAtlas::Font8x8 = {0, 0, 128, 128, 0},
        TextureAtlas::Blank = {90, 106, 4, 4, 0},
        TextureAtlas::Steel = {0, 0, 256, 256, 1};
}
}
