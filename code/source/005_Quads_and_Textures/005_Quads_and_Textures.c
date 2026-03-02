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


SDL_GPUTexture* CreateTexture(Uint32 aWidth, Uint32 aHeight, SDL_GPUTextureUsageFlags aUsage, SDL_GPUTextureFormat aFormat, const char* aName)
{
  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, aName);

  SDL_GPUTextureCreateInfo textureCreateInfo;
  SDL_zero(textureCreateInfo);
  textureCreateInfo.width = aWidth;
  textureCreateInfo.height = aHeight;
  textureCreateInfo.layer_count_or_depth = 1;
  textureCreateInfo.num_levels = 1;
  textureCreateInfo.usage = aUsage;
  textureCreateInfo.format = aFormat;
  return SDL_CreateGPUTexture(gContext.mDevice, &textureCreateInfo);
}

SDL_GPUTexture* CreateAndUploadTexture(SDL_GPUCopyPass* aCopyPass, const char* aTextureName) {
  char texture_path[4096];
  SDL_snprintf(texture_path, SDL_arraysize(texture_path), "Assets/Images/%s.bmp", aTextureName);
  SDL_Surface* surface = SDL_LoadBMP(texture_path);
  if (surface->format != SDL_PIXELFORMAT_RGBA32)
  {
    SDL_Surface* temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    surface = temp;
  }

  SDL_GPUTransferBufferCreateInfo transferCreateInfo;
  SDL_zero(transferCreateInfo);
  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, aTextureName);
  transferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferCreateInfo.props = gContext.mProperties;
  
  {
    const SDL_PixelFormatDetails* formatDetails = SDL_GetPixelFormatDetails(surface->format);
    transferCreateInfo.size = surface->h * surface->pitch;
  }

  SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(gContext.mDevice, &transferCreateInfo);
  void* transferPtr = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
  memcpy(transferPtr, surface->pixels, transferCreateInfo.size);
  SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer);

  SDL_GPUCommandBuffer* commandBuffer = NULL;
  SDL_GPUCopyPass* copyPass = aCopyPass;
  bool needsToSubmit = NULL == copyPass;
  if (needsToSubmit) {
    commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    copyPass = SDL_BeginGPUCopyPass(commandBuffer);
  }

  SDL_GPUTexture* texture = CreateTexture(surface->w, surface->h, SDL_GPU_TEXTUREUSAGE_SAMPLER, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, aTextureName);
  SDL_assert(texture);

  // Copy to GPU
  SDL_GPUTextureTransferInfo textureTransferInfo;
  SDL_zero(textureTransferInfo);
  textureTransferInfo.pixels_per_row = surface->w;
  textureTransferInfo.rows_per_layer = surface->h;
  textureTransferInfo.transfer_buffer = transferBuffer;

  SDL_GPUTextureRegion textureRegion;
  SDL_zero(textureRegion);
  textureRegion.texture = texture;
  textureRegion.w = surface->w;
  textureRegion.h = surface->h;
  textureRegion.d = 1;

  SDL_UploadToGPUTexture(
    copyPass,
    &textureTransferInfo,
    &textureRegion,
    false
  );

  if (needsToSubmit) {
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  SDL_ReleaseGPUTransferBuffer(gContext.mDevice, transferBuffer);
  SDL_DestroySurface(surface);

  return texture;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Technique Code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// QuadContext

typedef struct ModelUniform {
  float2 mPosition;
  float2 mScale;
} ModelUniform;

typedef struct QuadContext {
  SDL_GPUGraphicsPipeline* mPipeline;
  SDL_GPUTexture* mTexture;
  SDL_GPUSampler* mSampler;
  ModelUniform mUniform;
} QuadContext;

QuadContext CreateQuadContext() {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "Quad.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    1,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.vertex_shader);

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "Quad.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    1,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.fragment_shader);

  SDL_assert(SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "QuadContext"));

  QuadContext pipeline;
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  pipeline.mTexture = CreateAndUploadTexture(NULL, "sample");

  SDL_GPUSamplerCreateInfo samplerCreateInfo;
  SDL_zero(samplerCreateInfo);
  samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  pipeline.mSampler = SDL_CreateGPUSampler(gContext.mDevice, &samplerCreateInfo);
  SDL_assert(pipeline.mPipeline);

  pipeline.mUniform.mPosition.x = 0.f;
  pipeline.mUniform.mPosition.y = 0.f;
  pipeline.mUniform.mScale.x = 0.5f;
  pipeline.mUniform.mScale.y = 0.5f;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return pipeline;
}

void DrawQuadContext(QuadContext* aPipeline, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_BindGPUGraphicsPipeline(aRenderPass, aPipeline->mPipeline);
  SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aPipeline->mUniform, sizeof(aPipeline->mUniform));

  {
    SDL_GPUTextureSamplerBinding textureBinding;
    SDL_zero(textureBinding);
    textureBinding.texture = aPipeline->mTexture;
    textureBinding.sampler = aPipeline->mSampler;
    SDL_BindGPUFragmentSamplers(aRenderPass, 0, &textureBinding, 1);
  }

  SDL_DrawGPUPrimitives(aRenderPass, 6, 1, 0, 0);
}

void DestroyQuadContext(QuadContext* aPipeline)
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

  QuadContext quadContext = CreateQuadContext();

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
      }
    }      
      
    if (key_map[SDL_SCANCODE_D]) quadContext.mUniform.mPosition.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_A]) quadContext.mUniform.mPosition.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_W]) quadContext.mUniform.mPosition.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_S]) quadContext.mUniform.mPosition.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_R]) quadContext.mUniform.mScale.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_F]) quadContext.mUniform.mScale.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_T]) quadContext.mUniform.mScale.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_G]) quadContext.mUniform.mScale.y -= speed * dt * 1.0f;

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

    DrawQuadContext(&quadContext, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyQuadContext(&quadContext);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}

#ifdef __cplusplus
} // end cpp_test
#endif
