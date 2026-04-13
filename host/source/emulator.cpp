#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "cpu.h"
#include "doomkeys.h"

// for DOOMGeneric
static unsigned char convertToDoomKey(unsigned int key){
  switch (key)
    {
    case SDLK_RETURN:
      key = KEY_ENTER;
      break;
    case SDLK_ESCAPE:
      key = KEY_ESCAPE;
      break;
    case SDLK_LEFT:
      key = KEY_LEFTARROW;
      break;
    case SDLK_RIGHT:
      key = KEY_RIGHTARROW;
      break;
    case SDLK_UP:
      key = KEY_UPARROW;
      break;
    case SDLK_DOWN:
      key = KEY_DOWNARROW;
      break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
      key = KEY_FIRE;
      break;
    case SDLK_SPACE:
      key = KEY_USE;
      break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
      key = KEY_RSHIFT;
      break;
    case SDLK_LALT:
    case SDLK_RALT:
      key = KEY_LALT;
      break;
    case SDLK_F2:
      key = KEY_F2;
      break;
    case SDLK_F3:
      key = KEY_F3;
      break;
    case SDLK_F4:
      key = KEY_F4;
      break;
    case SDLK_F5:
      key = KEY_F5;
      break;
    case SDLK_F6:
      key = KEY_F6;
      break;
    case SDLK_F7:
      key = KEY_F7;
      break;
    case SDLK_F8:
      key = KEY_F8;
      break;
    case SDLK_F9:
      key = KEY_F9;
      break;
    case SDLK_F10:
      key = KEY_F10;
      break;
    case SDLK_F11:
      key = KEY_F11;
      break;
    case SDLK_EQUALS:
    case SDLK_PLUS:
      key = KEY_EQUALS;
      break;
    case SDLK_MINUS:
      key = KEY_MINUS;
      break;
    default:
      key = tolower(key);
      break;
    }

  return key;
}

static void load_binary(RISCV_CPU& cpu, const std::vector<uint8_t>& bytes) {
    for (size_t offset = 0; offset < bytes.size(); offset += 4) {
        uint32_t word = 0;
        for (size_t i = 0; i < 4 && offset + i < bytes.size(); ++i) {
            word |= static_cast<uint32_t>(bytes[offset + i]) << (8 * i);
        }
        cpu.write_word(static_cast<uint32_t>(offset), word);
    }
}


int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "usage: emulator_test_harness <test.bin> [steps]\n";
        return 2;
    }
    int steps = argc == 3 ? std::atoi(argv[2]) : 2000;

    std::ifstream input(argv[1], std::ios::binary);
    if (!input) {
        std::cerr << "failed to open " << argv[1] << "\n";
        return 2;
    }

    std::vector<uint8_t> bytes{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()};

    RISCV_CPU cpu;
    load_binary(cpu, bytes);
    
    // Create SDL Window
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window   *window    = NULL;
    SDL_Renderer *renderer  = NULL;

    int width  = 640;
    int height = 400;
    int scale = 4;

    SDL_CreateWindowAndRenderer(width * scale, height * scale, 0, &window, &renderer);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    SDL_Event event;
    bool running = true;
    while(running) {

        // Get KeyBoard Input
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_KEYDOWN){
                unsigned char doomKey = convertToDoomKey(event.key.keysym.sym);
                cpu.put_keyBoard((1u << 8) | doomKey);
            } else if(event.type == SDL_KEYUP){
                unsigned char doomKey = convertToDoomKey(event.key.keysym.sym);
                cpu.put_keyBoard((0 << 8) | doomKey);
            }
            if(event.type == SDL_QUIT){
                running = false;
            }
        }

        // Process Guest Program
        for(int i = 0; i <  steps; i++)
            cpu.step();
    
        // Update Screen;
        std::vector<uint32_t>frameBuffer = cpu.getFrameBuffer();

        SDL_UpdateTexture(texture, nullptr, frameBuffer.data(), width * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    
    return 0;
}
