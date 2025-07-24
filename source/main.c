#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include <stdio.h>

int main(int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("Couldn't initialize SDL: ", SDL_GetError());
    return 1;
  }
  
  printf("Everything is working.", SDL_GetError());

  SDL_Quit();
  return 0;
}