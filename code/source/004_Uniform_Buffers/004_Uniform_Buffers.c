#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define sdl_check(aCondition, aMessage, ...) \
    do { \
        if (!aCondition) { \
          SDL_Log("%s(%d):  " aMessage, __FILE__, __LINE__,##__VA_ARGS__ ); \
          SDL_Log("\tSDL_Error: %s", SDL_GetError()); \
          SDL_Quit(); \
          exit(1); \
        } \
    } while (0)

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
  gContext.mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
  sdl_check(gContext.mDevice, "Couldn't create a GPU Device: ");

  sdl_check(SDL_ClaimWindowForGPUDevice(gContext.mDevice, gContext.mWindow), "Couldn't claim the Window for the GPU device: ");

  gContext.mProperties = SDL_CreateProperties();
  sdl_check(gContext.mProperties, "Couldn't create a property set for GPU device calls: ");

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
  sprintf(shader_path, "Assets/Shaders/%s.%s", aShaderFilename, gContext.mChosenBackendFormatExtension);

  size_t fileSize = 0;
  void* fileData = SDL_LoadFile(shader_path, &fileSize);
  sdl_check(fileData, "Couldn't load the shader file (%s) from disk: ", shader_path);

  SDL_GPUShaderCreateInfo shaderCreateInfo;
  SDL_zero(shaderCreateInfo);

  SDL_PropertiesID properties = gContext.mProperties;

  if (aProperties != SDL_PROPERTY_TYPE_INVALID) {
    properties = aProperties;
  }

  sdl_check(
    SDL_SetStringProperty(properties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, aShaderFilename),
    "While creating a shader (%s), there was an error setting the GPU Shader Name property: ",
    shader_path
  );

  shaderCreateInfo.entrypoint = gContext.mShaderEntryPoint;
  shaderCreateInfo.format = gContext.mChosenBackendFormat;
  shaderCreateInfo.code = fileData;
  shaderCreateInfo.code_size = fileSize;
  shaderCreateInfo.stage = aShaderStage;
  shaderCreateInfo.num_samplers = aSamplerCount;
  shaderCreateInfo.num_uniform_buffers = aUniformBufferCount;
  shaderCreateInfo.num_storage_buffers = aStorageBufferCount;
  shaderCreateInfo.num_storage_textures = aStorageTextureCount;
  shaderCreateInfo.props = properties;

  SDL_GPUShader* shader = SDL_CreateGPUShader(gContext.mDevice, &shaderCreateInfo);

  SDL_free(fileData);
  sdl_check(shader, "While creating a shader (%s), there was an error creating the GPU shader: ", shader_path);

  return shader;
}

typedef struct TrianglePipeline {
  SDL_GPUGraphicsPipeline* mPipeline;
  SDL_FRect mPosition;
} TrianglePipeline;

TrianglePipeline CreateTrianglePipeline() {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);
  colorTargetDescription.blend_state.enable_blend = true;
  colorTargetDescription.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDescription.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDescription.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDescription.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  colorTargetDescription.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDescription.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "Triangle.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    1,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.vertex_shader, "Failed to create the Triangle vertex shader: ");

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "Triangle.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    0,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.fragment_shader, "Failed to create the Triangle fragment shader: ");

  sdl_check(
    SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "TrianglePipeline"),
    "While creating the TrianglePipeline, there was an error setting the GPU Shader Name property: "
  );

  TrianglePipeline pipeline;
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  sdl_check(pipeline.mPipeline, "Failed to create the GPU Pipeline: ");

  pipeline.mPosition.x = 0.f;
  pipeline.mPosition.y = 0.f;
  pipeline.mPosition.w = 0.f;
  pipeline.mPosition.h = 0.f;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return pipeline;
}

void DrawTrianglePipeline(TrianglePipeline* aPipeline, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_BindGPUGraphicsPipeline(aRenderPass, aPipeline->mPipeline);
  SDL_PushGPUComputeUniformData(aCommandBuffer, 0, &aPipeline->mPosition, sizeof(aPipeline->mPosition));
  SDL_DrawGPUPrimitives(aRenderPass, 3, 1, 0, 0);
}

void DestroyTrianglePipeline(TrianglePipeline* aPipeline)
{
  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aPipeline->mPipeline);
  SDL_zero(*aPipeline);
}

////////////////////////////////////////////////////////////
/// FullscreenPipeline
typedef struct FullscreenPipeline {
  SDL_GPUGraphicsPipeline* mPipeline;
  int mColorIndex;
} FullscreenPipeline;

FullscreenPipeline CreateFullscreenPipeline() {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);
  colorTargetDescription.blend_state.enable_blend = true;
  colorTargetDescription.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDescription.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDescription.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDescription.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  colorTargetDescription.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDescription.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "FullscreenTriangle.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.vertex_shader, "Failed to create the Triangle vertex shader: ");

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "FullscreenTriangle.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    0,
    1,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.fragment_shader, "Failed to create the Triangle fragment shader: ");

  sdl_check(
    SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "FullscreenPipeline"),
    "While creating the FullscreenPipeline, there was an error setting the GPU Shader Name property: "
  );

  FullscreenPipeline pipeline;
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  sdl_check(pipeline.mPipeline, "Failed to create the GPU Pipeline: ");
  pipeline.mColorIndex = 0;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return pipeline;
}

void DrawFullscreenPipeline(FullscreenPipeline* aPipeline, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_Color colors[] = {
    {255, 0, 0, 255},
    {0, 255, 0, 255},
    {0, 0, 255, 255}
  };

  SDL_BindGPUGraphicsPipeline(aRenderPass, aPipeline->mPipeline);
  SDL_PushGPUFragmentUniformData(aCommandBuffer, 0, &colors[aPipeline->mColorIndex], sizeof(SDL_Color));
  SDL_DrawGPUPrimitives(aRenderPass, 3, 1, 0, 0);
}

void DestroyFullscreenPipeline(FullscreenPipeline* aPipeline)
{
  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aPipeline->mPipeline);
  SDL_zero(*aPipeline);
}

//002_Window_and_Clearing__Running.jpg


int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  sdl_check(SDL_Init(SDL_INIT_VIDEO), "Couldn't initialize SDL: ");

  SDL_Window* window = SDL_CreateWindow("003-Triangle_and_Fullscreen_Triangle", 1280, 720, 0);
  sdl_check(window, "Couldn't create a window: ");

  CreateGpuContext(window);

  FullscreenPipeline fullscreenPipeline = CreateFullscreenPipeline();
  TrianglePipeline trianglePipeline = CreateTrianglePipeline();

  const float speed = 0.005f;
  float dt = 1.0f;
  int keys;
  const bool* key_map = SDL_GetKeyboardState(&keys);
  bool running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.common.type) {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.scancode) {
          case SDL_SCANCODE_1: fullscreenPipeline.mColorIndex = 0; break;
          case SDL_SCANCODE_2: fullscreenPipeline.mColorIndex = 1; break;
          case SDL_SCANCODE_3: fullscreenPipeline.mColorIndex = 2; break;
        }
        break;
      }
    }

    if (key_map[SDL_SCANCODE_A]) trianglePipeline.mPosition.x += speed * dt * 1.0f;


    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    if (!commandBuffer)
    {
      SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
      continue;
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, gContext.mWindow, &swapchainTexture, NULL, NULL))
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
    colorTargetInfo.clear_color.g = 0.2f;
    colorTargetInfo.clear_color.b = 0.85f;
    colorTargetInfo.clear_color.a = 1.0f;

    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
      commandBuffer,
      &colorTargetInfo,
      1,
      NULL
    );

    DrawFullscreenPipeline(&fullscreenPipeline, commandBuffer, renderPass);
    DrawTrianglePipeline(&trianglePipeline, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyTrianglePipeline(&trianglePipeline);
  DestroyFullscreenPipeline(&fullscreenPipeline);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}