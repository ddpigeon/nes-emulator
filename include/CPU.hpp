#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>

class CPU;

typedef struct instruction {
    std::string name;
    uint8_t cycles;
    void (CPU::*operation)(void);
    void (CPU::*addrmode)(void);
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
        uint8_t fl = 0x00;

        // These do not exist, stores value the instruction operates on and the address of value.
        uint8_t operand;
        uint8_t operand_addr;

        enum flags {
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
        void ACC();
        void IMM();

        void ZEP();
        void ZPX();
        void ZPY();

        void IND();
        void INX();
        void INY();

        void ABS();
        void ABX();
        void ABY();

    public:
        //flag stuff
        void setFlag(flags f, bool v);
        bool getFlag(flags f);


    public:
        // put instruction functions here

};

#endif
