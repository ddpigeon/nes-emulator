#include "../include/Cartridge.hpp"

Cartridge::Cartridge(const std::string& sFileName)
{
    bImageValid = false;

    std::ifstream ifs;
    ifs.open(sFileName, std::ifstream::binary);
    if (ifs.is_open())
    {
        // Read file header
        ifs.read((char*)&header, sizeof(sHeader));

        // If first 4 bytes are "NES" followed by MS-DOS EOF
        if (header.name[0] == 'N' && header.name[1] == 'E' && header.name[2] == 'S' && header.name[3] == 0x1A)
        {
            // Extract mapper ID
            nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
            
            // Get mirror mode
            if (header.mapper1 & 0x01)
                mirror = VERTICAL;
            else
                mirror = HORIZONTAL;
                
            // "Discover" file format
            uint8_t nFileType = 1;

            if (nFileType == 0)
            {
                
            }

            if (nFileType == 1)
            {
                nPRGBanks = header.prg_rom_chunks;
                vPRGMemory.resize(nPRGBanks * 16384);
                ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

                nCHRBanks = header.chr_rom_chunks;
                if (nCHRBanks == 0)
                {
                    // Create CHR RAM
                    vCHRMemory.resize(8192);
                }
                else
                {
                    // Allocate for ROM
                    vCHRMemory.resize(nCHRBanks * 8192);
                }
                ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
            }

            if (nFileType == 2)
            {
                
            }

            // Load appropriate mapper based on mapper ID
            switch (nMapperID)
            {
            case 0: 
                pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks);
                bImageValid = true; 
                break;
            }
        }
    }

    ifs.close();
}

Cartridge::~Cartridge()
{
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t &data)
{
    uint32_t mapped_addr = 0;
    
    if (pMapper->cpuMapRead(addr, mapped_addr, data))
    {
        if (mapped_addr == 0xFFFFFFFF)
        {
            // Mapper has handled read directly
            return true;
        }
        else
        {
            // Mapper has mapped the address, we need to read
            data = vPRGMemory[mapped_addr];
            return true;
        }
    }
    
    return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    
    if (pMapper->cpuMapWrite(addr, mapped_addr, data))
    {
        if (mapped_addr == 0xFFFFFFFF)
        {
            // Mapper has handled write directly
            return true;
        }
        else
        {
            // Mapper has mapped a physical address, we need to write
            vPRGMemory[mapped_addr] = data;
            return true;
        }
    }
    
    return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t &data)
{
    uint32_t mapped_addr = 0;
    
    if (pMapper->ppuMapRead(addr, mapped_addr))
    {
        data = vCHRMemory[mapped_addr];
        return true;
    }
    
    return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    
    if (pMapper->ppuMapWrite(addr, mapped_addr))
    {
        vCHRMemory[mapped_addr] = data;
        return true;
    }
    
    return false;
}

void Cartridge::reset()
{
    // Reset mapper
    if (pMapper)
        pMapper->reset();
}

bool Cartridge::ImageValid()
{
    return bImageValid;
}

Cartridge::MIRROR Cartridge::GetMirrorMode()
{
    return mirror;
}