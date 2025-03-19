#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <array>
#include <memory>

#include "CPU.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"

class Bus
{
public:
    Bus();
    ~Bus();

public: // Devices on bus
    CPU cpu;
    PPU ppu;
    std::array<uint8_t, 2048> ram;
    std::shared_ptr<Cartridge> cart;

public: // Bus read and write
    void cpuWrite(uint16_t addr, uint8_t data);
    uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

public: // System Interface
    void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
    void reset();
    void clock();

private:
    // System clock count
    uint32_t nSystemClockCounter = 0;

    // Direct Memory Access (DMA) variables
    uint8_t dma_page = 0x00;
    uint8_t dma_addr = 0x00;
    uint8_t dma_data = 0x00;
    bool dma_transfer = false;
    bool dma_dummy = true;
};

#endif