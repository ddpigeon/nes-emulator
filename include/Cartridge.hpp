#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "Mapper.hpp"
#include "Mapper_000.hpp"

class Cartridge
{
public:
    Cartridge(const std::string& sFileName);
    ~Cartridge();

public:
    enum MIRROR
    {
        HARDWARE,
        HORIZONTAL,
        VERTICAL,
        ONESCREEN_LO,
        ONESCREEN_HI,
    } mirror = HORIZONTAL;

    // Communication with Main Bus
    bool cpuRead(uint16_t addr, uint8_t &data);
    bool cpuWrite(uint16_t addr, uint8_t data);

    // Communication with PPU Bus
    bool ppuRead(uint16_t addr, uint8_t &data);
    bool ppuWrite(uint16_t addr, uint8_t data);

    // Reset cartridge to known state
    void reset();

    // Indicates if cartridge has successfully loaded
    bool ImageValid();
    
    // Get mirror mode
    MIRROR GetMirrorMode();
    
private:
    // iNES Format Header
    struct sHeader
    {
        char name[4];
        uint8_t prg_rom_chunks;
        uint8_t chr_rom_chunks;
        uint8_t mapper1;
        uint8_t mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1;
        uint8_t tv_system2;
        char unused[5];
    } header;

    // Cartridge data storage
    std::vector<uint8_t> vPRGMemory;
    std::vector<uint8_t> vCHRMemory;

    // Mapper related variables
    uint8_t nMapperID = 0;
    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;

    // The mapper
    std::shared_ptr<Mapper> pMapper;

    // Whether the cartridge contains valid data
    bool bImageValid = false;
};

#endif