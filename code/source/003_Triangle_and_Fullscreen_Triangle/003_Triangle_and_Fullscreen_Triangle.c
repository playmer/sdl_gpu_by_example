#include <stdio.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#define sdl_check(aCondition, aMessage, ...) \
    do { \
        if (!aCondition) { \
          SDL_Log("%s(%d):  " aMessage, __FILE__, __LINE__, __VA_ARGS__ ); \
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

GpuContext CreateGpuContext(SDL_Window* aWindow) {
  GpuContext context;
  SDL_zero(context);

  context.mWindow = aWindow;
  context.mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
  sdl_check(context.mDevice, "Couldn't create a GPU Device: ");

  sdl_check(SDL_ClaimWindowForGPUDevice(context.mDevice, context.mWindow), "Couldn't claim the Window for the GPU device: ");

  context.mProperties = SDL_CreateProperties();
  sdl_check(context.mProperties, "Couldn't create a property set for GPU device calls: ");

  SDL_GPUShaderFormat availableFormats = SDL_GetGPUShaderFormats(context.mDevice);
  context.mShaderEntryPoint = NULL;

  if (availableFormats & SDL_GPU_SHADERFORMAT_SPIRV)
  {
    context.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_SPIRV;
    context.mShaderEntryPoint = "main";
    context.mChosenBackendFormatExtension = "spv";
  }
  else if (availableFormats & SDL_GPU_SHADERFORMAT_MSL)
  {
    context.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_MSL;
    context.mShaderEntryPoint = "main0";
    context.mChosenBackendFormatExtension = "msl";
  }
  else if (availableFormats & SDL_GPU_SHADERFORMAT_DXIL)
  {
    context.mChosenBackendFormat = SDL_GPU_SHADERFORMAT_DXIL;
    context.mShaderEntryPoint = "main";
    context.mChosenBackendFormatExtension = "dxil";
  }

  return context;
}

void DestroyGpuContext(GpuContext* aContext) {
  SDL_DestroyProperties(aContext->mProperties);
  SDL_DestroyGPUDevice(aContext->mDevice);
  SDL_DestroyWindow(aContext->mWindow);
  SDL_zero(*aContext);
}

SDL_GPUShader* CreateShader(
  GpuContext* aContext,
  const char* aShaderFilename,
  SDL_GPUShaderStage aShaderStage,
  Uint32 aSamplerCount,
  Uint32 aUniformBufferCount,
  Uint32 aStorageBufferCount,
  Uint32 aStorageTextureCount,
  SDL_PropertiesID aProperties)
{
  char shader_path[4096];
  sprintf(shader_path, "Assets/Shaders/%s.%s", aShaderFilename, aContext->mChosenBackendFormatExtension);

  size_t fileSize = 0;
  void* fileData = SDL_LoadFile(shader_path, &fileSize);
  sdl_check(fileData, "Couldn't load the shader file (%s) from disk: ", shader_path);

  SDL_GPUShaderCreateInfo shaderCreateInfo;
  SDL_zero(shaderCreateInfo);

  SDL_PropertiesID properties = aContext->mProperties;

  if (aProperties != SDL_PROPERTY_TYPE_INVALID) {
    properties = aProperties;
  }

  sdl_check(
    SDL_SetStringProperty(properties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, aShaderFilename),
    "While creating a shader (%s), there was an error setting the GPU Shader Name property: ",
    shader_path
  );

  shaderCreateInfo.entrypoint = aContext->mShaderEntryPoint;
  shaderCreateInfo.format = aContext->mChosenBackendFormat;
  shaderCreateInfo.code = fileData;
  shaderCreateInfo.code_size = fileSize;
  shaderCreateInfo.stage = aShaderStage;
  shaderCreateInfo.num_samplers = aSamplerCount;
  shaderCreateInfo.num_uniform_buffers = aUniformBufferCount;
  shaderCreateInfo.num_storage_buffers = aStorageBufferCount;
  shaderCreateInfo.num_storage_textures = aStorageTextureCount;
  shaderCreateInfo.props = properties;

  SDL_GPUShader* shader = SDL_CreateGPUShader(aContext->mDevice, &shaderCreateInfo);

  SDL_free(fileData);
  sdl_check(shader, "While creating a shader (%s), there was an error creating the GPU shader: ", shader_path);

  return shader;
}

typedef struct TrianglePipeline {
  SDL_GPUGraphicsPipeline* mPipeline;
} TrianglePipeline;

TrianglePipeline CreateTrianglePipeline(GpuContext* aContext) {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(aContext->mDevice, aContext->mWindow);
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
    aContext,
    "Triangle.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.vertex_shader, "Failed to create the Triangle vertex shader: ");

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    aContext,
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
    SDL_SetStringProperty(aContext->mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "TrianglePipeline"),
    "While creating the TrianglePipeline, there was an error setting the GPU Shader Name property: "
  );

  TrianglePipeline pipeline;
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(aContext->mDevice, &graphicsPipelineCreateInfo);
  sdl_check(pipeline.mPipeline, "Failed to create the GPU Pipeline: ");

  return pipeline;
}


int main(int argc, char** argv)
{
  sdl_check(SDL_Init(SDL_INIT_VIDEO), "Couldn't initialize SDL: ");

  SDL_Window* window = SDL_CreateWindow("003-Triangle_and_Fullscreen_Triangle", 1280, 720, 0);
  sdl_check(window, "Couldn't create a window: ");

  GpuContext context = CreateGpuContext(window);

  TrianglePipeline trianglePipeline = CreateTrianglePipeline(&context);

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
    colorTargetInfo.clear_color.g = 0.2f;
    colorTargetInfo.clear_color.b = 0.85f;
    colorTargetInfo.clear_color.a = 1.0f;

    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
      commandBuffer,
      &colorTargetInfo,
      1,
      NULL
    );

    SDL_BindGPUGraphicsPipeline(renderPass, trianglePipeline.mPipeline);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyGpuContext(&context);

  SDL_Quit();
  return 0;
}