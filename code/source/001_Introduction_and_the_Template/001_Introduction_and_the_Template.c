#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

int main(int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }
  
  SDL_Log("Everything is working.");

  SDL_Quit();
  return 0;
}