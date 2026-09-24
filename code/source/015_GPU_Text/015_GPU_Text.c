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

#pragma warning(push, 1)
#include "stb_truetype.h"
#pragma warning(pop)

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

typedef struct Transform {
  float4 mPosition;
  float4 mScale;
  float4 mRotation;
} Transform;

typedef struct Orientation {
  float3 mForward;
  float3 mRight;
  float3 mUp;
} Orientation;

Transform GetDefaultTransform()
{
  Transform toReturn;

  toReturn.mPosition.x = 0.f;
  toReturn.mPosition.y = 0.f;
  toReturn.mPosition.z = 0.f;
  toReturn.mPosition.w = 0.f;
  toReturn.mScale.x = 1.f;
  toReturn.mScale.y = 1.f;
  toReturn.mScale.z = 1.f;
  toReturn.mScale.w = 1.f;
  toReturn.mRotation.x = 0.f;
  toReturn.mRotation.y = 0.f;
  toReturn.mRotation.z = 0.f;
  toReturn.mRotation.w = 0.f;

  return toReturn;
}

float4 Float4_From3(float3 aFloat3, float aW) {
  float4 toReturn = {
    aFloat3.x, aFloat3.y, aFloat3.z, aW,
  };

  return toReturn;
}

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

float4x4 Float4x4_Inverse(const float4x4* aValue)
{
  const float3 a = Float4_XYZ(aValue->columns[0]);
  const float3 b = Float4_XYZ(aValue->columns[1]);
  const float3 c = Float4_XYZ(aValue->columns[2]);
  const float3 d = Float4_XYZ(aValue->columns[3]);

  const float x = aValue->data[0][3];
  const float y = aValue->data[1][3];
  const float z = aValue->data[2][3];
  const float w = aValue->data[3][3];

  const float3 s = Float3_Cross(a, b);
  const float3 t = Float3_Cross(c, d);
  const float3 u = Float3_Add(Float3_Scalar_Multiply(a, y), Float3_Scalar_Multiply(b, x));
  const float3 v = Float3_Subtract(Float3_Scalar_Multiply(c, w), Float3_Scalar_Multiply(d, z));

  const float determinant_inverse = 1.0f / (Float3_Dot(s, v) + Float3_Dot(t, u));

  const float3 s_prime = Float3_Scalar_Multiply(s, determinant_inverse);
  const float3 t_prime = Float3_Scalar_Multiply(t, determinant_inverse);
  const float3 u_prime = Float3_Scalar_Multiply(u, determinant_inverse);
  const float3 v_prime = Float3_Scalar_Multiply(v, determinant_inverse);

  const float3 row0 =      Float3_Add(Float3_Cross(      b, v_prime), Float3_Scalar_Multiply(t_prime, y));
  const float3 row1 = Float3_Subtract(Float3_Cross(v_prime,       a), Float3_Scalar_Multiply(t_prime, x));
  const float3 row2 =      Float3_Add(Float3_Cross(      d, u_prime), Float3_Scalar_Multiply(s_prime, w));
  const float3 row3 = Float3_Subtract(Float3_Cross(u_prime,       c), Float3_Scalar_Multiply(s_prime, z));

  float4x4 toReturn;
  toReturn.data[0][0] = row0.x;
  toReturn.data[0][1] = row1.x;
  toReturn.data[0][2] = row2.x;
  toReturn.data[0][3] = row3.x;

  toReturn.data[1][0] = row0.y;
  toReturn.data[1][1] = row1.y;
  toReturn.data[1][2] = row2.y;
  toReturn.data[1][3] = row3.y;

  toReturn.data[2][0] = row0.z;
  toReturn.data[2][1] = row1.z;
  toReturn.data[2][2] = row2.z;
  toReturn.data[2][3] = row3.z;

  toReturn.data[3][0] = -Float3_Dot(b, t_prime);
  toReturn.data[3][1] =  Float3_Dot(a, t_prime);;
  toReturn.data[3][2] = -Float3_Dot(d, s_prime);;
  toReturn.data[3][3] =  Float3_Dot(c, s_prime);;

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

float4x4 CreateModelMatrix(float4 aPosition, float4 aScale, float4 aRotation) {
  float4x4 translation = TranslationMatrix(aPosition);
  float4x4 rotation = RotationMatrix(aRotation);
  float4x4 scale = ScaleMatrix(aScale);

  float4x4 scale_rotation = Float4x4_Multiply(&rotation, &scale);

  return Float4x4_Multiply(&translation, &scale_rotation);
}

float4x4 CreateModelMatrixFromTransform(const Transform* aTransform) {
  return CreateModelMatrix(aTransform->mPosition, aTransform->mScale, aTransform->mRotation);
}

Orientation GetOrientation(const Transform* aTransform) {
  float4 forward = {
    0.f, 0.f, 1.0f, 1.0f
  };

  float4 right = {
    1.f, 0.f, 0.0f, 1.0f
  };

  float4 up = {
    0.f, 1.f, 0.0f, 1.0f
  };

  float4x4 rotation = RotationMatrix(aTransform->mRotation);

  Orientation toReturn = {
    Float4_XYZ(Float4x4_Float4_Multiply(&rotation, forward)),
    Float4_XYZ(Float4x4_Float4_Multiply(&rotation, right)),
    Float4_XYZ(Float4x4_Float4_Multiply(&rotation, up))
  };

  return toReturn;
}

////////////////////////////////////////////////////////////
/// Views

float4x4 LookAtLH(float3 aEye, float3 aCenter, float3 aUp) {
  float4x4 toReturn;
  SDL_zero(toReturn);

  float3 forward = Float3_Normalize(Float3_Subtract(aEye, aCenter));

  //toReturn.data[0][0] = 1.0f / (aAspectRatio * tanHalfFovy);
  //toReturn.data[1][1] = 1.0f / (tanHalfFovy);
  //toReturn.data[2][2] = aFar / (aFar - aNear);
  //toReturn.data[2][3] = 1.0f;
  //toReturn.data[3][2] = -(aFar * aNear) / (aFar - aNear);

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
  toReturn.data[3][2] = aNear/(1.0f - epsilon);

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

SDL_GPUTransferBuffer* CreateTransferBuffer(Uint32 aSize, SDL_GPUTransferBufferUsage aUsage, const char* aName)
{
  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_TRANSFERBUFFER_CREATE_NAME_STRING, aName);

  SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo;
  SDL_zero(transferBufferCreateInfo);
  transferBufferCreateInfo.props = gContext.mProperties;
  transferBufferCreateInfo.size = aSize;
  transferBufferCreateInfo.usage = aUsage;

  SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(gContext.mDevice, &transferBufferCreateInfo);
  SDL_assert(transferBuffer);

  return transferBuffer;
}

SDL_GPUTexture* CreateTexture(Uint32 aWidth, Uint32 aHeight, Uint32 layers_or_depth, Uint32 levels, SDL_GPUTextureUsageFlags aUsage, SDL_GPUTextureFormat aFormat, const char* aName)
{
  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, aName);

  SDL_GPUTextureCreateInfo textureCreateInfo;
  SDL_zero(textureCreateInfo);
  textureCreateInfo.width = aWidth;
  textureCreateInfo.height = aHeight;
  textureCreateInfo.layer_count_or_depth = layers_or_depth;
  textureCreateInfo.num_levels = levels;
  textureCreateInfo.usage = aUsage;
  textureCreateInfo.format = aFormat;
  textureCreateInfo.props = gContext.mProperties;
  return SDL_CreateGPUTexture(gContext.mDevice, &textureCreateInfo);
}

SDL_GPUTexture* CreateAndUploadTexture(SDL_GPUCopyPass* aCopyPass, const char* aTextureName) {
  char stringBuffer[4096];
  SDL_snprintf(stringBuffer, SDL_arraysize(stringBuffer), "Assets/Images/%s", aTextureName);
  SDL_Surface* surface = SDL_LoadSurface(stringBuffer);
  if (surface->format != SDL_PIXELFORMAT_RGBA32)
  {
    SDL_Surface* temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    surface = temp;
  }

  Uint32 textureSize = surface->h * surface->pitch;

  SDL_snprintf(stringBuffer, SDL_arraysize(stringBuffer), "CreateAndUploadTexture Transfer Buffer for %s", aTextureName);

  SDL_GPUTransferBuffer* transferBuffer = CreateTransferBuffer(textureSize, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, stringBuffer);

  void* transferPtr = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
  memcpy(transferPtr, surface->pixels, textureSize);
  SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer);

  SDL_GPUCommandBuffer* commandBuffer = NULL;
  SDL_GPUCopyPass* copyPass = aCopyPass;
  bool needsToSubmit = NULL == copyPass;
  if (needsToSubmit) {
    commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
    copyPass = SDL_BeginGPUCopyPass(commandBuffer);
  }

  SDL_GPUTexture* texture = CreateTexture(surface->w, surface->h, 1, 1, SDL_GPU_TEXTUREUSAGE_SAMPLER, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, aTextureName);
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

SDL_GPUBuffer* CreateAndUploadBuffer(const void* aData, Uint32 aSize, SDL_GPUBufferUsageFlags aUsage, const char* aName)
{
  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_BUFFER_CREATE_NAME_STRING, aName);

  SDL_GPUBufferCreateInfo bufferCreateInfo;
  SDL_zero(bufferCreateInfo);

  bufferCreateInfo.usage = aUsage;
  bufferCreateInfo.size = aSize;
  bufferCreateInfo.props = gContext.mProperties;

  SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(gContext.mDevice, &bufferCreateInfo);
  SDL_assert(buffer);

  {
    char tranfer_buffer_name[4096];
    SDL_snprintf(tranfer_buffer_name, SDL_arraysize(tranfer_buffer_name), "CreateAndUploadBuffer Transfer Buffer for %s", aName);

    SDL_GPUTransferBuffer* transferBuffer = CreateTransferBuffer(aSize, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, tranfer_buffer_name);

    {
      void* mappedBuffer = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
      SDL_memcpy(mappedBuffer, aData, aSize);
      SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer);
    }

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
    SDL_ReleaseGPUTransferBuffer(gContext.mDevice, transferBuffer);
  }

  return buffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Slug
//
// Written _heavily_ referencing the following sources:
//
// https://github.com/xpjb/Slug
// https://github.com/mightycow/Sluggish/
//
// and of course:
// https://github.com/EricLengyel/Slug
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Slug_QuadraticCurve {
  float2 p1;
  float2 p2;
  float2 p3;
} Slug_QuadraticCurve;


typedef struct Slug_CubicCurve {
  float2 p1;
  float2 p2;
  float2 p3;
  float2 p4;
} Slug_CubicCurve;



typedef struct CubicToQuadraticCurveResult
{
  Slug_QuadraticCurve result[2];
} CubicToQuadraticCurveResult;

/// Approximate a cubic Bézier with quadratic Bézier curves using degree elevation.
/// Uses a simple subdivision approach: split at t=0.5 and approximate each half.
///
/// Should maybe look at the algoritm implemented suggested here:
/// https://www.rose-hulman.edu/~finn/CCLI/Notes/day12.pdf
/// and implemented here:
/// https://github.com/GreenLightning/gpu-font-rendering/blob/master/source/font.cpp#L266
CubicToQuadraticCurveResult Slug_CubicToQuadratics(Slug_CubicCurve aCubicCurve) {
  const float2 p0 = aCubicCurve.p1;
  const float2 p1 = aCubicCurve.p2;
  const float2 p2 = aCubicCurve.p3;
  const float2 p3 = aCubicCurve.p4;

  // Midpoint of cubic at t=0.5
  const float t = 0.5f;
  const float mt = 1.0f - t;
  const float mt2 = mt * mt;
  const float mt3 = mt2 * mt;
  const float t2 = t * t;
  const float t3 = t2 * t;
  const float2 mid = {
    mt3 * p0.x + 3.0f * mt2 * t * p1.x + 3.0f * mt * t2 * p2.x + t3 * p3.x,
    mt3 * p0.y + 3.0f * mt2 * t * p1.y + 3.0f * mt * t2 * p2.y + t3 * p3.y
  };

  // Control points for first quadratic (p0 to mid)
  const float c1_x = (2.0f * p0.x + p1.x) / 3.0f;
  const float c1_y = (2.0f * p0.y + p1.y) / 3.0f;
  const float c2_x = (p0.x + 2.0f * p1.x) / 3.0f;
  const float c2_y = (p0.y + 2.0f * p1.y) / 3.0f;
  const float2 q1_cp = {
    (c1_x + c2_x) / 2.0f,
    (c1_y + c2_y) / 2.0f
  };
  // Control for second quadratic (mid to p3)
  const float c3_x = (2.0f * p2.x + p3.x) / 3.0f;
  const float c3_y = (2.0f * p2.y + p3.y) / 3.0f;
  const float c4_x = (p2.x + 2.0f * p3.x) / 3.0f;
  const float c4_y = (p2.y + 2.0f * p3.y) / 3.0f;
  const float2 q2_cp = {
    (c3_x + c4_x) / 2.0f,
    (c3_y + c4_y) / 2.0f
  };

  CubicToQuadraticCurveResult result = {
    { {p0, q1_cp, mid}, { mid, q2_cp, p3 } },
  };

  return result;
}

typedef struct SlugProcessingData {
  Slug_QuadraticCurve** mContours;
  size_t mContoursSize;
  size_t mContoursCapacity;
  Slug_QuadraticCurve* mCurves;
  size_t mCurvesSize;
  size_t mCurvesCapacity;
  Slug_QuadraticCurve mCurrentCurves;
  float2 mLastPoint;
  float2 mStartPoint;
} SlugProcessingData;

void Slug_PushContour(SlugProcessingData* aData) {
  if (aData->mContours == NULL) {
    aData->mContoursCapacity = 1;
    aData->mContours = (Slug_QuadraticCurve**)SDL_calloc(aData->mContoursCapacity, sizeof(Slug_QuadraticCurve*));
  }

  if ((aData->mContoursSize + 1) > aData->mContoursCapacity) {
    aData->mContoursCapacity = aData->mContoursCapacity * 2; // Naive scaling factor;
    Slug_QuadraticCurve** tempContours = (Slug_QuadraticCurve**)SDL_calloc(aData->mContoursCapacity, sizeof(Slug_QuadraticCurve*));
    SDL_memcpy(tempContours, aData->mContours, sizeof(aData->mContours) * aData->mContoursSize);
  }

  aData->mContours[aData->mContoursSize] = aData->mCurves;
  ++aData->mContoursSize;

  aData->mCurves = NULL;
  aData->mCurvesSize = 0;
  aData->mCurvesCapacity = 0;
}

void Slug_PushCurve(SlugProcessingData* aData, Slug_QuadraticCurve aCurve) {
  if (aData->mCurves == NULL) {
    aData->mCurvesCapacity = 200; // Naive initial value
    aData->mCurves = (Slug_QuadraticCurve*)SDL_calloc(aData->mCurvesCapacity, sizeof(Slug_QuadraticCurve));
  }

  if ((aData->mCurvesSize + 1) > aData->mCurvesCapacity) {
    aData->mCurvesCapacity = aData->mCurvesCapacity * 2; // Naive scaling factor;
    Slug_QuadraticCurve* tempCurves = (Slug_QuadraticCurve*)SDL_calloc(aData->mCurvesCapacity, sizeof(Slug_QuadraticCurve));

    SDL_memcpy(tempCurves, aData->mCurves, sizeof(aData->mCurves) * aData->mCurvesSize);
  }

  aData->mCurves[aData->mCurvesSize] = aCurve;
  ++aData->mCurvesSize;
}

void Slug_CurveMoveTo(SlugProcessingData* aData, stbtt_vertex* aVertex)
{
  if (aData->mCurvesSize != 0) {
    Slug_PushContour(aData);
  }

  float vx = (float)aVertex->x, vy = (float)aVertex->y;

  aData->mLastPoint.x = aData->mStartPoint.x = vx;
  aData->mLastPoint.y = aData->mStartPoint.y = vy;
}

void Slug_CurveLineTo(SlugProcessingData * aData, stbtt_vertex * aVertex)
{
  float vx = (float)aVertex->x, vy = (float)aVertex->y;
  Slug_QuadraticCurve curve = {
    { aData->mLastPoint.x, aData->mLastPoint.y },
    {(aData->mLastPoint.x + vx) / 2.0f, (aData->mLastPoint.y + vy) / 2.0f },
    { vx, vy }
  };

  Slug_PushCurve(aData, curve);

  aData->mLastPoint.x = vx;
  aData->mLastPoint.y = vy;
}

void Slug_CurveQuadTo(SlugProcessingData* aData, stbtt_vertex* aVertex)
{
  float vx = (float)aVertex->x, vy = (float)aVertex->y;
  Slug_QuadraticCurve curve = {
    { aData->mLastPoint.x, aData->mLastPoint.y },
    { (float)aVertex->cx, (float)aVertex->cy },
    { (float)aVertex->x, (float)aVertex->y }
  };

  Slug_PushCurve(aData, curve);

  aData->mLastPoint.x = vx;
  aData->mLastPoint.y = vy;
}

void Slug_CurveCubicTo(SlugProcessingData* aData, stbtt_vertex* aVertex)
{
  float vx = (float)aVertex->x, vy = (float)aVertex->y;

  Slug_CubicCurve cubic = {
    { aData->mLastPoint.x, aData->mLastPoint.y },
    { (float)aVertex->cx, (float)aVertex->cy },
    { (float)aVertex->cx1, (float)aVertex->cy1 },
    { vx, vy }
  };
  CubicToQuadraticCurveResult result = Slug_CubicToQuadratics(cubic);

  Slug_PushCurve(aData, result.result[0]);
  Slug_PushCurve(aData, result.result[1]);

  aData->mLastPoint.x = vx;
  aData->mLastPoint.y = vy;
}

void ProcessCodepoint(stbtt_fontinfo* aFontInfo, SlugProcessingData* aData, Uint32 aCodePoint, int aGlyphIndex)
{
  stbtt_vertex* vertices;
  const int vertexCount = stbtt_GetGlyphShape(aFontInfo, aGlyphIndex, &vertices);

  SDL_Log("CodePoint: %d", aCodePoint);

  for (int i = 0; i < vertexCount; ++i) {
    SDL_Log("\ttype: %d",(int)vertices[i].type);
  }
}

SlugProcessingData ProcessFont(const char* aFontPath)
{
  stbtt_fontinfo fontInfo;
  SlugProcessingData data;
  SDL_zero(data);

  {
    size_t fontSize = 0;
    void* fontData = SDL_LoadFile(aFontPath, &fontSize);
    stbtt_InitFont(&fontInfo, (const unsigned char*)fontData, 0);
  }

  for (Uint32 i = 0; i < 0xFFFF; ++i)
  {
    int index = stbtt_FindGlyphIndex(&fontInfo, i);

    if (index != 0) {
      ProcessCodepoint(&fontInfo, &data, i, index);
    }
  }

  return data;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Technique Code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct TechniqueContext {
  SDL_GPUGraphicsPipeline* mPipeline;
  SDL_GPUBuffer* mVertexBuffer;
  SDL_GPUBuffer* mIndexBuffer;
  Transform mUniform[2];
} TechniqueContext;

TechniqueContext CreateTechniqueContext(SDL_GPUTextureFormat aDepthFormat) {
  SDL_GPUColorTargetDescription colorTargetDescription;
  SDL_zero(colorTargetDescription);
  colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(gContext.mDevice, gContext.mWindow);

  SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
  SDL_zero(graphicsPipelineCreateInfo);

  graphicsPipelineCreateInfo.target_info.num_color_targets = 1;
  graphicsPipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDescription;
  graphicsPipelineCreateInfo.target_info.depth_stencil_format = aDepthFormat;
  graphicsPipelineCreateInfo.target_info.has_depth_stencil_target = true;
  graphicsPipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  graphicsPipelineCreateInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;
  graphicsPipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;

  graphicsPipelineCreateInfo.vertex_input_state.num_vertex_buffers = 1;
  graphicsPipelineCreateInfo.vertex_input_state.num_vertex_attributes = 2;

  SDL_GPUVertexAttribute attributes[2];

  // Position
  attributes[0].location = 0;
  attributes[0].buffer_slot = 0;
  attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  attributes[0].offset = 0;

  // Color
  attributes[1].location = 1;
  attributes[1].buffer_slot = 0;
  attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  attributes[1].offset = sizeof(float3);

  graphicsPipelineCreateInfo.vertex_input_state.vertex_attributes = attributes;

  SDL_GPUVertexBufferDescription bufferDescription;
  bufferDescription.slot = 0;
  bufferDescription.pitch = 2 * sizeof(float3);
  bufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  bufferDescription.instance_step_rate = 0;

  graphicsPipelineCreateInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;


  // Remember to come back to this later in the tutorial, don't show it off immediately.
  graphicsPipelineCreateInfo.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;

  graphicsPipelineCreateInfo.depth_stencil_state.enable_depth_test = true;
  graphicsPipelineCreateInfo.depth_stencil_state.enable_depth_write = true;

  graphicsPipelineCreateInfo.vertex_shader = CreateShader(
    "VertexAndIndexBuffer.vert",
    SDL_GPU_SHADERSTAGE_VERTEX,
    0,
    3,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.vertex_shader);

  graphicsPipelineCreateInfo.fragment_shader = CreateShader(
    "VertexAndIndexBuffer.frag",
    SDL_GPU_SHADERSTAGE_FRAGMENT,
    0,
    0,
    0,
    0,
    SDL_PROPERTY_TYPE_INVALID
  );
  SDL_assert(graphicsPipelineCreateInfo.fragment_shader);

  SDL_assert(SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_GRAPHICSPIPELINE_CREATE_NAME_STRING, "TechniqueContext"));

  TechniqueContext context;
  context.mPipeline = SDL_CreateGPUGraphicsPipeline(gContext.mDevice, &graphicsPipelineCreateInfo);
  SDL_assert(context.mPipeline);

  {
    static const float3 cVertexPositions[16] = {
      /* 0 */ /* Position */ { -1.0f,  1.0f, -1.0f }, /* Color */ { 1.0f, 0.0f, 0.0f }, // top left forward
      /* 1 */ /* Position */ {  1.0f,  1.0f, -1.0f }, /* Color */ { 0.0f, 1.0f, 0.0f }, // top right forward
      /* 2 */ /* Position */ { -1.0f, -1.0f, -1.0f }, /* Color */ { 0.0f, 0.0f, 1.0f }, // bottom left foward
      /* 3 */ /* Position */ {  1.0f, -1.0f, -1.0f }, /* Color */ { 1.0f, 1.0f, 1.0f }, // bottom right forward
      /* 4 */ /* Position */ { -1.0f,  1.0f,  1.0f }, /* Color */ { 0.0f, 0.0f, 0.0f }, // top left back
      /* 5 */ /* Position */ {  1.0f,  1.0f,  1.0f }, /* Color */ { 1.0f, 1.0f, 0.0f }, // top right back
      /* 6 */ /* Position */ { -1.0f, -1.0f,  1.0f }, /* Color */ { 1.0f, 0.0f, 1.0f }, // bottom left back
      /* 7 */ /* Position */ {  1.0f, -1.0f,  1.0f }, /* Color */ { 0.0f, 1.0f, 1.0f }, // bottom right back
    };

    context.mVertexBuffer = CreateAndUploadBuffer(&cVertexPositions, sizeof(cVertexPositions), SDL_GPU_BUFFERUSAGE_VERTEX, "VertexBuffer");
  }

  {
    static const Uint16 cVertexIndices[36] = {
      // Front Face
      0, 1, 2,
      1, 3, 2,

      // Back Face
      5, 4, 7,
      4, 6, 7,

      // Top Face
      4, 5, 0,
      5, 1, 0,

      // Bottom Face
      2, 3, 6,
      3, 7, 6,

      // Left Face:
      4, 0, 6,
      0, 2, 6,

      // Right Face: 
      1, 5, 3,
      5, 7, 3,
    };

    context.mIndexBuffer = CreateAndUploadBuffer(&cVertexIndices, sizeof(cVertexIndices), SDL_GPU_BUFFERUSAGE_INDEX, "IndexBuffer");
  }

  context.mUniform[0].mPosition.x = 2.f;
  context.mUniform[0].mPosition.y = -1.f;
  context.mUniform[0].mPosition.z = 5.f;
  context.mUniform[0].mPosition.w = 0.f;
  context.mUniform[0].mScale.x = 0.5f;
  context.mUniform[0].mScale.y = 0.5f;
  context.mUniform[0].mScale.z = 0.5f;
  context.mUniform[0].mScale.w = 0.5f;
  context.mUniform[0].mRotation.x = 0.f;
  context.mUniform[0].mRotation.y = 0.f;
  context.mUniform[0].mRotation.z = 0.f;
  context.mUniform[0].mRotation.w = 0.f;

  context.mUniform[1].mPosition.x = 0.f;
  context.mUniform[1].mPosition.y = -1.f;
  context.mUniform[1].mPosition.z = 10.f;
  context.mUniform[1].mPosition.w = 0.f;
  context.mUniform[1].mScale.x = 2.f;
  context.mUniform[1].mScale.y = 2.f;
  context.mUniform[1].mScale.z = 2.f;
  context.mUniform[1].mScale.w = 2.f;
  context.mUniform[1].mRotation.x = 0.f;
  context.mUniform[1].mRotation.y = 0.f;
  context.mUniform[1].mRotation.z = 0.f;
  context.mUniform[1].mRotation.w = 0.f;

  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.vertex_shader);
  SDL_ReleaseGPUShader(gContext.mDevice, graphicsPipelineCreateInfo.fragment_shader);

  return context;
}

void DrawTechniqueContext(TechniqueContext* aContext, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPURenderPass* aRenderPass)
{
  SDL_BindGPUGraphicsPipeline(aRenderPass, aContext->mPipeline);

  float4x4 model = CreateModelMatrixFromTransform(&aContext->mUniform[0]);

  SDL_PushGPUVertexUniformData(aCommandBuffer, 1, &model, sizeof(model));
  SDL_PushGPUVertexUniformData(aCommandBuffer, 2, &gContext.WorldToNDC, sizeof(gContext.WorldToNDC));

  {
    SDL_GPUBufferBinding binding;
    binding.buffer = aContext->mVertexBuffer;
    binding.offset = 0;
    SDL_BindGPUVertexBuffers(aRenderPass, 0, &binding, 1);
  }

  {
    SDL_GPUBufferBinding binding;
    binding.buffer = aContext->mIndexBuffer;
    binding.offset = 0;
    SDL_BindGPUIndexBuffer(aRenderPass, &binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
  }

  // Draw the first cube
  SDL_DrawGPUIndexedPrimitives(aRenderPass, 36, 1, 0, 0, 0);

  // Draw the second cube, make sure to recalculate the model matrix for it and reupload it.
  model = CreateModelMatrixFromTransform(&aContext->mUniform[1]);
  SDL_PushGPUVertexUniformData(aCommandBuffer, 1, &model, sizeof(model));
  SDL_DrawGPUIndexedPrimitives(aRenderPass, 36, 1, 0, 0, 0);
}

void DestroyTechniqueContext(TechniqueContext* aContext)
{
  SDL_ReleaseGPUBuffer(gContext.mDevice, aContext->mVertexBuffer);
  SDL_ReleaseGPUBuffer(gContext.mDevice, aContext->mIndexBuffer);
  SDL_ReleaseGPUGraphicsPipeline(gContext.mDevice, aContext->mPipeline);
  SDL_zero(*aContext);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Camera Movements
void FlybyCamera(Transform* aCamera, const bool* aKeyMap, float2 aFrameMouseMove, float aSpeed, float aDt)
{
  Orientation orientation = GetOrientation(aCamera);
  float3 movementDirection = { 0.f, 0.f, 0.f };
  SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(NULL, NULL);

  if (SDL_BUTTON_LMASK & mouseFlags) {
    aCamera->mRotation.x += aFrameMouseMove.y * aSpeed * aDt * .05f;
    aCamera->mRotation.y += aFrameMouseMove.x * aSpeed * aDt * .05f;
  }

  if (aKeyMap[SDL_SCANCODE_D]) movementDirection = Float3_Add(movementDirection, orientation.mRight);
  if (aKeyMap[SDL_SCANCODE_A]) movementDirection = Float3_Subtract(movementDirection, orientation.mRight);
  if (aKeyMap[SDL_SCANCODE_W]) movementDirection = Float3_Add(movementDirection, orientation.mForward);
  if (aKeyMap[SDL_SCANCODE_S]) movementDirection = Float3_Subtract(movementDirection, orientation.mForward);
  if (aKeyMap[SDL_SCANCODE_SPACE]) movementDirection = Float3_Add(movementDirection, orientation.mUp);
  if (aKeyMap[SDL_SCANCODE_LSHIFT]) movementDirection = Float3_Subtract(movementDirection, orientation.mUp);

  if (SDL_BUTTON_MMASK & mouseFlags) {

    movementDirection = Float3_Add(movementDirection, Float3_Scalar_Multiply(orientation.mRight, aFrameMouseMove.x * -.1f));
    movementDirection = Float3_Add(movementDirection, Float3_Scalar_Multiply(orientation.mUp, aFrameMouseMove.y * .1f));
  }
  
  aCamera->mPosition = Float4_From3(Float3_Add(
    Float3_Scalar_Multiply(movementDirection, aSpeed * aDt),
    Float4_XYZ(aCamera->mPosition)),
    0.0f
  );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  SDL_assert(SDL_Init(SDL_INIT_VIDEO));



  ProcessFont("Assets/Roboto-Regular.ttf");



  SDL_Window* window = SDL_CreateWindow(TARGET_NAME, 1280, 720, 0);
  SDL_assert(window);

  CreateGpuContext(window);

  SDL_GPUTexture* depthTexture = NULL;
  Uint32 depthWidth = 0;
  Uint32 depthHeight = 0;
  SDL_GPUTextureFormat depthFormat = GetSupportedDepthFormat();

  TechniqueContext context = CreateTechniqueContext(depthFormat);
  Transform cameraTransform = GetDefaultTransform();
  cameraTransform.mPosition.z = -1.0f;
  //cameraTransform.mScale.x = 1.0f;
  //cameraTransform.mScale.y = 1.0f;
  //cameraTransform.mScale.z = 1.0f;

  const float speed = 5.f;
  Uint64 last_frame_ticks_so_far = SDL_GetTicksNS();
  int keys;
  const bool* key_map = SDL_GetKeyboardState(&keys);
  bool running = true;

  float2 mouseMove;

  while (running) {
    mouseMove.x = 0.f;
    mouseMove.y = 0.f;

    Uint64 current_frame_ticks_so_far = SDL_GetTicksNS();
    float dt = (current_frame_ticks_so_far - last_frame_ticks_so_far) / 1000000000.f;
    last_frame_ticks_so_far = current_frame_ticks_so_far;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.common.type) {
        case SDL_EVENT_QUIT:
          running = false;
          break;
        case SDL_EVENT_MOUSE_MOTION:
          mouseMove.x = event.motion.xrel;
          mouseMove.y = event.motion.yrel;
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

    if (key_map[SDL_SCANCODE_RIGHT])    context.mUniform[0].mPosition.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_LEFT])     context.mUniform[0].mPosition.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_UP])       context.mUniform[0].mPosition.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_DOWN])     context.mUniform[0].mPosition.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_E])        context.mUniform[0].mPosition.z += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_Q])        context.mUniform[0].mPosition.z -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_R])        context.mUniform[0].mScale.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_F])        context.mUniform[0].mScale.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_T])        context.mUniform[0].mScale.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_G])        context.mUniform[0].mScale.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_INSERT])   context.mUniform[0].mRotation.x += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_DELETE])   context.mUniform[0].mRotation.x -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_HOME])     context.mUniform[0].mRotation.y += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_END])      context.mUniform[0].mRotation.y -= speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_PAGEUP])   context.mUniform[0].mRotation.z += speed * dt * 1.0f;
    if (key_map[SDL_SCANCODE_PAGEDOWN]) context.mUniform[0].mRotation.z -= speed * dt * 1.0f;

    FlybyCamera(&cameraTransform, key_map, mouseMove, speed, dt);


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
      SDL_ReleaseGPUTexture(gContext.mDevice, depthTexture);
      depthTexture = CreateTexture(swapchainWidth, swapchainHeight, 1, 1, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET, depthFormat, "DepthTexture");
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

    float4x4 modelMatrix = CreateModelMatrixFromTransform(&cameraTransform);
    float4x4 viewMatrix = Float4x4_Inverse(&modelMatrix);

    SDL_PushGPUVertexUniformData(commandBuffer, 0, &viewMatrix, sizeof(viewMatrix));

    DrawTechniqueContext(&context, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  SDL_ReleaseGPUTexture(gContext.mDevice, depthTexture);

  DestroyTechniqueContext(&context);

  DestroyGpuContext();

  SDL_Quit();
  return 0;
}

#ifdef __cplusplus
} // end cpp_test
#endif
