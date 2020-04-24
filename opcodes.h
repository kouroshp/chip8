#ifndef OPCODES_H
#define OPCODES_H

#include "chip8.h"

void op_clear_or_return(struct chip8 *chip8);
void op_clear_screen(struct chip8 *chip8);
void op_return(struct chip8 *chip8);

void op_jump(struct chip8 *chip8);
void op_call(struct chip8 *chip8);
void op_skip_equal(struct chip8 *chip8);
void op_skip_not_equal(struct chip8 *chip8);
void op_skip_registers_equal(struct chip8 *chip8);
void op_load(struct chip8 *chip8);
void op_add(struct chip8 *chip8);

void op_arithmetic(struct chip8 *chip8);
void op_load_from_register(struct chip8 *chip8);
void op_or(struct chip8 *chip8);
void op_and(struct chip8 *chip8);
void op_xor(struct chip8 *chip8);
void op_add_registers(struct chip8 *chip8);
void op_subtract_x_y(struct chip8 *chip8);
void op_shift_right(struct chip8 *chip8);
void op_subtract_y_x(struct chip8 *chip8);
void op_shift_left(struct chip8 *chip8);

void op_skip_registers_not_equal(struct chip8 *chip8);
void op_load_i(struct chip8 *chip8);
void op_jump_offset(struct chip8 *chip8);
void op_random(struct chip8 *chip8);
void op_draw(struct chip8 *chip8);
void op_skip_if_key(struct chip8 *chip8);

void op_misc(struct chip8 *chip8);
void op_load_delay_timer(struct chip8 *chip8);
void op_wait_for_key(struct chip8 *chip8);
void op_set_delay_timer(struct chip8 *chip8);
void op_set_sound_timer(struct chip8 *chip8);
void op_add_i(struct chip8 *chip8);
void op_load_sprite(struct chip8 *chip8);
void op_bcd(struct chip8 *chip8);
void op_register_dump(struct chip8 *chip8);
void op_register_load(struct chip8 *chip8);

#endif
