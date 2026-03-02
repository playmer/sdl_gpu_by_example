#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

// This is for testing to ensure the code works in both C and C++,
// this entire preprocessor block should just be the #include
// in your own code.
#ifndef __cplusplus
#include <SDL3/SDL_main.h>
#else
namespace cpp_test {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MATH
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct float2 {
  float x, y;
} float2;

typedef struct float3 {
  float x, y, z;
} float3;

typedef struct float4 {
  float x, y, z, w;
} float4;

//////////////////////////////////////////////////////
// Downcasts

float2 Float3_XY(float3 aValue) {
  float2 toReturn = { aValue.x, aValue.y };
  return toReturn;
}

float2 Float4_XY(float4 aValue) {
  float2 toReturn = { aValue.x, aValue.y };
  return toReturn;
}

float3 Float4_XYZ(float4 aValue) {
  float3 toReturn = { aValue.x, aValue.y, aValue.z };
  return toReturn;
}

//////////////////////////////////////////////////////
// Subtraction

float2 Float2_Subtract(float2 aLeft, float2 aRight) {
  float2 toReturn = { aLeft.x - aRight.x, aLeft.y - aRight.y };
  return toReturn;
}

float3 Float3_Subtract(float3 aLeft, float3 aRight) {
  float3 toReturn = { aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z };
  return toReturn;
}

float4 Float4_Subtract(float4 aLeft, float4 aRight) {
  float4 toReturn = { aLeft.x - aRight.x, aLeft.y - aRight.y, aLeft.z - aRight.z, aLeft.w - aRight.w };
  return toReturn;
}

//////////////////////////////////////////////////////
// Addition

float2 Float2_Add(float2 aLeft, float2 aRight) {
  float2 toReturn = { aLeft.x + aRight.x, aLeft.y + aRight.y };
  return toReturn;
}

float3 Float3_Add(float3 aLeft, float3 aRight) {
  float3 toReturn = { aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z };
  return toReturn;
}

float4 Float4_Add(float4 aLeft, float4 aRight) {
  float4 toReturn = { aLeft.x + aRight.x, aLeft.y + aRight.y, aLeft.z + aRight.z, aLeft.w + aRight.w };
  return toReturn;
}

//////////////////////////////////////////////////////
// Multiplication

float2 Float2_Multiply(float2 aLeft, float2 aRight) {
  float2 toReturn = { aLeft.x * aRight.x, aLeft.y * aRight.y };
  return toReturn;
}

float3 Float3_Multiply(float3 aLeft, float3 aRight) {
  float3 toReturn = { aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z };
  return toReturn;
}

float4 Float4_Multiply(float4 aLeft, float4 aRight) {
  float4 toReturn = { aLeft.x * aRight.x, aLeft.y * aRight.y, aLeft.z * aRight.z, aLeft.w * aRight.w };
  return toReturn;
}

//////////////////////////////////////////////////////
// Scalar Addition

float2 Float2_Scalar_Add(float2 aLeft, float aRight) {
  float2 toReturn = { aLeft.x + aRight, aLeft.y + aRight };
  return toReturn;
}

float3 Float3_Scalar_Add(float3 aLeft, float aRight) {
  float3 toReturn = { aLeft.x + aRight, aLeft.y + aRight, aLeft.z + aRight };
  return toReturn;
}

float4 Float4_Scalar_Add(float4 aLeft, float aRight) {
  float4 toReturn = { aLeft.x + aRight, aLeft.y + aRight, aLeft.z + aRight, aLeft.w + aRight };
  return toReturn;
}

//////////////////////////////////////////////////////
// Scalar Multiplication

float2 Float2_Scalar_Multiply(float2 aLeft, float aRight) {
  float2 toReturn = { aLeft.x * aRight, aLeft.y * aRight };
  return toReturn;
}

float3 Float3_Scalar_Multiply(float3 aLeft, float aRight) {
  float3 toReturn = { aLeft.x * aRight, aLeft.y * aRight, aLeft.z * aRight };
  return toReturn;
}

float4 Float4_Scalar_Multiply(float4 aLeft, float aRight) {
  float4 toReturn = { aLeft.x * aRight, aLeft.y * aRight, aLeft.z * aRight, aLeft.w * aRight };
  return toReturn;
}

//////////////////////////////////////////////////////
// Scalar Divison

float2 Float2_Scalar_Division(float2 aLeft, float aRight) {
  float2 toReturn = { aLeft.x / aRight, aLeft.y / aRight };
  return toReturn;
}

float3 Float3_Scalar_Division(float3 aLeft, float aRight) {
  float3 toReturn = { aLeft.x / aRight, aLeft.y / aRight, aLeft.z / aRight };
  return toReturn;
}

float4 Float4_Scalar_Division(float4 aLeft, float aRight) {
  float4 toReturn = { aLeft.x / aRight, aLeft.y / aRight, aLeft.z / aRight, aLeft.w / aRight };
  return toReturn;
}

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
  gContext.mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
  SDL_assert(gContext.mDevice);

  SDL_assert(SDL_ClaimWindowForGPUDevice(gContext.mDevice, gContext.mWindow));

  gContext.mProperties = SDL_CreateProperties();
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

  SDL_PropertiesID properties = gContext.mProperties;

  if (aProperties != 0) {
    properties = aProperties;
  }

  SDL_assert(SDL_SetStringProperty(properties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, aShaderFilename));

  SDL_GPUShaderCreateInfo shaderCreateInfo;
  SDL_zero(shaderCreateInfo);

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
  float2 mPositon;
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
  pipeline.mPositon.x = 0.5f;
  pipeline.mPositon.y = 0.5f;
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
  SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aPipeline->mPositon, sizeof(float2));
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
int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  SDL_assert(SDL_Init(SDL_INIT_VIDEO));

  SDL_Window* window = SDL_CreateWindow(TARGET_NAME, 1280, 720, 0);
  SDL_assert(window);

  CreateGpuContext(window);

  FullscreenContext fullscreenContext = CreateFullscreenContext();

  const float speed = 1.f;
  Uint64 last_frame_ticks_so_far = SDL_GetTicksNS();	
  int keys;
  const bool* key_map = SDL_GetKeyboardState(&keys);
  bool running = true;

  while (running) {
    Uint64 current_frame_ticks_so_far = SDL_GetTicksNS();
    float dt = (current_frame_ticks_so_far - last_frame_ticks_so_far) / 1000000000.f;
    last_frame_ticks_so_far = current_frame_ticks_so_far;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.common.type) {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.scancode) {
          case SDL_SCANCODE_1: fullscreenContext.mColorIndex = 0; break;
          case SDL_SCANCODE_2: fullscreenContext.mColorIndex = 1; break;
          case SDL_SCANCODE_3: fullscreenContext.mColorIndex = 2; break;
          default: break;
        }
        break;
      }
    }

    if (key_map[SDL_SCANCODE_D]) fullscreenContext.mPositon.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_A]) fullscreenContext.mPositon.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_W]) fullscreenContext.mPositon.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_S]) fullscreenContext.mPositon.y -= speed * dt * 1.0f;


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

    DrawFullscreenContext(&fullscreenContext, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyFullscreenContext(&fullscreenContext);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}

#ifdef __cplusplus
} // end cpp_test
#endif
