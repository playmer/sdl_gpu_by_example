#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#define sdl_check(aCondition, aMessage, aExitCode) \
    do { \
        if (!aCondition) { \
            SDL_Quit(); \
            exit(aExitCode); \
        } \
    } while (0)


int main(int argc, char** argv)
{
  sdl_check(SDL_Init(SDL_INIT_VIDEO), "Couldn't initialize SDL: ", 1);

  SDL_Window* window = SDL_CreateWindow("002-Window_and_Clearing", 1280, 720, 0);
  sdl_check(window, "Couldn't create a window: ", 1);
  
  bool running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.common.type) {
          case SDL_EVENT_QUIT:
            running = false;
            break;
        }
    }
  }

  SDL_Quit();
  return 0;
}