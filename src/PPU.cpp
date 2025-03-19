#include "PPU.hpp"

PPU::PPU() {

    for (int i = 0; i < 8; i++) {
        palScreen[i].r = (i & 1) ? 255 : 0;
        palScreen[i].g = (i & 2) ? 255 : 0;
        palScreen[i].b = (i & 4) ? 255 : 0;
        palScreen[i].a = 255;
    }

    sprScreen = new Sprite(256, 240);
    sprNameTable[0] = new Sprite(256, 240);
    sprNameTable[1] = new Sprite(256, 240);
    sprPatternTable[0] = new Sprite(128, 128);
    sprPatternTable[1] = new Sprite(128, 128);
}

PPU::~PPU() {
    delete sprScreen;
    delete sprNameTable[0];
    delete sprNameTable[1];
    delete sprPatternTable[0];
    delete sprPatternTable[1];
}

Pixel& PPU::getColorFromPalette(uint8_t pallet, uint8_t pixel) {
    return palScreen[ppuRead(0x3F00 + (pallet << 2) + pixel) & 0x3F];
}

uint8_t  PPU::cpuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;

    if (rdonly) {
        switch(addr) {
            case 0x0000: // Control Register
                data = control.reg;
                break;
            case 0x0001: // Mask Register
                data = mask.reg;
                break;
            case 0x0002: // Status Register
                data = status.reg;
                break;
            case 0x0003: // OAM Address
                break;
            case 0x0004: // OAM Data
                break;
            case 0x0005: // Scroll
                break;
            case 0x0006: // PPU Address
                break;
            case 0x0007: // PPU Data
                break;
        }
    } else {
        switch(addr) {
            case 0x0000: // Control Register
                break;
            case 0x0001: // Mask Register
                mask.reg = data;
                break;
            case 0x0002: // Status Register
                data = (status.reg & 0xE0) | (data & 0x1F);
                status.vblank = 0;
                loopy_latch = 0;
                break;
            case 0x0003: // OAM Address
                break;
            case 0x0004: // OAM Data
                break;
            case 0x0005: // Scroll
                break;
            case 0x0006: // PPU Address
                break;
            case 0x0007: // PPU Data
                data = buffer;
                buffer = ppuRead(vram_addr.reg);
                if (vram_addr.reg >= 0x3F00) {
                    data = buffer;
                }
                vram_addr.reg += (control.increment_mode) ? 32 : 1;
                break;
        }
    }

    return data;
}

void PPU::cpuWrite(uint16_t addr, uint8_t data) {
    switch(addr) {
        case 0x0000: // Control Register
            control.reg = data;
            tram_addr.nametable_x = control.nametable_x;
            tram_addr.nametable_y = control.nametable_y;
            break;
        case 0x0001: // Mask Register
            mask.reg = data;
            break;
        case 0x0002: // Status Register
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            if (loopy_latch == 0) {
                fine_x = data & 0x07;
                tram_addr.coarse_x = data >> 3;
                loopy_latch = 1;
            } else {
                tram_addr.fine_y = data & 0x07;
                tram_addr.coarse_y = data >> 3;
                loopy_latch = 0;
            }
            break;
        case 0x0006: // PPU Address
            if (loopy_latch == 0) {
                tram_addr.reg = (uint16_t)((data & 0x3f) << 8) | (tram_addr.reg & 0x00FF);
                loopy_latch = 1;
            } else {
                tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
                vram_addr = tram_addr;
                loopy_latch = 0;
            }
            break;
        case 0x0007: // PPU Data
            ppuWrite(vram_addr.reg, data);
            vram_addr.reg += (control.increment_mode) ? 32 : 1;
            break;
    }
}

uint8_t PPU::ppuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;
    addr &= 0x3FFF;

    if (cart->ppuRead(addr, data)) {
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        addr &= 0x0FFF;
        if (cart->mirror == Cartridge::MIRROR::VERTICAL) {
            if (addr > 0x0000 && addr <= 0x03FF) {
                data = tblPattern[0][addr & 0x03FF];
            } else if (addr > 0x0400 && addr <= 0x07FF) {
                data = tblName[1][addr & 0x03FF];
            } else if (addr > 0x0800 && addr <= 0x0BFF) {
                data = tblName[0][addr & 0x03FF];
            } else if (addr > 0x0C00 && addr <= 0x0FFF) {
                data = tblPattern[1][addr & 0x03FF];
            }
        } else if (cart->mirror == Cartridge::MIRROR::HORIZONTAL) {
            if (addr > 0x0000 && addr <= 0x03FF) {
                data = tblPattern[0][addr & 0x03FF];
            } else if (addr > 0x0400 && addr <= 0x07FF) {
                data = tblName[0][addr & 0x03FF];
            } else if (addr > 0x0800 && addr <= 0x0BFF) {
                data = tblName[1][addr & 0x03FF];
            } else if (addr > 0x0C00 && addr <= 0x0FFF) {
                data = tblPattern[1][addr & 0x03FF];
            }
        }
    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
    }

    return data;
}


void PPU::ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;

    if (cart->ppuWrite(addr, data)){}
    else if (addr >= 0x000 && addr <= 0x1FFF) {
        tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        addr &= 0x0FFF;
        if (cart->mirror == Cartridge::MIRROR::VERTICAL) {
            if (addr > 0x0000 && addr <= 0x03FF) {
                tblName[0][addr & 0x03FF] = data;
            } else if (addr > 0x0400 && addr <= 0x07FF) {
                tblName[1][addr & 0x03FF] = data;
            } else if (addr > 0x0800 && addr <= 0x0BFF) {
                tblName[0][addr & 0x03FF] = data;
            } else if (addr > 0x0C00 && addr <= 0x0FFF) {
                tblName[1][addr & 0x03FF] = data;
            }
        } else if (cart->mirror == Cartridge::MIRROR::HORIZONTAL) {
            if (addr > 0x0000 && addr <= 0x03FF) {
                tblName[0][addr & 0x03FF] = data;
            } else if (addr > 0x0400 && addr <= 0x07FF) {
                tblName[0][addr & 0x03FF] = data;
            } else if (addr > 0x0800 && addr <= 0x0BFF) {
                tblName[1][addr & 0x03FF] = data;
            } else if (addr > 0x0C00 && addr <= 0x0FFF) {
                tblName[1][addr & 0x03FF] = data;
            }
        }
    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        tblPalette[addr] = data;
    }
}


void PPU::ConnectCartridge(std::shared_ptr<Cartridge> c) {
    cart = c;
}

void PPU::reset() {
    fine_x = 0;
    loopy_latch = 0;
    buffer = 0;
    vram_addr.reg = 0;
    tram_addr.reg = 0;
    scanline = 0;
    cycle = 0;
    control.reg = 0;
    mask.reg = 0;
    status.reg = 0;
    mask.reg = 0;
    control.reg = 0;
    vram_addr.reg = 0;
    tram_addr.reg = 0;
    bg_next_tile_id = 0;
    bg_next_tile_attrib = 0;
    bg_next_tile_lsb = 0;
    bg_next_tile_msb = 0;
    bg_shifter_pattern_lo = 0;
    bg_shifter_pattern_hi = 0;
    bg_shifter_attrib_lo = 0;
    bg_shifter_attrib_hi = 0;
}

void PPU::clock() {
    auto IncrementScrollX = [&] () {
        if (mask.render_background || mask.render_sprites) {
            if (vram_addr.coarse_x == 31) {
                vram_addr.coarse_x = 0;
                vram_addr.nametable_x ^= 1;
            } else {
                vram_addr.coarse_x++;
            }
        }
    };

    auto IncrementScrollY = [&] () {
        if (mask.render_background || mask.render_sprites) {
            if (vram_addr.fine_y < 7) {
                vram_addr.fine_y++;
            } else {
                vram_addr.fine_y = 0;
                if (vram_addr.coarse_y == 29) {
                    vram_addr.coarse_y = 0;
                    vram_addr.nametable_y ^= 1;
                } else if (vram_addr.coarse_y == 31) {
                    vram_addr.coarse_y = 0;
                } else {
                    vram_addr.coarse_y++;
                }
            }
        }
    };

    auto TransferAddressX = [&] () {
        if (mask.render_background || mask.render_sprites) {
            vram_addr.nametable_x = tram_addr.nametable_x;
            vram_addr.coarse_x = tram_addr.coarse_x;
        }
    };

    auto TransferAddressY = [&] () {
        if (mask.render_background || mask.render_sprites) {
            vram_addr.nametable_y = tram_addr.nametable_y;
            vram_addr.fine_y = tram_addr.fine_y;
            vram_addr.coarse_y = tram_addr.coarse_y;
        }
    };

    auto LoadBackgroundShifters = [&] () {
        if (mask.render_background) {
            bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | (bg_next_tile_lsb);
            bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | (bg_next_tile_msb);
            bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0x01) ? 0xFF : 0x00);
            bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0x02) ? 0xFF : 0x00);
        }
    };

    auto UpdateShifters = [&] () {
        if (mask.render_background) {
            bg_shifter_pattern_lo <<= 1;
            bg_shifter_pattern_hi <<= 1;
            bg_shifter_attrib_lo <<= 1;
            bg_shifter_attrib_hi <<= 1;
        }
    };

    if (scanline >= -1 && scanline < 240)
	{		
		if (scanline == 0 && cycle == 0)
		{

			cycle = 1;
		}

		if (scanline == -1 && cycle == 1)
		{

			status.vblank = 0;
		}


		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338))
		{
			UpdateShifters();
			switch ((cycle - 1) % 8)
			{
			case 0:
				LoadBackgroundShifters();
				bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
				break;
			case 2:
				bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11) 
					                                 | (vram_addr.nametable_x << 10) 
					                                 | ((vram_addr.coarse_y >> 2) << 3) 
					                                 | (vram_addr.coarse_x >> 2));
				if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
				if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
				bg_next_tile_attrib &= 0x03;
				break;


			case 4: 
				bg_next_tile_lsb = ppuRead((control.bg_pattern_addr << 12) 
					                       + ((uint16_t)bg_next_tile_id << 4) 
					                       + (vram_addr.fine_y) + 0);

				break;
			case 6:


				bg_next_tile_msb = ppuRead((control.bg_pattern_addr << 12)
					                       + ((uint16_t)bg_next_tile_id << 4)
					                       + (vram_addr.fine_y) + 8);
				break;
			case 7:



				IncrementScrollX();
				break;
			}
		}

		if (cycle == 256)
		{
			IncrementScrollY();
		}

		if (cycle == 257)
		{
			LoadBackgroundShifters();
			TransferAddressX();
		}

		if (cycle == 338 || cycle == 340)
		{
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305)
		{

			TransferAddressY();
		}
	}

	if (scanline == 240)
	{

	}

	if (scanline >= 241 && scanline < 261)
	{
		if (scanline == 241 && cycle == 1)
		{
			status.vblank = 1;

			if (control.enable_nmi) 
				nmi = true;
		}
	}


	uint8_t bg_pixel = 0x00;   
	uint8_t bg_palette = 0x00; 

	if (mask.render_background)
	{
		uint16_t bit_mux = 0x8000 >> fine_x;

		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

		bg_pixel = (p1_pixel << 1) | p0_pixel;

		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}

	sprScreen->SetPixel(cycle - 1, scanline, getColorFromPalette(bg_palette, bg_pixel));

	if (cycle >= 341)
	{
		cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
	}

}