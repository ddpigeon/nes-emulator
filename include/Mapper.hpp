#ifndef MAPPER_HPP
#define MAPPER_HPP

#include <cstdint>
#include <vector>

class Mapper
{
public:
    Mapper(uint8_t prgBanks, uint8_t chrBanks);
    virtual ~Mapper();

public:
    // Transform CPU address into PRG ROM offset
    virtual bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) = 0;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) = 0;
    
    // Transform PPU address into CHR ROM offset
    virtual bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) = 0;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) = 0;
    
    virtual void reset() = 0;

protected:
    // Number of 16K PRG banks
    uint8_t nPRGBanks = 0;
    
    // Number of 8K CHR banks
    uint8_t nCHRBanks = 0;
};

#endif