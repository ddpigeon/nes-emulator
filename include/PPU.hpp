#pragma once
#include <cstdint>
#include <memory>

#include "PixelGameEngine.hpp"

#include "Cartridge.hpp"

class PPU {
public:
    PPU();
    ~PPU();

    uint8_t cpuRead(uint16_t addr, bool rdonly = false);
    void cpuWrite(uint16_t addr, uint8_t data);

    uint8_t ppuRead(uint16_t addr, bool rdonly = false);
    void ppuWrite(uint16_t addr, uint8_t data);

    void clock();
    void reset();

    void ConnectCartridge(std::shared_ptr<Cartridge> c);

    Pixel& getColorFromPalette(uint8_t pallet,  uint8_t pixel);

    bool frame_complete = false;
    bool nmi = false;
private:
    uint8_t tblName[2][1024];
    uint8_t tblPattern[2][4096];
    uint8_t tblPalette[32];

    Pixel palScreen[8];

    Sprite* sprScreen;
    Sprite* sprNameTable[2];
    Sprite* sprPatternTable[2];

    union {
        struct {
            uint8_t unused: 5;
            uint8_t sprite_overflow: 1;
            uint8_t sprite0_hit: 1;
            uint8_t vblank: 1;
        };

        uint8_t reg;
    } status;

    union {
        struct {
            uint8_t grayscale : 1;
            uint8_t render_background_left : 1;
            uint8_t render_sprites_left : 1;
            uint8_t render_background : 1;
            uint8_t render_sprites : 1;
            uint8_t enhance_red : 1;
            uint8_t enhance_green : 1;
            uint8_t enhance_blue : 1;
        };

        uint8_t reg;
    } mask;

    union {
        struct {
            uint8_t nametable_x: 1;
            uint8_t nametable_y: 1;
            uint8_t increment_mode: 1;
            uint8_t sprite_pattern_addr: 1;
            uint8_t bg_pattern_addr: 1;
            uint8_t sprite_size: 1;
            uint8_t slave_mode: 1;
            uint8_t enable_nmi: 1;
        };

        uint8_t reg;
    } control;

    union loopy_register {
        struct {
            uint16_t coarse_x: 5;
            uint16_t coarse_y: 5;
            uint16_t nametable_x: 1;
            uint16_t nametable_y: 1;
            uint16_t fine_y: 3;
            uint16_t unused: 1;
        };

        uint16_t reg = 0x0000;
    };

    loopy_register vram_addr;
    loopy_register tram_addr;

    uint8_t fine_x = 0;

    uint8_t loopy_latch = 0;
    uint8_t buffer = 0;

    int16_t scanline = 0;
    int16_t cycle = 0;

    uint8_t bg_next_tile_id = 0;
    uint8_t bg_next_tile_attrib = 0;
    uint8_t bg_next_tile_lsb = 0;
    uint8_t bg_next_tile_msb = 0;
    uint16_t bg_shifter_pattern_lo = 0;
    uint16_t bg_shifter_pattern_hi = 0;
    uint16_t bg_shifter_attrib_lo = 0;
    uint16_t bg_shifter_attrib_hi = 0;
    std::shared_ptr<Cartridge> cart;
};