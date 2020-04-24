#include "chip8.h"
#include "opcodes.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define OPCODE_LEN 16

void chip8_init(struct chip8 *chip8)
{
    chip8->cpu.opcode = 0;
    memset(chip8->cpu.V, 0, sizeof(chip8->cpu.V));
    chip8->cpu.I = 0;
    chip8->cpu.pc = 0x200;
    memset(chip8->cpu.stack, 0, sizeof(chip8->cpu.stack));
    chip8->cpu.sp = 0;

    memset(chip8->memory, 0, sizeof(chip8->memory));
    memset(chip8->graphics, 0, sizeof(chip8->graphics));
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    memset(chip8->keypad, 0, sizeof(chip8->keypad));

    chip8->draw = 0;

    // load font set into memory
    uint8_t font_set[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };

    for (int i = 0; i < 80; i++) {
        chip8->memory[i] = font_set[i];
    }
}

void chip8_load(struct chip8 *chip8, uint8_t *program, size_t size)
{
    memcpy(chip8->memory + 512, program, size);
}

void chip8_emulate_cycle(struct chip8 *chip8)
{
    void (*opcodes[])(struct chip8 * chip8) = {
        &op_clear_or_return,
        &op_jump,
        &op_call,
        &op_skip_equal,
        &op_skip_not_equal,
        &op_skip_registers_equal,
        &op_load,
        &op_add,
        &op_arithmetic,
        &op_skip_registers_not_equal,
        &op_load_i,
        &op_jump_offset,
        &op_random,
        &op_draw,
        &op_skip_if_key,
        &op_misc
    };

    // fetch (merge two bytes together)
    chip8->cpu.opcode = chip8->memory[chip8->cpu.pc] << 8 | chip8->memory[chip8->cpu.pc + 1];

    // decode (only need the first 4 bits)
    uint8_t index = (chip8->cpu.opcode & 0xf000) >> 12;

    // execute
    if (index < OPCODE_LEN) {
        opcodes[index](chip8);
    } else {
        printf("Unknown opcode: 0x%X\n", chip8->cpu.opcode);
    }

    // update timers
    if (chip8->delay_timer > 0) {
        chip8->delay_timer -= 1;
    }
    if (chip8->sound_timer > 0) {
        chip8->sound_timer -= 1;
        if (chip8->sound_timer == 0) {
            printf("\a");
        }
    }
}
