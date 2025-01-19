#include <iostream>
#include "../include/CPU.hpp"

CPU::CPU() {
    // Initialize instruction table here
    // instruction_table i = {"name", cycles, &CPU::instruction func, &CPU::addr mode
}

CPU::~CPU(){
}


void CPU::setFlag(flags f, bool v) {
    if (v) {
        fl |= f;
    }
    else {
        fl &= ~f;
    }
}

bool CPU::getFlag(flags f) {
    return fl & f;
}



// Addressing modes
void CPU::ACC() {
    operand = a;
}

void CPU::IMM() {
    operand_addr = pc++;
}

void CPU::ZEP() {
    operand_addr = read(pc++);
    operand_addr &= 0x00FF; //set address to zero page
}

void CPU::ABS() {
    uint16_t lo = read(pc++);
    uint16_t hi = read(pc++);
    operand_addr = (hi << 8) | lo;
}






// Instructions

