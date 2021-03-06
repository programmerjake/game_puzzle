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
#include "util/lock.h"
#include <iostream>

namespace programmerjake
{
namespace game_puzzle
{
void lock_hierarchy::handleError(const char *what, std::size_t lock_level, const void *theLock)
{
    variables_t &variables = get_variables();
    std::cerr << "locking hierarchy error: " << what << " lock_level=" << lock_level
              << " theLock=" << theLock << std::endl;
    std::cerr << "locked levels:\n";
    std::size_t lastV = 0, count = 0;
    const std::size_t skipAtCount = 3;
    for(std::size_t v : variables.lock_levels)
    {
        if(count == 0 || lastV != v)
        {
            if(count > skipAtCount)
            {
                std::cerr << "(" << count - skipAtCount << " not shown)\n";
            }
            lastV = v;
            count = 1;
        }
        else
        {
            count++;
        }
        if(count <= skipAtCount)
            std::cerr << v << "\n";
    }
    if(count > skipAtCount)
    {
        std::cerr << "(" << count - skipAtCount << " not shown)\n";
    }
    std::cerr << std::endl;
    std::terminate();
}
}
}
