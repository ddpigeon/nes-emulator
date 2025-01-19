#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>

class CPU;

typedef struct instruction {
    std::string name;
    uint8_t cycles;
    uint8_t (CPU::*addrmode)(void) = nullptr;
    uint8_t (CPU::*operation)(void) = nullptr;
} instruction;

class CPU {
    public:
        CPU();
        ~CPU();

        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr);

        void clock();

    private:
        uint16_t pc = 0x0000;
        uint8_t sp = 0x00;
        uint8_t a = 0x00;
        uint8_t x = 0x00;
        uint8_t y = 0x00;

        uint8_t fetched = 0x00;

        enum FLAGS {
            C = (1 << 0), //carry
            Z = (1 << 1), //zero
            I = (1 << 2), //disable interrupts
            D = (1 << 3), //decimal mode
            B = (1 << 4), //break
            U = (1 << 5), //unused
            V = (1 << 6), //overflow
            N = (1 << 7)  //negative
        };

        std::array<instruction, 255> instruction_table;

    public:
        // addressing modes:
        uint16_t ACC();
        uint16_t IMM();

        uint16_t ZEP();
        uint16_t ZPX();
        uint16_t ZPY();

        uint16_t IND();
        uint16_t INX();
        uint16_t INY();

        uint16_t ABS();
        uint16_t ABX();
        uint16_t ABY();

    public:
        // put instruction functions here

};

#endif
