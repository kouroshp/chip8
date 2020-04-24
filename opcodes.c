#include "opcodes.h"
#include "chip8.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARITHMETIC_OPCODE_LEN 15

void op_arithmetic(struct chip8 *chip8)
{
    void (*opcodes[])(struct chip8 * chip8) = {
        &op_load_from_register,
        &op_or,
        &op_and,
        &op_xor,
        &op_add_registers,
        &op_subtract_x_y,
        &op_shift_right,
        &op_subtract_y_x,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &op_shift_left
    };

    uint8_t index = chip8->cpu.opcode & 0x000f;

    if (index < ARITHMETIC_OPCODE_LEN) {
        void (*op)(struct chip8 * chip8) = opcodes[index];
        if (op != NULL) {
            op(chip8);
            return;
        }
    }

    printf("Unknown opcode: 0x%X\n", chip8->cpu.opcode);
    chip8->cpu.pc += 2;
}

void op_clear_or_return(struct chip8 *chip8)
{
    switch (chip8->cpu.opcode & 0x00ff) {
    case 0xe0:
        op_clear_screen(chip8);
        break;
    case 0xee:
        op_return(chip8);
        break;
    default:
        printf("Unknown opcode: 0x%X\n", chip8->cpu.opcode);
        chip8->cpu.pc += 2;
    }
}

void op_clear_screen(struct chip8 *chip8)
{
    memset(chip8->graphics, 0x0, sizeof(chip8->graphics));
    chip8->draw = true;
    chip8->cpu.pc += 2;
}

void op_return(struct chip8 *chip8)
{
    // remove from stack
    chip8->cpu.sp -= 1;

    // set pc to address on stack
    chip8->cpu.pc = chip8->cpu.stack[chip8->cpu.sp];

    // move along
    chip8->cpu.pc += 2;
}

void op_jump(struct chip8 *chip8)
{
    uint16_t address = chip8->cpu.opcode & 0x0fff;
    chip8->cpu.pc = address;
}

void op_call(struct chip8 *chip8)
{
    // decode address
    uint16_t address = chip8->cpu.opcode & 0x0fff;

    // store current address on stack
    chip8->cpu.stack[chip8->cpu.sp] = chip8->cpu.pc;
    chip8->cpu.sp += 1;

    // set current address to new address
    chip8->cpu.pc = address;
}

// skip equal
void op_skip_equal(struct chip8 *chip8)
{
    uint16_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t value = chip8->cpu.opcode & 0x00ff;

    // skip next instruction if equal
    if (chip8->cpu.V[index] == value) {
        chip8->cpu.pc += 4;
    } else {
        chip8->cpu.pc += 2;
    }
}

// skip not equal
void op_skip_not_equal(struct chip8 *chip8)
{
    uint16_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t value = chip8->cpu.opcode & 0x00ff;

    // skip next instruction if not equal
    if (chip8->cpu.V[index] != value) {
        chip8->cpu.pc += 4;
    } else {
        chip8->cpu.pc += 2;
    }
}

// skip registers equal
void op_skip_registers_equal(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    if (chip8->cpu.V[x] == chip8->cpu.V[y]) {
        chip8->cpu.pc += 4;
    } else {
        chip8->cpu.pc += 2;
    }
}

// load
void op_load(struct chip8 *chip8)
{
    uint16_t dest = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t value = chip8->cpu.opcode & 0x00ff;

    chip8->cpu.V[dest] = value;
    chip8->cpu.pc += 2;
}

// add
void op_add(struct chip8 *chip8)
{
    uint16_t dest = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t value = chip8->cpu.opcode & 0x00ff;

    chip8->cpu.V[dest] += value;
    chip8->cpu.pc += 2;
}

// load from register
void op_load_from_register(struct chip8 *chip8)
{
    uint16_t dest = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t source = (chip8->cpu.opcode & 0x00f0) >> 4;

    chip8->cpu.V[dest] = chip8->cpu.V[source];
    chip8->cpu.pc += 2;
}

// bitwise or between registers
void op_or(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    chip8->cpu.V[x] = chip8->cpu.V[x] | chip8->cpu.V[y];
    chip8->cpu.pc += 2;
}

// bitwise and between registers
void op_and(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    chip8->cpu.V[x] = chip8->cpu.V[x] & chip8->cpu.V[y];
    chip8->cpu.pc += 2;
}

// bitwise xor between registers
void op_xor(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    chip8->cpu.V[x] = chip8->cpu.V[x] ^ chip8->cpu.V[y];
    chip8->cpu.pc += 2;
}

// add register values
void op_add_registers(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    uint16_t result = chip8->cpu.V[x] + chip8->cpu.V[y];

    // set VF if result is > 8 bits and only keep 8 bits
    chip8->cpu.V[0xf] = result > 255;
    chip8->cpu.V[x] = result & 0xff;
    chip8->cpu.pc += 2;
}

// subtract y from x
void op_subtract_x_y(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    // set VF to 1 if result is not negative
    chip8->cpu.V[0xf] = chip8->cpu.V[x] > chip8->cpu.V[y];
    chip8->cpu.V[x] = chip8->cpu.V[x] - chip8->cpu.V[y];
    chip8->cpu.pc += 2;
}

// shift right
void op_shift_right(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;

    // set VF to least significant bit of Vx
    chip8->cpu.V[0xf] = chip8->cpu.V[x] & 0x1;
    chip8->cpu.V[x] = chip8->cpu.V[x] >> 1; // shift right by 1
    chip8->cpu.pc += 2;
}

// subtract x from y
void op_subtract_y_x(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    // set VF to 1 if result is not negative
    chip8->cpu.V[0xf] = chip8->cpu.V[y] > chip8->cpu.V[x];
    chip8->cpu.V[x] = chip8->cpu.V[y] - chip8->cpu.V[x];
    chip8->cpu.pc += 2;
}

// shift left
void op_shift_left(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;

    // set VF to most significant bit of Vx
    chip8->cpu.V[0xf] = (chip8->cpu.V[x] & 0x80) >> 7;
    chip8->cpu.V[x] = chip8->cpu.V[x] << 1; // shift left by 1
    chip8->cpu.pc += 2;
}

// skip registers not equal
void op_skip_registers_not_equal(struct chip8 *chip8)
{
    uint16_t x = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint16_t y = (chip8->cpu.opcode & 0x00f0) >> 4;

    if (chip8->cpu.V[x] != chip8->cpu.V[y]) {
        chip8->cpu.pc += 4;
    } else {
        chip8->cpu.pc += 2;
    }
}

// load address into I
void op_load_i(struct chip8 *chip8)
{
    uint16_t address = chip8->cpu.opcode & 0x0fff;
    chip8->cpu.I = address;
    chip8->cpu.pc += 2;
}

// jump to address + offset
void op_jump_offset(struct chip8 *chip8)
{
    uint8_t offset = chip8->cpu.V[0];
    uint16_t address = (chip8->cpu.opcode & 0x0fff) + offset;

    chip8->cpu.pc = address;
}

// generate random number and AND with value
void op_random(struct chip8 *chip8)
{
    uint16_t dest = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint8_t value = chip8->cpu.opcode & 0x00ff;
    uint8_t number = rand() % 256;

    chip8->cpu.V[dest] = number & value;
    chip8->cpu.pc += 2;
}

// draw to screen
void op_draw(struct chip8 *chip8)
{
    uint8_t x = chip8->cpu.V[(chip8->cpu.opcode & 0x0f00) >> 8];
    uint8_t y = chip8->cpu.V[(chip8->cpu.opcode & 0x00f0) >> 4];
    uint8_t height = chip8->cpu.opcode & 0x000f;
    chip8->cpu.V[0xF] = 0; // set collision to false

    for (uint8_t yIndex = 0; yIndex < height; yIndex++) {
        // get line
        uint8_t line = chip8->memory[chip8->cpu.I + yIndex];

        // loop through each bit
        for (uint8_t xIndex = 0; xIndex < 8; xIndex++) {
            // get new pixel
            uint8_t new = (line & (0x80 >> xIndex)) >> (7 - xIndex);

            // get current pixel (stored as one array of 64 bit rows)
            uint8_t current = chip8->graphics[((y + yIndex) * 64) + x + xIndex];

            // xor new pixel
            uint8_t result = current ^ new;

            // store result
            chip8->graphics[((y + yIndex) * 64) + x + xIndex] = result;

            // set collision to true if something changed
            if (result != current && result == 0) {
                chip8->cpu.V[0xF] = 1;
            }
        }
    }

    chip8->draw = true;
    chip8->cpu.pc += 2;
}

void op_skip_if_key(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint8_t key = chip8->cpu.V[index];

    if ((chip8->cpu.opcode & 0x00ff) == 0x9e) {
        // if key is pressed skip
        if (chip8->keypad[key] == true) {
            chip8->cpu.pc += 4;
        } else {
            chip8->cpu.pc += 2;
        }
    } else if ((chip8->cpu.opcode & 0x00ff) == 0xa1) {
        // if key is not pressed, skip
        if (chip8->keypad[key] == false) {
            chip8->cpu.pc += 4;
        } else {
            chip8->cpu.pc += 2;
        }
    } else {
        printf("Unknown opcode: 0x%X\n", chip8->cpu.opcode);
        chip8->cpu.pc += 2;
    }
}

void op_misc(struct chip8 *chip8)
{
    switch (chip8->cpu.opcode & 0x00ff) {
    case 0x07:
        op_load_delay_timer(chip8);
        break;
    case 0x0a:
        op_wait_for_key(chip8);
        break;
    case 0x15:
        op_set_delay_timer(chip8);
        break;
    case 0x18:
        op_set_sound_timer(chip8);
        break;
    case 0x1e:
        op_add_i(chip8);
        break;
    case 0x29:
        op_load_sprite(chip8);
        break;
    case 0x33:
        op_bcd(chip8);
        break;
    case 0x55:
        op_register_dump(chip8);
        break;
    case 0x65:
        op_register_load(chip8);
        break;
    default:
       printf("Unknown opcode: 0x%X\n", chip8->cpu.opcode);
       chip8->cpu.pc += 2;
    }
}

void op_load_delay_timer(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    chip8->cpu.V[index] = chip8->delay_timer;
    chip8->cpu.pc += 2;
}

void op_wait_for_key(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;

    bool pressed = false;

    for (uint8_t i = 0; i < 16; i++) {
        if (chip8->keypad[i] != 0) {
            chip8->cpu.V[index] = i;
            pressed = true;
        }
    }

    // don't move on in the program until we got something
    if (pressed) {
        chip8->cpu.pc += 2;
    }
}

// load delay timer
void op_set_delay_timer(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    chip8->delay_timer = chip8->cpu.V[index];
    chip8->cpu.pc += 2;
}

// load sound timer
void op_set_sound_timer(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    chip8->sound_timer = chip8->cpu.V[index];
    chip8->cpu.pc += 2;
}

// add Vx to I
void op_add_i(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    chip8->cpu.I += chip8->cpu.V[index];

    if (chip8->cpu.I > 0xfff) {
        chip8->cpu.V[0xf] = 1;
    } else {
       chip8->cpu.V[0xf] = 0;
    }

    chip8->cpu.pc += 2;
}

// set I to location of sprite in Vx
void op_load_sprite(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    chip8->cpu.I = chip8->cpu.V[index] * 0x5;
    chip8->cpu.pc += 2;
}

// bcd craziness
void op_bcd(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;
    uint8_t value = chip8->cpu.V[index];

    uint8_t h = value - (value % 100);
    uint8_t t = (value - h) - ((value - h) % 10);
    uint8_t u = value - h - t;

    chip8->memory[chip8->cpu.I] = h / 100;
    chip8->memory[chip8->cpu.I + 1] = t / 10;
    chip8->memory[chip8->cpu.I + 2] = u;

    chip8->cpu.pc += 2;
}

// dump V0-Vx in memory starting from I
void op_register_dump(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;

    for (uint8_t i = 0; i <= index; i++) {
        chip8->memory[chip8->cpu.I] = chip8->cpu.V[i];
        chip8->cpu.I += 1;
    }

    chip8->cpu.pc += 2;
}

// load registers from memory
void op_register_load(struct chip8 *chip8)
{
    uint8_t index = (chip8->cpu.opcode & 0x0f00) >> 8;

    for (uint8_t i = 0; i <= index; i++) {
        chip8->cpu.V[i] = chip8->memory[chip8->cpu.I];
        chip8->cpu.I += 1;
    }

    chip8->cpu.pc += 2;
}
