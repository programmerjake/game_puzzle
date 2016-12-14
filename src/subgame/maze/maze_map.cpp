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
#include "subgame/maze/maze_map.h"
#include "util/vector.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include "util/logging.h"
#include <random>
#include <chrono>

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace maze
{
namespace
{
std::uint64_t makeRandomSeedOffset()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::system_clock::now().time_since_epoch()).count();
}
}
std::uint64_t MazeMap::makeRandomSeed()
{
    // add in an offset in case random_device falls back on a
    // pseudo-random number generator which returns the same sequence every time
    static const std::uint64_t randomSeedOffset = makeRandomSeedOffset();
    std::random_device rd;
    return std::uniform_int_distribution<std::uint64_t>()(rd) + randomSeedOffset;
}

MazeMap MazeMap::makeRandom(std::uint32_t size, std::uint64_t randomSeed)
{
    if(size < 2)
        size = 2;
    struct Random final
    {
        std::uint64_t value;
        explicit Random(std::uint64_t seed) : value(seed ^ (seed >> 32) * 0x6854CBDC51ULL)
        {
        }
        std::uint32_t nextU32()
        {
            value = (value * 0x5DEECE66DULL + 0xB) & ((1ULL << 48) - 1);
            return static_cast<std::uint32_t>(value >> 16);
        }
        std::int32_t nextS32()
        {
            return static_cast<std::int32_t>(nextU32());
        }
        float nextCanonicalFloat()
        {
            return static_cast<float>(
                       nextU32() & static_cast<std::uint32_t>(((1ULL << 23) - 1)
                                                              << (32 - 23))) // leave only 23 bits
                   * static_cast<float>(1.0 / 65536.0 / 65536.0); // divide by pow(2, 32)
        }
        std::uint32_t nextU32(std::uint32_t max) // returns a number in [0, max]
        {
            return static_cast<std::uint64_t>(nextU32()) * max / 0xFFFFFFFF;
        }
        std::int32_t nextS32(std::int32_t max) // returns a number in [0, max]
        {
            return static_cast<std::uint64_t>(nextU32()) * max / 0xFFFFFFFF;
        }
        std::uint32_t nextU32(std::uint32_t min,
                              std::uint32_t max) // returns a number in [min, max]
        {
            return nextU32(max - min) + min;
        }
        std::int32_t nextS32(std::int32_t min, std::int32_t max) // returns a number in [min, max]
        {
            return static_cast<std::int32_t>(nextU32(static_cast<std::uint32_t>(max)
                                                     - static_cast<std::uint32_t>(min))) + min;
        }
    };
    Random random(randomSeed);
#warning finish
    MazeMap retval(size * 2 + 1, size * 2 + 1);
    for(std::size_t y = 0; y < retval.height; y++)
        for(std::size_t x = 0; x < retval.width; x++)
            retval.set(x, y, Cell(Cell::Type::Wall1));
    std::unordered_map<VectorI, std::shared_ptr<std::unordered_set<VectorI>>> sets;
    std::unordered_set<VectorI> edges;
    for(std::size_t x = 0; x < size; x++)
    {
        for(std::size_t y = 0; y < size; y++)
        {
            auto point = VectorI(2 * x + 1, 0, 2 * y + 1);
            auto &set = sets[point];
            set = std::make_shared<std::unordered_set<VectorI>>();
            set->insert(point);
            retval.set(point.x, point.z, Cell::Type::Empty);
            if(x > 0)
                edges.insert(VectorI(2 * x, 0, 2 * y + 1));
            if(y > 0)
                edges.insert(VectorI(2 * x + 1, 0, 2 * y));
        }
    }
    while(!edges.empty())
    {
        std::size_t edgeIndex = random.nextU32(edges.size() - 1);
        auto edgeIter = edges.begin();
        for(std::size_t i = 0; i < edgeIndex; i++)
            ++edgeIter;
        auto edge = *edgeIter;
        edges.erase(edgeIter);
        retval.set(edge.x, edge.z, Cell::Type::Empty);
        VectorI point1, point2;
        if((edge.x % 2) == 0) // is a horizontal edge
        {
            point1 = edge - VectorI(1, 0, 0);
            point2 = edge + VectorI(1, 0, 0);
        }
        else
        {
            point1 = edge - VectorI(0, 0, 1);
            point2 = edge + VectorI(0, 0, 1);
        }
        auto sourceSet = sets[point1];
        auto targetSet = sets[point2];
        for(auto point : *sourceSet)
        {
            for(auto dir : std::initializer_list<VectorI>{
                    VectorI(1, 0, 0), VectorI(-1, 0, 0), VectorI(0, 0, 1), VectorI(0, 0, -1),
                })
            {
                auto edge = point + dir;
                auto targetPoint = point + 2 * dir;
                if(targetSet->count(targetPoint))
                {
                    edges.erase(edge);
                }
            }
            targetSet->insert(point);
            sets[point] = targetSet;
        }
    }
    retval.set(1, 1, Cell(Cell::Type::Start));
    retval.set(retval.width - 2, retval.height - 2, Cell(Cell::Type::Finish));
#if 0
    std::wostringstream ss;
    for(std::size_t y = 0; y < retval.height; y++)
    {
        for(std::size_t x = 0; x < retval.width; x++)
        {
            switch(retval.get(x, y).type)
            {
            case Cell::Type::Empty:
                ss << L' ';
                break;
            case Cell::Type::Start:
                ss << L'S';
                break;
            case Cell::Type::Finish:
                ss << L'F';
                break;
            case Cell::Type::Wall1:
                ss << L'#';
                break;
            case Cell::Type::Wall2:
                ss << L'@';
                break;
            case Cell::Type::Wall3:
                ss << L'%';
                break;
            case Cell::Type::Wall4:
                ss << L'W';
                break;
            }
        }
        ss << L'\n';
    }
    getDebugLog() << ss.str() << postnl;
#endif
    return retval;
}
}
}
}
}
