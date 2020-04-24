#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_PROGRAM_SIZE 4096 - 512

struct cpu {
    uint16_t opcode; // Current opcode
    uint8_t V[16]; // Registers V0-VE
    uint16_t I; // Index register
    uint16_t pc; // Program counter
    uint16_t stack[16]; // Stack
    uint16_t sp; // Stack pointer
};

struct chip8 {
    struct cpu cpu;
    uint8_t memory[4096];
    uint8_t graphics[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];
    bool draw;
};

void chip8_init(struct chip8 *chip8);
void chip8_emulate_cycle(struct chip8 *chip8);
void chip8_load(struct chip8 *chip8, uint8_t *program, size_t size);

#endif
