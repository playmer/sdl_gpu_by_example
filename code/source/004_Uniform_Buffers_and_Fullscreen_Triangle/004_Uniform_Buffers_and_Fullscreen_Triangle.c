#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>


#ifndef __cplusplus
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#else
namespace cpp_test {
#endif

#if 0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MATH
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct float2 {
  float x, y;
} float2;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shared GPU Code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct GpuContext {
  SDL_Window* mWindow;
  SDL_GPUDevice* mDevice;
  SDL_PropertiesID mProperties;
  const char* mShaderEntryPoint;
  SDL_GPUShaderFormat mChosenBackendFormat;
  const char* mChosenBackendFormatExtension;
} GpuContext;

GpuContext gContext;

void CreateGpuContext(SDL_Window* aWindow) {
  SDL_zero(gContext);

  gContext.mWindow = aWindow;
  {
    gContext.mProperties = SDL_CreateProperties();
    SDL_SetBooleanProperty(gContext.mProperties, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, true);
    //SDL_SetBooleanProperty(gContext.mProperties, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, true);
    SDL_SetBooleanProperty(gContext.mProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
    SDL_SetBooleanProperty(gContext.mProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true);
    SDL_SetBooleanProperty(gContext.mProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, true);

    gContext.mDevice = SDL_CreateGPUDeviceWithProperties(gContext.mProperties);
  }


  //gContext.mDevice = SDL_CreateGPUDevice(
  //  SDL_GPU_SHADERFORMAT_SPIRV |
  //  SDL_GPU_SHADERFORMAT_DXIL |
  //  SDL_GPU_SHADERFORMAT_MSL,
  //  true,
  //  NULL);
  SDL_assert(gContext.mDevice);

  SDL_assert(SDL_ClaimWindowForGPUDevice(gContext.mDevice, gContext.mWindow));

  SDL_assert(gContext.mProperties);

  SDL_GPUShaderFormat availableFormats = SDL_GetGPUShaderFormats(gContext.mDevice);
  gContext.mShaderEntryPoint = NULL;

  if (availableFormats & SDL_GPU_SHADERFORMAT_SPIRV)
  {
    gContext.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_SPIRV;
    gContext.mShaderEntryPoint = "main";
    gContext.mChosenBackendFormatExtension = "spv";
  }
  else if (availableFormats & SDL_GPU_SHADERFORMAT_MSL)
  {
    gContext.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_MSL;
    gContext.mShaderEntryPoint = "main0";
    gContext.mChosenBackendFormatExtension = "msl";
  }
  else if (availableFormats & SDL_GPU_SHADERFORMAT_DXIL)
  {
    gContext.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_DXIL;
    gContext.mShaderEntryPoint = "main";
    gContext.mChosenBackendFormatExtension = "dxil";
  }
}

void DestroyGpuContext() {
  SDL_DestroyProperties(gContext.mProperties);
  SDL_DestroyGPUDevice(gContext.mDevice);
  SDL_DestroyWindow(gContext.mWindow);
  SDL_zero(gContext);
}

SDL_GPUShader* CreateShader(
  const char* aShaderFilename,
  SDL_GPUShaderStage aShaderStage,
  Uint32 aSamplerCount,
  Uint32 aUniformBufferCount,
  Uint32 aStorageBufferCount,
  Uint32 aStorageTextureCount,
  SDL_PropertiesID aProperties)
{
  char shader_path[4096];
  SDL_snprintf(shader_path, SDL_arraysize(shader_path), "Assets/Shaders/%s/%s.%s", TARGET_NAME, aShaderFilename, gContext.mChosenBackendFormatExtension);

  size_t fileSize = 0;
  void* fileData = SDL_LoadFile(shader_path, &fileSize);
  SDL_assert(fileData);

  SDL_GPUShaderCreateInfo shaderCreateInfo;
  SDL_zero(shaderCreateInfo);

  SDL_PropertiesID properties = gContext.mProperties;

  if (aProperties != SDL_PROPERTY_TYPE_INVALID) {
    properties = aProperties;
  }

  SDL_assert(SDL_SetStringProperty(properties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, aShaderFilename));

  shaderCreateInfo.entrypoint = gContext.mShaderEntryPoint;
  shaderCreateInfo.format = gContext.mChosenBackendFormat;
  shaderCreateInfo.code = (Uint8*)fileData;
  shaderCreateInfo.code_size = fileSize;
  shaderCreateInfo.stage = aShaderStage;
  shaderCreateInfo.num_samplers = aSamplerCount;
  shaderCreateInfo.num_uniform_buffers = aUniformBufferCount;
  shaderCreateInfo.num_storage_buffers = aStorageBufferCount;
  shaderCreateInfo.num_storage_textures = aStorageTextureCount;
  shaderCreateInfo.props = properties;

  SDL_GPUShader* shader = SDL_CreateGPUShader(gContext.mDevice, &shaderCreateInfo);

  SDL_free(fileData);
  SDL_assert(shader);

  return shader;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Technique Code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// FullscreenContext
typedef struct FullscreenContext {
  SDL_GPUGraphicsPipeline* mPipeline;
  float2 mOffset;
  int mColorIndex;
} FullscreenContext;

FullscreenContext CreateFullscreenContext() {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "FullscreenTriangle.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    1,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.vertex_shader);

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "FullscreenTriangle.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    0,
    1,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.fragment_shader);

  SDL_assert(SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "FullscreenContext"));

  FullscreenContext pipeline;
  SDL_zero(pipeline);
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  pipeline.mOffset.x = 0.5f;
  pipeline.mOffset.y = 0.5f;
  SDL_assert(pipeline.mPipeline);

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return pipeline;
}

void DrawFullscreenContext(FullscreenContext* aPipeline, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_FColor colors[] = {
    {1, 0, 0, 1},
    {0, 1, 0, 1},
    {0, 0, 1, 1}
  };

  SDL_BindGPUGraphicsPipeline(aRenderPass, aPipeline->mPipeline);
  SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aPipeline->mOffset, sizeof(float2));
  SDL_PushGPUFragmentUniformData(aCommandBuffer, 0, &colors[aPipeline->mColorIndex], sizeof(SDL_FColor));
  SDL_DrawGPUPrimitives(aRenderPass, 3, 1, 0, 0);
}

void DestroyFullscreenContext(FullscreenContext* aPipeline)
{
  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aPipeline->mPipeline);
  SDL_zero(*aPipeline);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SDL_Window* window = NULL;;
FullscreenContext fullscreenContext;
const float speed = 1.f;
Uint64 last_frame_ticks_so_far = 0;
int keys = 0;
const bool* key_map = NULL;
bool running = true;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
  (void)argc;
  (void)argv;
  SDL_assert(SDL_Init(SDL_INIT_VIDEO));

  window = SDL_CreateWindow(TARGET_NAME, 1280, 720, SDL_WINDOW_RESIZABLE);
  SDL_assert(window);

  CreateGpuContext(window);

  fullscreenContext = CreateFullscreenContext();

  last_frame_ticks_so_far = SDL_GetTicksNS();
  key_map = SDL_GetKeyboardState(&keys);
  running = true;

  SDL_SetGPUAllowedFramesInFlight(gContext.mDevice, 1);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
  Uint64 current_frame_ticks_so_far = SDL_GetTicksNS();
  float dt = (current_frame_ticks_so_far - last_frame_ticks_so_far) / 1000000000.f;
  last_frame_ticks_so_far = current_frame_ticks_so_far;


  if (key_map[SDL_SCANCODE_D]) fullscreenContext.mOffset.x += speed * dt * 1.0f;
  if (key_map[SDL_SCANCODE_A]) fullscreenContext.mOffset.x -= speed * dt * 1.0f;
  if (key_map[SDL_SCANCODE_W]) fullscreenContext.mOffset.y += speed * dt * 1.0f;
  if (key_map[SDL_SCANCODE_S]) fullscreenContext.mOffset.y -= speed * dt * 1.0f;


  SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
  if (!commandBuffer)
  {
    SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
    return SDL_APP_CONTINUE;;
  }

  SDL_GPUTexture* swapchainTexture;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, gContext.mWindow, &swapchainTexture, NULL, NULL))
  {
    SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
    return SDL_APP_CONTINUE;
  }

  SDL_GPUColorTargetInfo colorTargetInfo;
  SDL_zero(colorTargetInfo);

  colorTargetInfo.texture = swapchainTexture;
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.clear_color.r = 1.f;
  colorTargetInfo.clear_color.g = 1.f;
  colorTargetInfo.clear_color.b = 1.f;
  colorTargetInfo.clear_color.a = 1.f;

  SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
    commandBuffer,
    &colorTargetInfo,
    1,
    NULL
  );

  DrawFullscreenContext(&fullscreenContext, commandBuffer, renderPass);

  SDL_EndGPURenderPass(renderPass);
  SDL_SubmitGPUCommandBuffer(commandBuffer);

  SDL_WaitForGPUSwapchain(gContext.mDevice, gContext.mWindow);
  SDL_WaitForGPUIdle(gContext.mDevice);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  switch (event->common.type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
      break;
    case SDL_EVENT_KEY_DOWN:
      switch (event->key.scancode) {
      case SDL_SCANCODE_1: fullscreenContext.mColorIndex = 0; break;
      case SDL_SCANCODE_2: fullscreenContext.mColorIndex = 1; break;
      case SDL_SCANCODE_3: fullscreenContext.mColorIndex = 2; break;
      default: break;
      }
      break;
    case SDL_EVENT_WINDOW_EXPOSED:
    {
      SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
      if (!commandBuffer)
      {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return SDL_APP_CONTINUE;;
      }

      SDL_GPUTexture* swapchainTexture;
      int w_w = 0, w_h = 0;
      Uint32 s_w = 0, s_h = 0;
      if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, gContext.mWindow, &swapchainTexture, &s_w, &s_h))
      {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return SDL_APP_CONTINUE;
      }

      SDL_GetWindowSizeInPixels(gContext.mWindow, &w_w, &w_h);

      SDL_GPUColorTargetInfo colorTargetInfo;
      SDL_zero(colorTargetInfo);

      colorTargetInfo.texture = swapchainTexture;
      colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
      colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
      colorTargetInfo.clear_color.r = 1.f;
      colorTargetInfo.clear_color.g = 1.f;
      colorTargetInfo.clear_color.b = 1.f;
      colorTargetInfo.clear_color.a = 1.f;

      SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
        commandBuffer,
        &colorTargetInfo,
        1,
        NULL
      );

      DrawFullscreenContext(&fullscreenContext, commandBuffer, renderPass);

      SDL_EndGPURenderPass(renderPass);
      SDL_SubmitGPUCommandBuffer(commandBuffer);

      SDL_WaitForGPUSwapchain(gContext.mDevice, gContext.mWindow);
      //SDL_WaitForGPUIdle(gContext.mDevice);
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  DestroyFullscreenContext(&fullscreenContext);

  DestroyGpuContext();

  SDL_Quit();
}

#else

SDL_Window* gWindow;
SDL_Renderer* gRenderer;

SDL_FRect RightAlignedRect()
{
  SDL_FRect rect;

  int w = 0, h = 0;
  SDL_GetWindowSizeInPixels(gWindow, &w, &h);

  rect.x = w - 100;
  rect.y = 200;
  rect.w = 100;
  rect.h = 100;

  return rect;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
  (void)argc;
  (void)argv;
  SDL_assert(SDL_Init(SDL_INIT_VIDEO));

  gWindow = SDL_CreateWindow("", 1280, 720, SDL_WINDOW_RESIZABLE);

  gRenderer = SDL_CreateRenderer(gWindow, "opengl");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
  SDL_FRect rect = RightAlignedRect();

  SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
  SDL_RenderClear(gRenderer);

  SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
  SDL_RenderFillRect(gRenderer, &rect);
  SDL_RenderPresent(gRenderer);

  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  switch (event->common.type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
      break;
    case SDL_EVENT_WINDOW_EXPOSED:
    {
      SDL_FRect rect = RightAlignedRect();

      SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
      SDL_RenderClear(gRenderer);

      SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
      SDL_RenderFillRect(gRenderer, &rect);
      SDL_RenderPresent(gRenderer);
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  SDL_Quit();
}


#endif

#ifdef __cplusplus
} // end cpp_test
#endif
