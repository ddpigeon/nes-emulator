#include <iostream>
#include "../include/Bus.hpp"

Bus::Bus()
{
    // Clear RAM contents
    for (auto &i : ram) i = 0x00;
    
    // Connect CPU to communication bus
    cpu.ConnectBus(this);

    // Connect PPU to communication bus
    ppu.ConnectBus(this);
}

Bus::~Bus()
{
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
    if (cart->cpuWrite(addr, data))
    {
        // Cartridge address range
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        // System RAM Address Range, mirrored every 0x800 bytes
        ram[addr & 0x07FF] = data;
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // PPU registers, mirrored every 8 bytes
        ppu.cpuWrite(addr & 0x0007, data);
    }
    else if (addr == 0x4014)
    {
        // DMA transfer to OAM
        dma_page = data;
        dma_addr = 0x00;
        dma_transfer = true;
    }
    else if (addr >= 0x4000 && addr <= 0x4017)
    {
        // NES APU and I/O registers
    }
    else if (addr >= 0x4020 && addr <= 0xFFFF)
    {
        // Cartridge space: ROM, RAM, and mapper registers
    }
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
    uint8_t data = 0x00;
    
    if (cart->cpuRead(addr, data))
    {
        // Cartridge address range
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        // System RAM, mirrored every 0x800 bytes
        data = ram[addr & 0x07FF];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // PPU registers, mirrored every 8 bytes
        data = ppu.cpuRead(addr & 0x0007, bReadOnly);
    }
    else if (addr >= 0x4000 && addr <= 0x4017)
    {
        // NES APU and I/O registers
    }
    else if (addr >= 0x4020 && addr <= 0xFFFF)
    {
        // Cartridge space: ROM, RAM, and mapper registers
    }
    
    return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
    ppu.ConnectCartridge(cartridge);
}

void Bus::reset()
{
    cpu.reset();
    ppu.reset();
    nSystemClockCounter = 0;
    dma_page = 0x00;
    dma_addr = 0x00;
    dma_data = 0x00;
    dma_dummy = true;
    dma_transfer = false;
}

void Bus::clock()
{
    // PPU runs 3 times faster than CPU
    ppu.clock();
    
    // CPU and PPU are synchronized, but the CPU is slower, so we only clock
    // the CPU every 3 PPU cycles
    if (nSystemClockCounter % 3 == 0)
    {
        // Check if DMA transfer is requested
        if (dma_transfer)
        {
            if (dma_dummy)
            {
                // Synchronize with CPU clock - this dummy read is CPU cycle aligned
                if (nSystemClockCounter % 2 == 1)
                {
                    dma_dummy = false;
                }
            }
            else
            {
                // Perform DMA read
                if (nSystemClockCounter % 2 == 0)
                {
                    dma_data = cpuRead((dma_page << 8) | dma_addr);
                }
                else
                {
                    // Perform DMA write - sprite memory is a separate address space in PPU
                    ppu.pOAM[dma_addr] = dma_data;
                    
                    // Increment address
                    dma_addr++;
                    
                    // If we have written 256 bytes, the DMA transfer is complete
                    if (dma_addr == 0x00)
                    {
                        dma_transfer = false;
                        dma_dummy = true;
                    }
                }
            }
        }
        else
        {
            // No DMA happening, the CPU is free to do its thing
            cpu.clock();
        }
    }
    
    // The PPU provides a synchronization signal
    if (ppu.nmi)
    {
        ppu.nmi = false;
        cpu.nmi();
    }
    
    nSystemClockCounter++;
}



