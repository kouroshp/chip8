#include "chip8.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

void update_key_state(struct chip8 *chip8, SDL_Keycode key, bool pressed);
void render_frame(struct chip8 *chip8, SDL_Renderer *renderer);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        puts("Usage: chip8 [file]");
        return 0;
    }

    // read file into memory
    FILE *file = fopen(argv[1], "rb");

    if (file == NULL) {
        printf("Could not open file: %s\n", argv[1]);
        return -1;
    }

    uint8_t program[MAX_PROGRAM_SIZE];
    size_t size = fread(program, 1, MAX_PROGRAM_SIZE, file);

    if (ferror(file)) {
        printf("There was a problem reading file: %s\n", argv[1]);
        return -1;
    }

    // create chip8 emulator
    struct chip8 chip8;
    chip8_init(&chip8);
    chip8_load(&chip8, program, size);

    // set up video
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("CHIP8",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        320,
        0);

    if (window == NULL) {
        printf("%s", SDL_GetError());
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("%s", SDL_GetError());
    }

    // clear screen
    render_frame(&chip8, renderer);

    // start emulating
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                update_key_state(&chip8, e.key.keysym.sym, true);
            } else if (e.type == SDL_KEYUP) {
                update_key_state(&chip8, e.key.keysym.sym, false);
            }
        }

        chip8_emulate_cycle(&chip8);

        if (chip8.draw == true) {
            render_frame(&chip8, renderer);
            chip8.draw = false;
        }

        SDL_Delay(16);
    }

    // free SDL memory
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}

void update_key_state(struct chip8 *chip8, SDL_Keycode key, bool pressed)
{
    switch (key) {
    case SDLK_1:
        chip8->keypad[0x1] = pressed;
        break;
    case SDLK_2:
        chip8->keypad[0x2] = pressed;
        break;
    case SDLK_3:
        chip8->keypad[0x3] = pressed;
        break;
    case SDLK_4:
        chip8->keypad[0xc] = pressed;
        break;
    case SDLK_q:
        chip8->keypad[0x4] = pressed;
        break;
    case SDLK_w:
        chip8->keypad[0x5] = pressed;
        break;
    case SDLK_e:
        chip8->keypad[0x6] = pressed;
        break;
    case SDLK_r:
        chip8->keypad[0xd] = pressed;
        break;
    case SDLK_a:
        chip8->keypad[0x7] = pressed;
        break;
    case SDLK_s:
        chip8->keypad[0x8] = pressed;
        break;
    case SDLK_d:
        chip8->keypad[0x9] = pressed;
        break;
    case SDLK_f:
        chip8->keypad[0xe] = pressed;
        break;
    case SDLK_z:
        chip8->keypad[0xa] = pressed;
        break;
    case SDLK_x:
        chip8->keypad[0x0] = pressed;
        break;
    case SDLK_c:
        chip8->keypad[0xb] = pressed;
        break;
    case SDLK_v:
        chip8->keypad[0xf] = pressed;
        break;
    }
}

void render_frame(struct chip8 *chip8, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (chip8->graphics[(y * 64) + x] > 0) {
                SDL_Rect rect = {
                    .x = x * 10,
                    .y = y * 10,
                    .w = 10,
                    .h = 10
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}
