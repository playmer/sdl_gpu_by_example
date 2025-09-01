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

  
typedef struct float4 {
  float x,y,z,w;
} float4;

typedef struct float4x4 {
  union {
    float4 columns[4];
    float mat[4][4];
  };
} float4x4;

float4x4 OrthographicProjectionLHZO(float aLeft, float aRight, float aBottom, float aTop, float aNear, float aFar) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  toReturn.mat[0][0] = 2.0f / (aRight - aLeft);
  toReturn.mat[1][1] = 2.0f / (aTop - aBottom);
  toReturn.mat[2][2] = 1.0f / (aFar - aNear);

  toReturn.mat[3][0] = - (aRight + aLeft) / (aRight - aLeft);
  toReturn.mat[3][1] = - (aTop + aBottom) / (aTop - aBottom);
  toReturn.mat[3][2] = - aNear / (aFar - aNear);

  toReturn.mat[3][3] = 1.0f;

  return toReturn;
}

float4x4 PerspectiveProjectionLHZO(float aFov, float aAspectRatio, float aNear, float aFar) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  const float tanHalfFovy = SDL_tan(aFov / 2.0f);

  toReturn.mat[0][0] = 1.0f / (aAspectRatio * tanHalfFovy);
  toReturn.mat[1][1] = 1.0f / (tanHalfFovy);
  toReturn.mat[2][2] = aFar / (aFar - aNear);
  toReturn.mat[2][3] = 1.0f;
  toReturn.mat[3][2] = -(aFar * aNear) / (aFar - aNear);

  return toReturn;
}


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

SDL_GPUBuffer* CreateGPUBuffer(Uint32 aSize, SDL_GPUBufferUsageFlags aUsage, const char* aName)
{
  SDL_GPUBufferCreateInfo createInfo;

  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_BUFFER_CREATE_NAME_STRING, aName);
  createInfo.props = gContext.mProperties;
  createInfo.size = aSize;
  createInfo.usage = aUsage;

  SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(gContext.mDevice, &createInfo);
  sdl_check(buffer, "Couldn't create GPU buffer (%s)", aName);

  return buffer;
}

SDL_GPUTexture* CreateAndUploadTexture(SDL_GPUCopyPass* aCopyPass, const char* aTextureName) {
  char texture_path[4096];
  sprintf(texture_path, "Assets/Images/%s.bmp", aTextureName);
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
  uint8_t *transferPtr = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
  memcpy(transferPtr, surface->pixels, transferCreateInfo.size);
  SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer);

  SDL_GPUCommandBuffer* commandBuffer = NULL;
  SDL_GPUCopyPass* copyPass = aCopyPass;
  bool needsToSubmit = NULL == copyPass;
  if (needsToSubmit) {
    commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    copyPass = SDL_BeginGPUCopyPass(commandBuffer);
  }

  SDL_GPUTextureCreateInfo textureCreateInfo;
  SDL_zero(textureCreateInfo);
  textureCreateInfo.width = surface->w;
  textureCreateInfo.height = surface->h;
  textureCreateInfo.layer_count_or_depth = 1;
  textureCreateInfo.num_levels = 1;
  textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
  textureCreateInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
  SDL_GPUTexture* texture = SDL_CreateGPUTexture(gContext.mDevice, &textureCreateInfo);

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

typedef struct CubePipeline {
  SDL_GPUGraphicsPipeline* mPipeline;
  SDL_GPUTexture* mTexture;
  SDL_GPUSampler* mSampler;
  float mPosition[4];
  float mScale[4];
} CubePipeline;

CubePipeline CreateCubePipeline() {
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
  //graphicsPipelineCreateInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;
  graphicsPipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "Cube.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    2,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.vertex_shader, "Failed to create the Triangle vertex shader: ");

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "Cube.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    1,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  sdl_check(graphicsPipelineCreateInfo.fragment_shader, "Failed to create the Triangle fragment shader: ");

  sdl_check(
    SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "CubePipeline"),
    "While creating the CubePipeline, there was an error setting the GPU Shader Name property: "
  );

  CubePipeline pipeline;
  pipeline.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  pipeline.mTexture = CreateAndUploadTexture(NULL, "sample");

  SDL_GPUSamplerCreateInfo samplerCreateInfo;
  SDL_zero(samplerCreateInfo);
  samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  pipeline.mSampler = SDL_CreateGPUSampler(gContext.mDevice, &samplerCreateInfo);
  sdl_check(pipeline.mPipeline, "Failed to create the GPU Pipeline: ");

  pipeline.mPosition[0] = 0.f;
  pipeline.mPosition[1] = 0.f;
  pipeline.mPosition[2] = 0.f;
  pipeline.mPosition[3] = 0.f;
  pipeline.mScale[0] = 1.f;
  pipeline.mScale[1] = 1.f;
  pipeline.mScale[2] = 1.f;
  pipeline.mScale[3] = 1.f;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return pipeline;
}

void DrawCubePipeline(CubePipeline* aPipeline, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_BindGPUGraphicsPipeline(aRenderPass, aPipeline->mPipeline);
  SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aPipeline->mPosition, sizeof(aPipeline->mPosition) + sizeof(aPipeline->mScale));
  SDL_PushGPUVertexUniformData(aCommandBuffer, 1, &gContext.WorldToNDC, sizeof(gContext.WorldToNDC));

  {
    SDL_GPUTextureSamplerBinding textureBinding;
    SDL_zero(textureBinding);
    textureBinding.texture = aPipeline->mTexture;
    textureBinding.sampler = aPipeline->mSampler;
    SDL_BindGPUFragmentSamplers(aRenderPass, 0, &textureBinding, 1);
  }

  SDL_DrawGPUPrimitives(aRenderPass, 6 /* 6 per face */ * 6 /* 6 sides of our cube */, 1, 0, 0);
}

void DestroyCubePipeline(CubePipeline* aPipeline)
{
  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aPipeline->mPipeline);
  SDL_zero(*aPipeline);
}



int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  sdl_check(SDL_Init(SDL_INIT_VIDEO), "Couldn't initialize SDL: ");

  SDL_Window* window = SDL_CreateWindow("003-Triangle_and_Fullscreen_Triangle", 1280, 720, 0);
  sdl_check(window, "Couldn't create a window: ");

  CreateGpuContext(window);

  CubePipeline cubePipeline = CreateCubePipeline();

  const float speed = 5.f;
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

    gContext.WorldToNDC = PerspectiveProjectionLHZO(
      45.0f * SDL_PI_F / 180.0f,
      (float)w / (float)h,
      0.0f, 1.0f
    );
      
    if (key_map[SDL_SCANCODE_D]) cubePipeline.mPosition[0] += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_A]) cubePipeline.mPosition[0] -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_W]) cubePipeline.mPosition[1] += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_S]) cubePipeline.mPosition[1] -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_E]) cubePipeline.mPosition[2] += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_Q]) cubePipeline.mPosition[2] -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_R]) cubePipeline.mScale[0] += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_F]) cubePipeline.mScale[0] -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_T]) cubePipeline.mScale[0] += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_G]) cubePipeline.mScale[0] -= speed * dt * 1.0f;

    SDL_Log("{%f, %f}, {%f, %f}, {%f, %f}}",
      cubePipeline.mPosition[0],
      cubePipeline.mPosition[0],
      cubePipeline.mPosition[1],
      cubePipeline.mPosition[1],
      cubePipeline.mPosition[2],
      cubePipeline.mPosition[2]
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

    DrawCubePipeline(&cubePipeline, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  DestroyCubePipeline(&cubePipeline);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}
