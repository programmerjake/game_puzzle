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

#ifndef SUBGAME_MAIN_MACHINE_H_
#define SUBGAME_MAIN_MACHINE_H_

#include "ui/element.h"
#include "subgame/main/connection.h"

namespace programmerjake
{
namespace game_puzzle
{
namespace subgames
{
namespace main
{
class Machine : public ui::Element
{
public:
    std::vector<Connection::MachineInput> inputs;
    const std::shared_ptr<Connection> output;
    Machine(float minX, float maxX, float minY, float maxY, VectorF source, bool powered = false)
        : Element(minX, maxX, minY, maxY),
          inputs(),
          output(std::make_shared<Connection>(source, powered))
    {
    }
    virtual void move(double deltaTime) override
    {
        bool powered = transferFunction();
        if(powered != output->powered->get())
            output->powered->set(powered);
    }
    void addInput(const std::shared_ptr<Connection> &input)
    {
        inputs.push_back(input->addInput(getInputPosition(inputs.size())));
    }

protected:
    virtual bool transferFunction() = 0;
    virtual VectorF getInputPosition(std::size_t inputIndex)
    {
        return VectorF((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, 0);
    }
};
}
}
}
}

#endif /* SUBGAME_MAIN_MACHINE_H_ */
