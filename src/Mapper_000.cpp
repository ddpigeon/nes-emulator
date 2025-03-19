#include "../include/Mapper_000.hpp"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
}

Mapper_000::~Mapper_000()
{
}

void Mapper_000::reset()
{
    // Nothing to do for Mapper 000
}

// Handle CPU addressing
bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
    // NROM has two modes: 16K and 32K
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        // If 16K PRG ROM, mirror appropriately
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }

    return false;
}

bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        // For Mapper 0, no write operations are supported for CPU addressing
        // But we return true so the bus knows the address was in range
        return true;
    }

    return false;
}

// Handle PPU addressing
bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
    // CHR ROM/RAM from 0x0000 to 0x1FFF
    if (addr <= 0x1FFF)
    {
        // No mapping required for Mapper 0
        mapped_addr = addr;
        return true;
    }

    return false;
}

bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
    // CHR ROM/RAM from 0x0000 to 0x1FFF
    if (addr <= 0x1FFF)
    {
        // For Mapper 0, writes to CHR are only allowed if CHR RAM is present (no CHR ROM banks)
        if (nCHRBanks == 0)
        {
            mapped_addr = addr;
            return true;
        }
    }

    return false;
}