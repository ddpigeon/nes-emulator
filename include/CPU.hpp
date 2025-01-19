#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>


class CPU;
class Bus;
struct instruction
{
    std::string name;
    uint8_t cycles;
    void (CPU::*operation)(void);
    void (CPU::*addrmode)(void);
};

class CPU
{
public:
    CPU();
    ~CPU();

    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);

    void clock();

        void ConnectBus(Bus *b) {
            bus = b;
        }

private:
        // Communication bus
        Bus *bus = nullptr;
    uint16_t pc = 0x0000;
    uint8_t sp = 0x00;
    uint8_t a = 0x00;
    uint8_t x = 0x00;
    uint8_t y = 0x00;
    uint8_t fl = 0x00;

    // These do not exist, stores value the instruction operates on and the address of value.
    uint8_t operand;
    uint8_t operand_addr;

    enum flags
    {
        C = (1 << 0), // carry
        Z = (1 << 1), // zero
        I = (1 << 2), // disable interrupts
        D = (1 << 3), // decimal mode
        B = (1 << 4), // break
        U = (1 << 5), // unused
        V = (1 << 6), // overflow
        N = (1 << 7)  // negative
    };

    std::array<instruction, 255> instruction_table;

public:
    // addressing modes:
    uint8_t ACC();
    uint8_t IMM();

    uint8_t ZEP();
    uint8_t ZPX();
    uint8_t ZPY();

    uint8_t IND();
    uint8_t INX();
    uint8_t INY();

    uint8_t ABS();
    uint8_t ABX();
    uint8_t ABY();

public:
    // flag stuff
    void setFlag(flags f, bool v);
    bool getFlag(flags f);

public:
    // put instruction functions here
};

#endif
