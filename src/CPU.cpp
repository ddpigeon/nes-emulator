#include <iostream>
#include "../include/CPU.hpp"

CPU::CPU()
{
    // Initialize instruction table here
    // instruction_table i = {"name", cycles, &CPU::instruction func, &CPU::addr mode
}

CPU::~CPU()
{
}

void CPU::setFlag(flags f, bool v)
{
    if (v)
    {
        fl |= f;
    }
    else
    {
        fl &= ~f;
    }
}

bool CPU::getFlag(flags f)
{
    return fl & f;
}

// Addressing modes
uint8_t CPU::ACC()
{
    operand = a;
    return 0;
}

uint8_t CPU::IMM()
{
    operand_addr = pc++;
    return 0;
}

uint8_t CPU::ZEP()
{
    operand_addr = read(pc++);
    return 0;
}

uint8_t CPU::ZPX()
{
    operand_addr = (read(pc++) + x) & 0xFF;
    return 0;
}

uint8_t CPU::ZPY()
{
    operand_addr = (read(pc++) + y) & 0xFF;
    return 0;
}

uint8_t CPU::IND()
{
    uint16_t addr = read(pc++);
    addr |= read(pc++) << 8;
    operand_addr = read(addr);
    operand_addr |= read((addr & 0xFF00) | ((addr + 1) & 0xFF)) << 8;
    return 0;
}

uint8_t CPU::INX()
{
    uint16_t addr = (read(pc++) + x) & 0xFF;
    operand_addr = read(addr);
    operand_addr |= read((addr & 0xFF00) | ((addr + 1) & 0xFF)) << 8;
    return 0;
}

uint8_t CPU::INY()
{
    uint16_t addr = read(pc++);
    addr |= read(pc++) << 8;
    addr += y;
    operand_addr = read(addr);
    operand_addr |= read((addr & 0xFF00) | ((addr + 1) & 0xFF)) << 8;
    return 0;
}

uint8_t CPU::ABS()
{
    uint16_t addr = read(pc++);
    addr |= read(pc++) << 8;
    operand_addr = addr;
    return 0;
}

uint8_t CPU::ABX()
{
    uint16_t addr = read(pc++);
    addr |= read(pc++) << 8;
    addr += x;
    operand_addr = addr;
    if ((addr & 0xFF00) != (addr + x) & 0xFF00)
    {
        return 1;
    }
    return 0;
}

uint8_t CPU::ABY()
{
    uint16_t addr = read(pc++);
    addr |= read(pc++) << 8;
    addr += y;
    operand_addr = addr;
    if ((addr & 0xFF00) != (addr + y) & 0xFF00)
    {
        return 1;
    }
    return 0;
}

// Instructions
