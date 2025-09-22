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
typedef struct float4 {
  float x,y,z,w;
} float4;

typedef struct float4x4 {
  union {
    float4 columns[4];
    float data[4][4];
  };
} float4x4;

float4x4 float4x4_multiply(const float4x4* aLeft, const float4x4* aRight)
{
  float4x4 toReturn;
  SDL_zero(toReturn);
  
  for (size_t j = 0; j < 4; ++j) // Column
    for (size_t i = 0; i < 4; ++i) // Row
      for (size_t n = 0; n < 4; ++n) // Iterative Muls
        toReturn.data[j][i] += aLeft->data[n][i] * aRight->data[j][n];

  return toReturn;
}

float4x4 OrthographicProjectionLHZO(float aLeft, float aRight, float aBottom, float aTop, float aNear, float aFar) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  toReturn.data[0][0] = 2.0f / (aRight - aLeft);
  toReturn.data[1][1] = 2.0f / (aTop - aBottom);
  toReturn.data[2][2] = 1.0f / (aFar - aNear);

  toReturn.data[3][0] = -(aRight + aLeft) / (aRight - aLeft);
  toReturn.data[3][1] = -(aTop + aBottom) / (aTop - aBottom);
  toReturn.data[3][2] = -aNear / (aFar - aNear);

  toReturn.data[3][3] = 1.0f;

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
  float4x4 WorldToNDC;
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
  SDL_snprintf(shader_path, SDL_arraysize(shader_path), "Assets/Shaders/%s.%s", aShaderFilename, gContext.mChosenBackendFormatExtension);

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

SDL_GPUBuffer* CreateGPUBuffer(Uint32 aSize, SDL_GPUBufferUsageFlags aUsage, const char* aName)
{
  SDL_GPUBufferCreateInfo createInfo;

  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_BUFFER_CREATE_NAME_STRING, aName);
  createInfo.props = gContext.mProperties;
  createInfo.size = aSize;
  createInfo.usage = aUsage;

  SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(gContext.mDevice, &createInfo);
  SDL_assert(buffer);

  return buffer;
}

SDL_GPUTexture* CreateTexture(Uint32 aWidth, Uint32 aHeight, SDL_GPUTextureUsageFlags aUsage, SDL_GPUTextureFormat aFormat)
{
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

  SDL_GPUTexture* texture = CreateTexture(surface->w, surface->h, SDL_GPU_TEXTUREUSAGE_SAMPLER, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM);
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
typedef struct QuadPipeline {
  SDL_GPUGraphicsPipeline* mPipeline;
  SDL_GPUTexture* mTexture;
  SDL_GPUSampler* mSampler;
  SDL_FRect mPosition;
} QuadPipeline;

QuadPipeline CreateQuadPipeline() {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "TransformedQuad.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    2,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.vertex_shader);

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "TransformedQuad.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    1,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.fragment_shader);

  SDL_assert(SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "QuadPipeline"));

  QuadPipeline pipeline;
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  pipeline.mTexture = CreateAndUploadTexture(NULL, "sample");

  SDL_GPUSamplerCreateInfo samplerCreateInfo;
  SDL_zero(samplerCreateInfo);
  samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  pipeline.mSampler = SDL_CreateGPUSampler(gContext.mDevice, &samplerCreateInfo);
  SDL_assert(pipeline.mPipeline);

  pipeline.mPosition.x = 128.f;
  pipeline.mPosition.y = 128.f;
  pipeline.mPosition.w = 256.f;
  pipeline.mPosition.h = 256.f;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return pipeline;
}

void DrawQuadPipeline(QuadPipeline* aPipeline, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_BindGPUGraphicsPipeline(aRenderPass, aPipeline->mPipeline);
  SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aPipeline->mPosition, sizeof(aPipeline->mPosition));
  SDL_PushGPUVertexUniformData(aCommandBuffer, 1, &gContext.WorldToNDC, sizeof(gContext.WorldToNDC));

  {
    SDL_GPUTextureSamplerBinding textureBinding;
    SDL_zero(textureBinding);
    textureBinding.texture = aPipeline->mTexture;
    textureBinding.sampler = aPipeline->mSampler;
    SDL_BindGPUFragmentSamplers(aRenderPass, 0, &textureBinding, 1);
  }

  SDL_DrawGPUPrimitives(aRenderPass, 6, 1, 0, 0);
}

void DestroyQuadPipeline(QuadPipeline* aPipeline)
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

  SDL_Window* window = SDL_CreateWindow("SDL GPU Example", 1280, 720, 0);
  SDL_assert(window);

  CreateGpuContext(window);

  QuadPipeline quadPipeline = CreateQuadPipeline();

  const float speed = 200.f;
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
    
    int w = 0, h = 0;
    SDL_GetWindowSizeInPixels(gContext.mWindow, &w, &h);

    gContext.WorldToNDC = OrthographicProjectionLHZO(
      0, w,
      0, h,
      0.0f, 1.0f
    );
      
    if (key_map[SDL_SCANCODE_D]) quadPipeline.mPosition.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_A]) quadPipeline.mPosition.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_W]) quadPipeline.mPosition.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_S]) quadPipeline.mPosition.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_R]) quadPipeline.mPosition.w += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_F]) quadPipeline.mPosition.w -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_T]) quadPipeline.mPosition.h += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_G]) quadPipeline.mPosition.h -= speed * dt * 1.0f;

    SDL_Log("{%f, %f}, {%f, %f}}",
      quadPipeline.mPosition.x,
      quadPipeline.mPosition.y,
      quadPipeline.mPosition.w,
      quadPipeline.mPosition.h
    );

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

    DrawQuadPipeline(&quadPipeline, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyQuadPipeline(&quadPipeline);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}

#ifdef __cplusplus
} // end cpp_test
#endif
