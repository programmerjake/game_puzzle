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
#ifndef TEXTURE_ATLAS_H_INCLUDED
#define TEXTURE_ATLAS_H_INCLUDED

#include "texture/texture_descriptor.h"
#include "util/checked_array.h"
#include <tuple>

namespace programmerjake
{
namespace game_puzzle
{
struct TextureAtlas final
{
    int left, top, width, height;
    std::size_t textureIndex;

public:
    static const TextureAtlas Blank, Font8x8;

public:
    static Image texture(std::size_t textureIndex);
    float minU() const
    {
        return (left + pixelOffset) / textureXRes(textureIndex);
    }
    float maxU() const
    {
        return (left + width - pixelOffset) / textureXRes(textureIndex);
    }
    float minV() const
    {
        return 1 - (top + height - pixelOffset) / textureYRes(textureIndex);
    }
    float maxV() const
    {
        return 1 - (top + pixelOffset) / textureYRes(textureIndex);
    }
    static int textureXRes(std::size_t textureIndex)
    {
        return textures()[textureIndex].width;
    }
    static int textureYRes(std::size_t textureIndex)
    {
        return textures()[textureIndex].height;
    }
    static constexpr float pixelOffset = 0.05f;
    TextureDescriptor td() const
    {
        return TextureDescriptor(texture(textureIndex), minU(), maxU(), minV(), maxV());
    }
    TextureDescriptor tdNoOffset() const
    {
        return TextureDescriptor(texture(textureIndex),
                                 static_cast<float>(left) / textureXRes(textureIndex),
                                 static_cast<float>(left + width) / textureXRes(textureIndex),
                                 1 - static_cast<float>(top + height) / textureYRes(textureIndex),
                                 1 - static_cast<float>(top) / textureYRes(textureIndex));
    }

private:
    struct ImageDescriptor final
    {
        Image *image;
        const wchar_t *const fileName;
        const int width, height;
        constexpr ImageDescriptor(const wchar_t *fileName, int width, int height)
            : image(nullptr), fileName(fileName), width(width), height(height)
        {
        }
        ImageDescriptor(const ImageDescriptor &) = default;
        ImageDescriptor &operator=(const ImageDescriptor &) = delete;
    };
    static checked_array<ImageDescriptor, 1> &textures();
};
}
}

#endif // TEXTURE_ATLAS_H_INCLUDED
