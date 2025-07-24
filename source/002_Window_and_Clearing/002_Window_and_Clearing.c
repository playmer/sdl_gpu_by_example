#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#define sdl_check(aCondition, aMessage, aExitCode) \
    do { \
        if (!aCondition) { \
            SDL_Quit(); \
            exit(aExitCode); \
        } \
    } while (0)

typedef struct GpuContext {
  SDL_Window* mWindow;
  SDL_GPUDevice* mDevice;
  SDL_PropertiesID mProperties;
  const char* mShaderEntryPoint;
  SDL_GPUShaderFormat mChosenBackendFormat;
} GpuContext;

GpuContext CreateGpuContext(SDL_Window* aWindow) {
  GpuContext context;
  SDL_zero(context);

  context.mWindow = aWindow;
  context.mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
  sdl_check(context.mDevice, "Couldn't create a GPU Device: ", 2);

  sdl_check(SDL_ClaimWindowForGPUDevice(context.mDevice, context.mWindow), "Couldn't claim the Window for the GPU device: ", 3);

  context.mProperties = SDL_CreateProperties();
  sdl_check(context.mProperties, "Couldn't create a property set for GPU device calls: ", 4);

  SDL_GPUShaderFormat availableFormats = SDL_GetGPUShaderFormats(context.mDevice);
  context.mShaderEntryPoint = NULL;

  if (availableFormats & SDL_GPU_SHADERFORMAT_SPIRV)
  {
    context.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_SPIRV;
    context.mShaderEntryPoint = "main";
  }
  else if (availableFormats & SDL_GPU_SHADERFORMAT_MSL)
  {
    context.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_MSL;
    context.mShaderEntryPoint = "main0";
  }
  else if (availableFormats & SDL_GPU_SHADERFORMAT_DXIL)
  {
    context.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_DXIL;
    context.mShaderEntryPoint = "main";
  }

  return context;
}

void DestroyGpuContext(GpuContext* aContext) {
  SDL_DestroyProperties(aContext->mProperties);
  SDL_DestroyGPUDevice(aContext->mDevice);
  SDL_DestroyWindow(aContext->mWindow);
  SDL_zero(*aContext);
}


int main(int argc, char** argv)
{
  sdl_check(SDL_Init(SDL_INIT_VIDEO), "Couldn't initialize SDL: ", 1);

  SDL_Window* window = SDL_CreateWindow("002-Window_and_Clearing", 1280, 720, 0);
  sdl_check(window, "Couldn't create a window: ", 1);

  GpuContext context = CreateGpuContext(window);
  
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

    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(context.mDevice);
    if (!commandBuffer)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        continue;
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, context.mWindow, &swapchainTexture, NULL, NULL))
    {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        continue;
    }

    SDL_GPUColorTargetInfo colorTargetInfo;
    SDL_zero(colorTargetInfo);

    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.clear_color.r = 0.2f;
    colorTargetInfo.clear_color.g= 0.2f;
    colorTargetInfo.clear_color.b = 0.85f;
    colorTargetInfo.clear_color.a = 1.0f;

    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
        commandBuffer,
        &colorTargetInfo,
        1,
        NULL
    );

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyGpuContext(&context);

  SDL_Quit();
  return 0;
}