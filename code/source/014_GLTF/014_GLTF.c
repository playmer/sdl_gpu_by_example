#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

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

typedef struct float4x4 {
  union {
    float4 columns[4];
    float data[4][4];
  };
} float4x4;

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
// Scalar Multiplication

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

//////////////////////////////////////////////////////
// Dot Product

float Float2_Dot(float2 aLeft, float2 aRight) {
  return
    (aLeft.x * aRight.x) +
    (aLeft.y * aRight.y);
}

float Float3_Dot(float3 aLeft, float3 aRight) {
  return
    (aLeft.x * aRight.x) +
    (aLeft.y * aRight.y) +
    (aLeft.z * aRight.z);
}

float Float4_Dot(float4 aLeft, float4 aRight) {
  return
    (aLeft.x * aRight.x) +
    (aLeft.y * aRight.y) +
    (aLeft.z * aRight.z) +
    (aLeft.w * aRight.w);
}

//////////////////////////////////////////////////////
// Cross Product

float3 Float3_Cross(float3 aLeft, float3 aRight) {
  float3 toReturn = {
    (aLeft.y * aRight.z) - (aLeft.z * aRight.y),
    (aLeft.z * aRight.x) - (aLeft.x * aRight.z),
    (aLeft.x * aRight.y) - (aLeft.y * aRight.x)
  };

  return toReturn;
}

// Convience function that ignores the 4th component, assuming it was irrelevant.
float3 Float4_Cross(float4 aLeft, float4 aRight) {
  float3 toReturn = {
    (aLeft.y * aRight.z) - (aLeft.z * aRight.y),
    (aLeft.z * aRight.x) - (aLeft.x * aRight.z),
    (aLeft.x * aRight.y) - (aLeft.y * aRight.x)
  };

  return toReturn;
}

//////////////////////////////////////////////////////
// Magnitude

float Float2_Magnitude(float2 aValue) {
  return SDL_sqrt(Float2_Dot(aValue, aValue));
}

float Float3_Magnitude(float3 aValue) {
  return SDL_sqrt(Float3_Dot(aValue, aValue));
}

float Float4_Magnitude(float4 aValue) {
  return SDL_sqrt(Float4_Dot(aValue, aValue));
}

//////////////////////////////////////////////////////
// Normalization

float2 Float2_Normalize(float2 aValue) {
  float magnitude = Float2_Magnitude(aValue);

  float2 toReturn = {
    aValue.x / magnitude,
    aValue.y / magnitude
  };

  return toReturn;
}

float3 Float3_Normalize(float3 aValue) {
  float magnitude = Float3_Magnitude(aValue);

  float3 toReturn = {
    aValue.x / magnitude,
    aValue.y / magnitude,
    aValue.z / magnitude
  };

  return toReturn;
}

float4 Float4_Normalize(float4 aValue) {
  float magnitude = Float4_Magnitude(aValue);

  float4 toReturn = {
    aValue.x / magnitude,
    aValue.y / magnitude,
    aValue.z / magnitude,
    aValue.w / magnitude
  };

  return toReturn;
}

//////////////////////////////////////////////////////
// Matrix Operations

float4 Float4x4_Float4_Multiply(const float4x4* aLeft, const float4 aRight)
{
  float4 toReturn;
  toReturn.x =
    (aLeft->data[0][0] * aRight.x) +
    (aLeft->data[1][0] * aRight.y) +
    (aLeft->data[2][0] * aRight.z) +
    (aLeft->data[3][0] * aRight.w);
  toReturn.y =
    (aLeft->data[0][1] * aRight.x) +
    (aLeft->data[1][1] * aRight.y) +
    (aLeft->data[2][1] * aRight.z) +
    (aLeft->data[3][1] * aRight.w);
  toReturn.z =
    (aLeft->data[0][2] * aRight.x) +
    (aLeft->data[1][2] * aRight.y) +
    (aLeft->data[2][2] * aRight.z) +
    (aLeft->data[3][2] * aRight.w);
  toReturn.w =
    (aLeft->data[0][3] * aRight.x) +
    (aLeft->data[1][3] * aRight.y) +
    (aLeft->data[2][3] * aRight.z) +
    (aLeft->data[3][3] * aRight.w);

  return toReturn;
}

float4x4 Float4x4_Multiply(const float4x4* aLeft, const float4x4* aRight)
{
  float4x4 toReturn;
  SDL_zero(toReturn);

  for (size_t i = 0; i < 4; ++i)
  {
    toReturn.data[i][0] =
      aLeft->data[0][0] * aRight->data[i][0] +
      aLeft->data[1][0] * aRight->data[i][1] +
      aLeft->data[2][0] * aRight->data[i][2] +
      aLeft->data[3][0] * aRight->data[i][3];

    toReturn.data[i][1] =
      aLeft->data[0][1] * aRight->data[i][0] +
      aLeft->data[1][1] * aRight->data[i][1] +
      aLeft->data[2][1] * aRight->data[i][2] +
      aLeft->data[3][1] * aRight->data[i][3];

    toReturn.data[i][2] =
      aLeft->data[0][2] * aRight->data[i][0] +
      aLeft->data[1][2] * aRight->data[i][1] +
      aLeft->data[2][2] * aRight->data[i][2] +
      aLeft->data[3][2] * aRight->data[i][3];

    toReturn.data[i][3] =
      aLeft->data[0][3] * aRight->data[i][0] +
      aLeft->data[1][3] * aRight->data[i][1] +
      aLeft->data[2][3] * aRight->data[i][2] +
      aLeft->data[3][3] * aRight->data[i][3];
  }
  return toReturn;
}


////////////////////////////////////////////////////////////
/// Core Matrices

float4x4 IdentityMatrix() {
  float4x4 toReturn;
  SDL_zero(toReturn);

  toReturn.data[0][0] = 1.0f;
  toReturn.data[1][1] = 1.0f;
  toReturn.data[2][2] = 1.0f;
  toReturn.data[3][3] = 1.0f;

  return toReturn;
}

float4x4 TranslationMatrix(float4 aPosition) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[3][0] = aPosition.x;
  toReturn.data[3][1] = aPosition.y;
  toReturn.data[3][2] = aPosition.z;

  return toReturn;
}

float4x4 ScaleMatrix(float4 aScale) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = aScale.x;
  toReturn.data[1][1] = aScale.y;
  toReturn.data[2][2] = aScale.z;

  return toReturn;
}

float4x4 RotationMatrixX(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[1][1] = SDL_cosf(aAngle);
  toReturn.data[1][2] = SDL_sinf(aAngle);
  toReturn.data[2][1] = -SDL_sinf(aAngle);
  toReturn.data[2][2] = SDL_cosf(aAngle);

  return toReturn;
}

float4x4 RotationMatrixY(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = SDL_cosf(aAngle);
  toReturn.data[0][2] = -SDL_sinf(aAngle);
  toReturn.data[2][0] = SDL_sinf(aAngle);
  toReturn.data[2][2] = SDL_cosf(aAngle);

  return toReturn;
}

float4x4 RotationMatrixZ(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = SDL_cosf(aAngle);
  toReturn.data[0][1] = SDL_sinf(aAngle);
  toReturn.data[1][0] = -SDL_sinf(aAngle);
  toReturn.data[1][1] = SDL_cosf(aAngle);

  return toReturn;
}

float4x4 RotationMatrix(float4 aPosition) {
  float4x4 xRotation = RotationMatrixX(aPosition.x);
  float4x4 yRotation = RotationMatrixY(aPosition.y);
  float4x4 zRotation = RotationMatrixZ(aPosition.z);

  float4x4 xyRotation = Float4x4_Multiply(&yRotation, &xRotation);

  return Float4x4_Multiply(&zRotation, &xyRotation);
}

float4x4 RotationMatrixFromQuaternion(float4 aQuaternion) {
  float x2 = aQuaternion.x * aQuaternion.x;
  float y2 = aQuaternion.y * aQuaternion.y;
  float z2 = aQuaternion.z * aQuaternion.z;

  float xy = aQuaternion.x * aQuaternion.y;
  float xz = aQuaternion.x * aQuaternion.z;
  float yz = aQuaternion.y * aQuaternion.z;

  float wx = aQuaternion.w * aQuaternion.x;
  float wy = aQuaternion.w * aQuaternion.y;
  float wz = aQuaternion.w * aQuaternion.z;

  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = 1.0f - 2.0f * (y2 - z2);
  toReturn.data[0][1] =        2.0f * (xy + wz);
  toReturn.data[0][2] =        2.0f * (xz - wy);

  toReturn.data[1][0] =        2.0f * (xy - wz);
  toReturn.data[1][1] = 1.0f - 2.0f * (x2 - z2);
  toReturn.data[1][2] =        2.0f * (yz + wx);

  toReturn.data[2][0] =        2.0f * (xz + wy);
  toReturn.data[2][1] =        2.0f * (yz - wx);
  toReturn.data[2][2] = 1.0f - 2.0f * (x2 - y2);

  return toReturn;
}

float4x4 CreateModelMatrix(float4 aPosition, float4 aScale, float4 aRotation) {
  float4x4 translation = TranslationMatrix(aPosition);
  float4x4 rotation = RotationMatrix(aRotation);
  float4x4 scale = ScaleMatrix(aScale);

  float4x4 scale_rotation = Float4x4_Multiply(&rotation, &scale);

  return Float4x4_Multiply(&translation, &scale_rotation);
}

float4x4 CreateModelMatrixWithQuaternion(float4 aPosition, float4 aScale, float4 aRotation) {
  float4x4 translation = TranslationMatrix(aPosition);
  float4x4 rotation = RotationMatrixFromQuaternion(aRotation);
  float4x4 scale = ScaleMatrix(aScale);

  float4x4 scale_rotation = Float4x4_Multiply(&rotation, &scale);

  return Float4x4_Multiply(&translation, &scale_rotation);
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

float4x4 PerspectiveProjectionLHZO(float aFovY, float aAspectRatio, float aNear, float aFar) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  const float focalLength = 1.0f / SDL_tan(aFovY * .5f);
  const float k = aFar / (aFar - aNear);

  toReturn.data[0][0] = focalLength / aAspectRatio;
  toReturn.data[1][1] = focalLength;
  toReturn.data[2][2] = k;
  toReturn.data[2][3] = 1.0f;
  toReturn.data[3][2] = -aNear * k;

  return toReturn;
}

float4x4 PerspectiveProjectionLHOZ(float aFovY, float aAspectRatio, float aNear, float aFar) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  const float focalLength = 1.0f / SDL_tan(aFovY * .5f);
  const float k = aNear / (aNear - aFar);

  toReturn.data[0][0] = focalLength / aAspectRatio;
  toReturn.data[1][1] = focalLength;
  toReturn.data[2][2] = k;
  toReturn.data[2][3] = 1.0f;
  toReturn.data[3][2] = -aFar * k;

  return toReturn;
}

float4x4 InfinitePerspectiveProjectionLHOZ(float aFovY, float aAspectRatio, float aNear) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  const float focalLength = 1.0f / SDL_tan(aFovY * .5f);

  // For ease of use we're hardcoding the epsilon to what's recommended in Foundations of Game Engine
  // Development: Rendering, which is 2^(-20).
  const float epsilon = SDL_powf(2, -20);

  toReturn.data[0][0] = focalLength / aAspectRatio;
  toReturn.data[1][1] = focalLength;
  toReturn.data[2][2] = epsilon;
  toReturn.data[2][3] = 1.0f;
  toReturn.data[3][2] = aNear / (1.0f - epsilon);

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

SDL_GPUTextureFormat GetSupportedDepthFormat()
{
  SDL_GPUTextureFormat possibleFormats[] = {
    SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
    SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
    SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
    SDL_GPU_TEXTUREFORMAT_D24_UNORM,
    SDL_GPU_TEXTUREFORMAT_D16_UNORM,
  };

  for (size_t i = 0; i < SDL_arraysize(possibleFormats); ++i) {
    if (SDL_GPUTextureSupportsFormat(gContext.mDevice,
      possibleFormats[i],
      SDL_GPU_TEXTURETYPE_2D,
      SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
    {
      return possibleFormats[i];
    }
  }

  // Didn't find a suitable depth format.
  SDL_assert(false);

  return SDL_GPU_TEXTUREFORMAT_INVALID;
}


SDL_GPUBuffer* CreateAndUploadBuffer(const void* aData, Uint32 aSize, SDL_GPUBufferUsageFlags aUsage)
{
  SDL_GPUBufferCreateInfo bufferCreateInfo;
  SDL_zero(bufferCreateInfo);

  bufferCreateInfo.usage = aUsage;
  bufferCreateInfo.size = aSize;
  bufferCreateInfo.props = gContext.mProperties;

  SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(gContext.mDevice, &bufferCreateInfo);
  SDL_assert(buffer);

  {
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo;
    SDL_zero(transferBufferCreateInfo);
    transferBufferCreateInfo.props = gContext.mProperties;
    transferBufferCreateInfo.size = aSize;
    transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(gContext.mDevice, &transferBufferCreateInfo);
    SDL_assert(transferBuffer);

    void* mappedBuffer = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
    SDL_memcpy(mappedBuffer, aData, aSize);

    SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer);

    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    SDL_assert(commandBuffer);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    SDL_assert(copyPass);

    SDL_GPUTransferBufferLocation source;
    source.offset = 0;
    source.transfer_buffer = transferBuffer;

    SDL_GPUBufferRegion destination;
    destination.buffer = buffer;
    destination.offset = 0;
    destination.size = aSize;

    SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  return buffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGLTF Code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct SceneInfo {
  Uint32 mIndicesCount;
  Uint32 mPositionBytes;
  Uint32 mNormalBytes;
  Uint32 mTangentBytes;
  Uint32 mTexcoordBytes[16];
  Uint32 mTotalNodes;
  Uint32 mRootNodes;
} SceneInfo;


void ProcessNodeInfo(cgltf_node* aNode, SceneInfo* aSceneInfo)
{
  aSceneInfo->mTotalNodes += aNode->children_count;

  for (size_t i = 0; i < aNode->children_count; ++i) {
    ProcessNodeInfo(aNode->children[i], aSceneInfo);
  }

  cgltf_mesh* mesh = aNode->mesh;
  if (mesh == NULL) {
    return;
  }

  for (size_t j = 0; j < mesh->primitives_count; ++j) {
    cgltf_primitive* primitive = &mesh->primitives[j];

    aSceneInfo->mIndicesCount += primitive->indices->count;

    SDL_assert(primitive->indices->type == cgltf_type_scalar);
    SDL_assert(primitive->indices->component_type == cgltf_component_type_r_32u);

    for (size_t k = 0; k < primitive->attributes_count; ++k) {
      cgltf_attribute* attribute = &primitive->attributes[k];

      //primitive->material->

      switch (attribute->type) {
        case cgltf_attribute_type_position: {
          SDL_assert(attribute->data->type == cgltf_type_vec3);
          SDL_assert(attribute->data->component_type == cgltf_component_type_r_32f);
          aSceneInfo->mPositionBytes += attribute->data->count * sizeof(float3);
          break;
        }
        case cgltf_attribute_type_normal: {
          SDL_assert(attribute->data->type == cgltf_type_vec3);
          SDL_assert(attribute->data->component_type == cgltf_component_type_r_32f);
          aSceneInfo->mNormalBytes += attribute->data->count * sizeof(float3);
          break;
        }
        case cgltf_attribute_type_tangent: {
          SDL_assert(attribute->data->type == cgltf_type_vec4);
          SDL_assert(attribute->data->component_type == cgltf_component_type_r_32f);
          aSceneInfo->mTangentBytes += attribute->data->count * sizeof(float4);
          break;
        }
        case cgltf_attribute_type_texcoord: {
          SDL_assert(attribute->data->type == cgltf_type_vec2);
          SDL_assert(attribute->data->component_type == cgltf_component_type_r_32f);

          int texcoordIndex = SDL_atoi(attribute->name + 9);
          aSceneInfo->mTexcoordBytes[texcoordIndex] += attribute->data->count * sizeof(float2);
          break;
        }
        default: break;
      }
    }
  }
}


SceneInfo GetSceneInfo(cgltf_data* aData)
{
  SceneInfo sceneInfo;
  SDL_zero(sceneInfo);

  if (!aData) {
    return sceneInfo;
  }

  if (!aData->scene) {
    return sceneInfo;
  }

  sceneInfo.mRootNodes = (Uint32)aData->scene->nodes_count;

  for (size_t i = 0; i < aData->scene->nodes_count; ++i) {
    sceneInfo.mTotalNodes++;
    ProcessNodeInfo(aData->scene->nodes[i], &sceneInfo);
  }

  return sceneInfo;
}

typedef struct Mesh {
  float4x4 mTransform;

  float4x4 mCurrentTransform; 

  Uint32 mIndicesCount;

  Uint32 mChildrenOffset;
  Uint32 mChildrenCount;

  // Offsets into parent model position/normal/tangent/index buffers
  Uint32 mPositionOffset;
  Uint32 mNormalOffset;
  Uint32 mTangentOffset;
  Uint32 mIndexOffset;

  Uint8 mBaseColorTextureCoordinates;
  Uint8 mMetallicRoughnessTextureCoordinates;

  Uint8 mDiffuseTextureCoordinates;
  Uint8 mSpecularGlossinessTextureCoordinates;

  Uint8 mNormalTextureCoordinates;
  Uint8 mOcclusionTextureCoordinates;
  Uint8 mEmissveTextureCoordinates;
} Mesh;

typedef struct Scene {
  SDL_GPUBuffer* mPositions;
  SDL_GPUBuffer* mNormals;
  SDL_GPUBuffer* mTangents;
  SDL_GPUBuffer* mTexcoords[16];
  SDL_GPUBuffer* mIndices;

  Mesh* mMeshes;
  size_t mRootMeshesCount;
  size_t mMeshesCount;
  //SDL_GPUBuffer* mTextureCoordinates; // float2
} Scene;

void ApplyMeshTransformToChildren(Scene* aScene, Mesh* aMesh)
{
  Mesh* meshChildren = aScene->mMeshes + aMesh->mChildrenOffset;
  for (size_t i = 0; i < aMesh->mChildrenCount; ++i)
  {
    Mesh* childMesh = meshChildren + i;

    float4x4 temp = childMesh->mCurrentTransform;
    childMesh->mCurrentTransform = Float4x4_Multiply(&aMesh->mCurrentTransform, &childMesh->mTransform);
    //childMesh->mCurrentTransform.columns[3].x += aMesh->mCurrentTransform.columns[3].x;
    //childMesh->mCurrentTransform.columns[3].y += aMesh->mCurrentTransform.columns[3].y;
    //childMesh->mCurrentTransform.columns[3].z += aMesh->mCurrentTransform.columns[3].z;
    //childMesh->mCurrentTransform = Float4x4_Multiply(&childMesh->mTransform, &aMesh->mCurrentTransform);
    ApplyMeshTransformToChildren(aScene, childMesh);
  }
}

void RecalculateSceneTransform(Scene* aScene)
{
  for (size_t i = 0; i < aScene->mRootMeshesCount; ++i)
  {
    Mesh* mesh = aScene->mMeshes + i;
    float4x4 temp = mesh->mCurrentTransform;
    mesh->mCurrentTransform = mesh->mTransform;
    ApplyMeshTransformToChildren(aScene, mesh);
  }
}

typedef struct SceneProcessing {
  Uint32 mPositionOffset;
  Uint32 mPositionOffsetSoFar;
  Uint32 mNormalOffset;
  Uint32 mNormalOffsetSoFar;
  Uint32 mTangentOffset;
  Uint32 mTangentOffsetSoFar;
  Uint32 mIndexOffset;
  Uint32 mIndexOffsetSoFar;
  Uint32 mCurrentMeshIndex;
  Uint32 mCurrentChildrenIndex;
} SceneProcessing;

SDL_GPUFilter GltfFilterToSDL(cgltf_filter_type aFilter)
{
  switch (aFilter) {
    case cgltf_filter_type_nearest: return SDL_GPU_FILTER_NEAREST;
    case cgltf_filter_type_linear: return SDL_GPU_FILTER_LINEAR;
    case cgltf_filter_type_nearest_mipmap_nearest: return SDL_GPU_FILTER_NEAREST;
    case cgltf_filter_type_linear_mipmap_nearest: return SDL_GPU_FILTER_LINEAR;
    case cgltf_filter_type_nearest_mipmap_linear: return SDL_GPU_FILTER_NEAREST;
    case cgltf_filter_type_linear_mipmap_linear: return SDL_GPU_FILTER_LINEAR;

    default:
    case cgltf_filter_type_undefined: {
      SDL_Log("Using unknown filter, defaulting to SDL_GPU_FILTER_NEAREST");
      return SDL_GPU_FILTER_NEAREST;
    }
  }
}

SDL_GPUSamplerAddressMode GltfAddressModeToSDL(cgltf_wrap_mode aWrap)
{
  switch (aWrap) {
    case cgltf_wrap_mode_clamp_to_edge: return SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    case cgltf_wrap_mode_mirrored_repeat: return SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
    case cgltf_wrap_mode_repeat: return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;

    default: {
      SDL_Log("Using unknown address mode, defaulting to SDL_GPU_SAMPLERADDRESSMODE_REPEAT");
      return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    }
  }
}

SDL_GPUSampler* CreateSamplerFromGltf(cgltf_sampler* aSampler)
{
  SDL_GPUSamplerCreateInfo samplerCreateInfo;
  SDL_zero(samplerCreateInfo);

  samplerCreateInfo.mag_filter = GltfFilterToSDL(aSampler->mag_filter);
  samplerCreateInfo.min_filter = GltfFilterToSDL(aSampler->min_filter);
  samplerCreateInfo.address_mode_u = GltfAddressModeToSDL(aSampler->wrap_s);
  samplerCreateInfo.address_mode_v = GltfAddressModeToSDL(aSampler->wrap_t);

  return SDL_CreateGPUSampler(gContext.mDevice, &samplerCreateInfo);
}

size_t transferBufferSize = 0;

void GenerateGPUMesh(cgltf_node* aNode, Scene* aScene, SceneProcessing* aSceneProcessing, Mesh* aMesh, Uint8* aTransferPtr)
{
  aMesh->mChildrenOffset = aSceneProcessing->mCurrentChildrenIndex;
  aMesh->mChildrenCount = (Uint32)aNode->children_count;
  Mesh* meshChildren = aScene->mMeshes + aMesh->mChildrenOffset;
  aSceneProcessing->mCurrentChildrenIndex += (Uint32)aNode->children_count;

  for (size_t i = 0; i < aNode->children_count; ++i) {
    GenerateGPUMesh(aNode->children[i], aScene, aSceneProcessing, meshChildren + i, aTransferPtr);
  }

  cgltf_node_transform_local(aNode, (float*)&aMesh->mTransform.data[0]);
  cgltf_node_transform_world(aNode, (float*)&aMesh->mCurrentTransform.data[0]);

  aMesh->mPositionOffset = aSceneProcessing->mPositionOffsetSoFar - aSceneProcessing->mPositionOffset;
  aMesh->mNormalOffset = aSceneProcessing->mNormalOffsetSoFar - aSceneProcessing->mNormalOffset;
  aMesh->mTangentOffset = aSceneProcessing->mTangentOffsetSoFar - aSceneProcessing->mTangentOffset;
  aMesh->mIndexOffset = aSceneProcessing->mIndexOffsetSoFar - aSceneProcessing->mIndexOffset;

  cgltf_mesh* mesh_file = aNode->mesh;
  if (mesh_file == NULL) {
    return;
  }

  for (size_t j = 0; j < mesh_file->primitives_count; ++j) {
    cgltf_primitive* primitive = &mesh_file->primitives[j];

    aMesh->mIndicesCount = (Uint32)primitive->indices->count;
    cgltf_accessor_unpack_indices(primitive->indices, (void*)(aTransferPtr + aSceneProcessing->mIndexOffsetSoFar), sizeof(Uint32), primitive->indices->count);
    aSceneProcessing->mIndexOffsetSoFar += (Uint32)primitive->indices->count * sizeof(Uint32);

    SDL_assert(aSceneProcessing->mIndexOffsetSoFar <= transferBufferSize);

    for (size_t k = 0; k < primitive->attributes_count; ++k) {
      Uint32* attributeCount = NULL;
      cgltf_attribute* attribute = &primitive->attributes[k];
      switch (attribute->type) {
        case cgltf_attribute_type_position: attributeCount = &aSceneProcessing->mPositionOffsetSoFar; break;
        case cgltf_attribute_type_normal: attributeCount = &aSceneProcessing->mNormalOffsetSoFar; break;
        case cgltf_attribute_type_tangent: attributeCount = &aSceneProcessing->mTangentOffsetSoFar; break;
        default: continue;
      }

      if (!attributeCount) {
        continue;
      }

      *attributeCount += cgltf_accessor_unpack_floats(
        attribute->data,
        (cgltf_float*)(aTransferPtr + *attributeCount),
        attribute->data->count * cgltf_num_components(attribute->data->type)
      ) * sizeof(float);

      SDL_assert(*attributeCount < transferBufferSize);
    }
  }
}

Scene GenerateGPUScene(cgltf_data* aData, SceneInfo aSceneInfo)
{
  Scene scene;
  SDL_zero(scene);

  Uint32 indexBytes = aSceneInfo.mIndicesCount * sizeof(Uint32);
  scene.mPositions = CreateGPUBuffer(aSceneInfo.mPositionBytes, SDL_GPU_BUFFERUSAGE_VERTEX, "Positions");
  scene.mNormals = CreateGPUBuffer(aSceneInfo.mNormalBytes, SDL_GPU_BUFFERUSAGE_VERTEX, "Normals");
  scene.mTangents = CreateGPUBuffer(aSceneInfo.mTangentBytes, SDL_GPU_BUFFERUSAGE_VERTEX, "Tangents");

  for (size_t i = 0; i < 16; ++i) {
    if (aSceneInfo.mTexcoordBytes[i] == 0) {
      continue;
    }

    scene.mTexcoords[i] = CreateGPUBuffer(aSceneInfo.mTexcoordBytes[i], SDL_GPU_BUFFERUSAGE_VERTEX, "Tangents");
  }

  scene.mIndices = CreateGPUBuffer(indexBytes, SDL_GPU_BUFFERUSAGE_INDEX, "Indices");

  SDL_GPUTransferBuffer* transferBuffer = NULL;

  {
    SDL_GPUTransferBufferCreateInfo transferCreateInfo;
    SDL_zero(transferCreateInfo);
    SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, "ModelTransferBuffer");
    transferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferCreateInfo.size = aSceneInfo.mPositionBytes + aSceneInfo.mNormalBytes + aSceneInfo.mTangentBytes + indexBytes;
    transferCreateInfo.props = gContext.mProperties;

    transferBufferSize = transferCreateInfo.size;

    transferBuffer = SDL_CreateGPUTransferBuffer(gContext.mDevice, &transferCreateInfo);
  }
  SceneProcessing processing;
  {
    SDL_zero(processing);
    processing.mPositionOffsetSoFar = processing.mPositionOffset = 0;
    processing.mNormalOffsetSoFar = processing.mNormalOffset = aSceneInfo.mPositionBytes;
    processing.mTangentOffsetSoFar = processing.mTangentOffset = processing.mNormalOffset + aSceneInfo.mNormalBytes;
    processing.mIndexOffsetSoFar = processing.mIndexOffset = processing.mTangentOffset + aSceneInfo.mTangentBytes;
  }

  // Copy all of the scene data into the transfer buffer, generate Mesh hierarchy.
  {
    Uint8* transferPtr = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);

    scene.mMeshesCount = aSceneInfo.mTotalNodes;
    scene.mMeshes = SDL_calloc(scene.mMeshesCount, sizeof(Mesh));

    scene.mRootMeshesCount = aSceneInfo.mRootNodes;

    processing.mCurrentChildrenIndex += scene.mRootMeshesCount;

    for (size_t i = 0; i < aData->scene->nodes_count; ++i) {
      GenerateGPUMesh(aData->scene->nodes[i], &scene, &processing, scene.mMeshes + i, transferPtr);
    }

    SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer);
  }

  // Upload to the appropriate buffers
  {
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    SDL_assert(commandBuffer);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    SDL_assert(copyPass);

    SDL_GPUTransferBufferLocation source;
    source.offset = 0;
    source.transfer_buffer = transferBuffer;

    SDL_GPUBufferRegion destination;
    destination.offset = 0;

    // Positions
    {
      source.offset = processing.mPositionOffset;
      destination.buffer = scene.mPositions;
      destination.size = aSceneInfo.mPositionBytes;

      SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    }

    // Normals
    {
      source.offset = processing.mNormalOffset;

      destination.buffer = scene.mNormals;
      destination.size = aSceneInfo.mNormalBytes;

      SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    }

    // Tangents
    {
      source.offset = processing.mTangentOffset;

      destination.buffer = scene.mTangents;
      destination.size = aSceneInfo.mTangentBytes;

      SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    }

    // Indices
    {
      source.offset = processing.mIndexOffset;

      destination.buffer = scene.mIndices;
      destination.size = indexBytes;

      SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    }

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    SDL_ReleaseGPUTransferBuffer(gContext.mDevice, transferBuffer);
  }

  RecalculateSceneTransform(&scene);

  return scene;
}




Scene LoadGltfModel(const char* aModelName) {
  char model_path[4096];
  SDL_snprintf(model_path, SDL_arraysize(model_path), "Assets/Models/%s", aModelName);

  cgltf_options options;
  SDL_zero(options);

  cgltf_data* data = NULL;
  cgltf_result result = cgltf_parse_file(&options, model_path, &data);
  SDL_assert(result == cgltf_result_success);

  result = cgltf_load_buffers(&options, data, model_path);
  SDL_assert(result == cgltf_result_success);

  SDL_Log("Model: %s", model_path);

  SceneInfo sceneInfo = GetSceneInfo(data);
  return GenerateGPUScene(data, sceneInfo);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Technique Code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ModelUbo {
  float4 mPosition;
  float4 mScale;
  float4 mRotation;
} ModelUbo;

typedef struct ModelContext {
  SDL_GPUGraphicsPipeline* mPipeline;
  SDL_GPUTexture* mTexture;
  SDL_GPUSampler* mSampler;
  ModelUbo mUbo[2];
  Scene mModel;
} ModelContext;

ModelContext CreateModelContext(SDL_GPUTextureFormat aDepthFormat) {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;                                  /*setting num_color_targets */
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;    /*setting color_target_descriptions */
  graphicsPipelineCreateInfo.target_info.depth_stencil_format = aDepthFormat;                    /*setting depth_stencil_format */
  graphicsPipelineCreateInfo.target_info.has_depth_stencil_target = true;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  graphicsPipelineCreateInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
  //graphicsPipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;
  graphicsPipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;


  SDL_GPUVertexAttribute attributes[3];

  // Position
  attributes[0].location = 0;
  attributes[0].buffer_slot = 0;
  attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  attributes[0].offset = 0;

  // Normal
  attributes[1].location = 1;
  attributes[1].buffer_slot = 1;
  attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  attributes[1].offset = 0;

  // Tangent
  attributes[2].location = 2;
  attributes[2].buffer_slot = 2;
  attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  attributes[2].offset = 0;

  graphicsPipelineCreateInfo.vertex_input_state.vertex_attributes = attributes;
  graphicsPipelineCreateInfo.vertex_input_state.num_vertex_attributes = SDL_arraysize(attributes);

  SDL_GPUVertexBufferDescription bufferDescription[3];
  bufferDescription[0].slot = 0;
  bufferDescription[0].pitch = sizeof(float3);
  bufferDescription[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  bufferDescription[1].instance_step_rate = 0;
  bufferDescription[1].slot = 1;
  bufferDescription[1].pitch = sizeof(float3);
  bufferDescription[1].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  bufferDescription[0].instance_step_rate = 0;
  bufferDescription[2].slot = 2;
  bufferDescription[2].pitch = sizeof(float4);
  bufferDescription[2].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  bufferDescription[2].instance_step_rate = 0;

  graphicsPipelineCreateInfo.vertex_input_state.vertex_buffer_descriptions = bufferDescription;
  graphicsPipelineCreateInfo.vertex_input_state.num_vertex_buffers = SDL_arraysize(bufferDescription);

  // Remember to come back to this later in the tutorial, don't show it off immediately.
  graphicsPipelineCreateInfo.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;

  graphicsPipelineCreateInfo.depth_stencil_state.enable_depth_test = true;
  graphicsPipelineCreateInfo.depth_stencil_state.enable_depth_write = true;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "VertexAndIndexBuffer.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    2,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.vertex_shader);

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "VertexAndIndexBuffer.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    1,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.fragment_shader);

  SDL_assert(SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, "ModelContext"));

  ModelContext context;

  // Broke the name so that we don't waste time zipping it while the example isn't done.
  context.mModel = LoadGltfModel("buster_drone.glb");

  context.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  context.mTexture = CreateAndUploadTexture(NULL, "sample");

  SDL_GPUSamplerCreateInfo samplerCreateInfo;
  SDL_zero(samplerCreateInfo);
  samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
  context.mSampler = SDL_CreateGPUSampler(gContext.mDevice, &samplerCreateInfo);
  SDL_assert(context.mPipeline);

  context.mUbo[0].mPosition.x = 0.f;
  context.mUbo[0].mPosition.y = -1.f;
  context.mUbo[0].mPosition.z = 5.f;
  context.mUbo[0].mPosition.w = 1.f;
  context.mUbo[0].mScale.x = 1.0f;
  context.mUbo[0].mScale.y = 1.0f;
  context.mUbo[0].mScale.z = 1.0f;
  context.mUbo[0].mScale.w = 1.0f;
  context.mUbo[0].mRotation.x = 0.f;
  context.mUbo[0].mRotation.y = 0.f;
  context.mUbo[0].mRotation.z = 0.f;
  context.mUbo[0].mRotation.w = 0.f;

  context.mUbo[1].mPosition.x = 0.f;
  context.mUbo[1].mPosition.y = -1.f;
  context.mUbo[1].mPosition.z = 10.f;
  context.mUbo[1].mPosition.w = 0.f;
  context.mUbo[1].mScale.x = 2.f;
  context.mUbo[1].mScale.y = 2.f;
  context.mUbo[1].mScale.z = 2.f;
  context.mUbo[1].mScale.w = 2.f;
  context.mUbo[1].mRotation.x = 0.f;
  context.mUbo[1].mRotation.y = 0.f;
  context.mUbo[1].mRotation.z = 0.f;
  context.mUbo[1].mRotation.w = 0.f;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return context;
}

void DrawModelContext(ModelContext* aContext, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_BindGPUGraphicsPipeline(aRenderPass, aContext->mPipeline);

  float4x4 model = CreateModelMatrix(aContext->mUbo[0].mPosition, aContext->mUbo[0].mScale, aContext->mUbo[0].mRotation);
  SDL_PushGPUVertexUniformData(aCommandBuffer, 1, &gContext.WorldToNDC, sizeof(gContext.WorldToNDC));

  for (size_t i = 0; i < aContext->mModel.mMeshesCount; ++i) {
    Mesh* mesh = aContext->mModel.mMeshes + i;

    // I should preprocess this list and iterate that instead so I don't have to do this.
    if (mesh->mIndicesCount == 0) {
      continue;
    }

    {
      SDL_GPUBufferBinding binding[3];
      binding[0].buffer = aContext->mModel.mPositions;
      binding[0].offset = mesh->mPositionOffset;
      binding[1].buffer = aContext->mModel.mNormals;
      binding[1].offset = mesh->mNormalOffset;
      binding[2].buffer = aContext->mModel.mTangents;
      binding[2].offset = mesh->mTangentOffset;
      SDL_BindGPUVertexBuffers(aRenderPass, 0, binding, SDL_arraysize(binding));
    
    }
    
    {
      SDL_GPUBufferBinding binding;
      binding.buffer = aContext->mModel.mIndices;
      binding.offset = mesh->mIndexOffset;
      SDL_BindGPUIndexBuffer(aRenderPass, &binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    }

    {
      SDL_GPUTextureSamplerBinding textureBinding;
      SDL_zero(textureBinding);
      textureBinding.texture = aContext->mTexture;
      textureBinding.sampler = aContext->mSampler;
      SDL_BindGPUFragmentSamplers(aRenderPass, 0, &textureBinding, 1);
    }

    //float4x4 meshMatrix = Float4x4_Multiply(&mesh->mCurrentTransform, &model);
    float4x4 meshMatrix = Float4x4_Multiply(&model, &mesh->mCurrentTransform);
    //float4x4 meshMatrix = model;

    SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &meshMatrix, sizeof(meshMatrix));

    SDL_DrawGPUIndexedPrimitives(aRenderPass, mesh->mIndicesCount, 1, 0, 0, 0);
  }
}

void DestroyModelContext(ModelContext* aContext)
{
  SDL_ReleaseGPUBuffer(gContext.mDevice, aContext->mModel.mPositions);
  SDL_ReleaseGPUBuffer(gContext.mDevice, aContext->mModel.mNormals);
  SDL_ReleaseGPUBuffer(gContext.mDevice, aContext->mModel.mTangents);
  SDL_ReleaseGPUBuffer(gContext.mDevice, aContext->mModel.mIndices);

  SDL_ReleaseGPUTexture(gContext.mDevice, aContext->mTexture);
  SDL_ReleaseGPUSampler(gContext.mDevice, aContext->mSampler);
  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aContext->mPipeline);

  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aContext->mPipeline);
  SDL_zero(*aContext);
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

  SDL_GPUTexture* depthTexture = NULL;
  Uint32 depthWidth = 0;
  Uint32 depthHeight = 0;
  SDL_GPUTextureFormat depthFormat = GetSupportedDepthFormat();

  ModelContext cubeContext = CreateModelContext(depthFormat);

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

    gContext.WorldToNDC = InfinitePerspectiveProjectionLHOZ(
      45.0f * SDL_PI_F / 180.0f,
      (float)w / (float)h,
      0.1f
    );

    if (key_map[SDL_SCANCODE_D])        cubeContext.mUbo[0].mPosition.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_A])        cubeContext.mUbo[0].mPosition.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_W])        cubeContext.mUbo[0].mPosition.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_S])        cubeContext.mUbo[0].mPosition.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_E])        cubeContext.mUbo[0].mPosition.z += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_Q])        cubeContext.mUbo[0].mPosition.z -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_R])        cubeContext.mUbo[0].mScale.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_F])        cubeContext.mUbo[0].mScale.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_T])        cubeContext.mUbo[0].mScale.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_G])        cubeContext.mUbo[0].mScale.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_INSERT])   cubeContext.mUbo[0].mRotation.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_DELETE])   cubeContext.mUbo[0].mRotation.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_HOME])     cubeContext.mUbo[0].mRotation.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_END])      cubeContext.mUbo[0].mRotation.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_PAGEUP])   cubeContext.mUbo[0].mRotation.z += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_PAGEDOWN]) cubeContext.mUbo[0].mRotation.z -= speed * dt * 1.0f;

    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    if (!commandBuffer)
    {
      SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
      continue;
    }

    SDL_GPUTexture* swapchainTexture;
    Uint32 swapchainWidth = 0;
    Uint32 swapchainHeight = 0;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, gContext.mWindow, &swapchainTexture, &swapchainWidth, &swapchainHeight))
    {
      SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
      continue;
    }

    if (depthWidth != swapchainWidth || depthHeight != swapchainHeight)
    {
      if (depthTexture) {
        SDL_ReleaseGPUTexture(gContext.mDevice, depthTexture);
      }

      depthTexture = CreateTexture(swapchainWidth, swapchainHeight, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET, depthFormat, "DepthTexture");
      SDL_assert(depthTexture);

      depthWidth = swapchainWidth;
      depthHeight = swapchainHeight;
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


    // Remember to come back to this later in the tutorial, don't show it off immediately.
    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo;
    SDL_zero(depthStencilTargetInfo);

    depthStencilTargetInfo.texture = depthTexture;
    depthStencilTargetInfo.clear_depth = 0.f;
    depthStencilTargetInfo.clear_stencil = 0.f;
    depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
    depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthStencilTargetInfo.cycle = true; // NOTE: Introduce cycling

    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
      commandBuffer,
      &colorTargetInfo,
      1,
      &depthStencilTargetInfo
    );

    DrawModelContext(&cubeContext, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  SDL_ReleaseGPUTexture(gContext.mDevice, depthTexture);

  DestroyModelContext(&cubeContext);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}
