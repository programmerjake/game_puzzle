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

#ifndef SUBGAME_MAZE_MAZE_MAP_H_
#define SUBGAME_MAZE_MAZE_MAP_H_

#include <vector>
#include <cstdint>
#include "stream/iostream.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace maze
{
struct Cell final
{
    enum class Type : std::uint8_t
    {
        Empty,
        Start,
        Finish,
        Wall1,
        Wall2,
        Wall3,
        Wall4,
    };
    Type type;
    static bool canWalkThrough(Type type)
    {
        switch(type)
        {
        case Type::Empty:
        case Type::Start:
        case Type::Finish:
            return true;
        case Type::Wall1:
        case Type::Wall2:
        case Type::Wall3:
        case Type::Wall4:
            return false;
        }
        return false;
    }
    static bool isSolid(Type type)
    {
        switch(type)
        {
        case Type::Empty:
        case Type::Start:
        case Type::Finish:
            return false;
        case Type::Wall1:
        case Type::Wall2:
        case Type::Wall3:
        case Type::Wall4:
            return true;
        }
        return true;
    }
    bool canWalkThrough() const
    {
        return canWalkThrough(type);
    }
    bool isSolid() const
    {
        return isSolid(type);
    }
    Cell(Type type = Type::Empty) : type(type)
    {
    }
};

class MazeMap final
{
private:
    std::vector<Cell> cells;
    std::size_t index(std::size_t x, std::size_t y) const
    {
        return x + width * y;
    }

public:
    const std::size_t width;
    const std::size_t height;
    MazeMap(std::size_t width, std::size_t height) : width(width), height(height)
    {
        cells.resize(width * height);
    }
    Cell get(std::int32_t x, std::int32_t y) const
    {
        if(static_cast<std::uint32_t>(x) >= width || static_cast<std::uint32_t>(y) >= height)
            return Cell();
        return cells[index(x, y)];
    }
    void set(std::int32_t x, std::int32_t y, Cell newValue)
    {
        if(static_cast<std::uint32_t>(x) < width && static_cast<std::uint32_t>(y) < height)
            cells[index(x, y)] = newValue;
    }
    static MazeMap makeRandom(std::uint32_t size, std::uint64_t randomSeed);
    static std::uint64_t makeRandomSeed();
};
}
}
}
}

#endif /* SUBGAME_MAZE_MAZE_MAP_H_ */
