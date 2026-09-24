---
title: An "FPS" Camera
description: Without a dynamic camera, we'll be really struggling when rendering models and scenes, so lets get this out of the way.
template: lesson_template.html
example_status: Finished
chapter_status: Outlined
collections: ["lessons"]
---


We could mostly brush off Camera Space and the View Matrix needed when working in 2D, but now that we're in 3D, we'll need to cover some more math. Something we skipped over a bit earlier when discussing coordinate spaces is that all Models have an inherent Matrix that represents where they exist within World Space:

```
    | X_Forward, X_Right, X_Up, X_Position |
M = | Y_Forward, Y_Right, Y_Up, Y_Position |
    | Z_Forward, Z_Right, Z_Up, Z_Position |
    | W_Forward, W_Right, W_Up, W_Position |

```

This isn't particularly interesting because we typically place Models at reasonable positions within Object Space such that they're easy to manipulate in World Space, and then we can Scale/Rotate/Move them around using the matrices covered earlier. So typically this Matrix looks like an implicit Identity Matrix:

```
    | 1, 0, 0, 0 |
M = | 0, 1, 0, 0 |
    | 0, 0, 1, 0 |
    | 0, 0, 0, 1 |
```

That would then get multiplied against some matrix we computed for this _instance_ of the model using the TRS Matrix, or similar.


So this Matrix is intended to transform from Object Space to World Space, but it turns out, that we can reverse that as well. If we have an Object in World Space, we can use the inverse of this Model Matrix

## A Reusable Transform

```c
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
```

## Inverting the Camera Transform

```c
float4x4 modelMatrix = CreateModelMatrixFromTransform(&cameraTransform);
float4x4 viewMatrix = Float4x4_Inverse(&modelMatrix);

SDL_PushGPUVertexUniformData(
  commandBuffer,
  0,
  &viewMatrix,
  sizeof(viewMatrix));
```

## Separating the Three Transform Uniforms

```hlsl
cbuffer UBO : register(b0, space1)
{
  float4x4 WorldToCamera;
};

cbuffer UBO : register(b1, space1)
{
  float4x4 ObjectToWorld;
};

cbuffer UB0 : register(b2, space1)
{
  float4x4 CameraToNDC;
};

output.Position = mul(
  CameraToNDC,
  mul(WorldToCamera, mul(ObjectToWorld, float4(input.Position, 1.0f))));
```

## Moving Relative to the Camera

```c
void FlybyCamera(
  Transform* aCamera,
  const bool* aKeyMap,
  float2 aFrameMouseMove,
  float aSpeed,
  float aDt)
{
  Orientation orientation = GetOrientation(aCamera);
  float3 movementDirection = { 0.f, 0.f, 0.f };

  if (aKeyMap[SDL_SCANCODE_D]) movementDirection = Float3_Add(movementDirection, orientation.mRight);
  if (aKeyMap[SDL_SCANCODE_A]) movementDirection = Float3_Subtract(movementDirection, orientation.mRight);
  if (aKeyMap[SDL_SCANCODE_W]) movementDirection = Float3_Add(movementDirection, orientation.mForward);
  if (aKeyMap[SDL_SCANCODE_S]) movementDirection = Float3_Subtract(movementDirection, orientation.mForward);
  if (aKeyMap[SDL_SCANCODE_SPACE]) movementDirection = Float3_Add(movementDirection, orientation.mUp);
  if (aKeyMap[SDL_SCANCODE_LSHIFT]) movementDirection = Float3_Subtract(movementDirection, orientation.mUp);

  aCamera->mPosition = Float4_From3(Float3_Add(
    Float3_Scalar_Multiply(movementDirection, aSpeed * aDt),
    Float4_XYZ(aCamera->mPosition)),
    0.0f);
}
```

## Rotating and Panning with the Mouse

```c
SDL_MouseButtonFlags mouseFlags = SDL_GetMouseState(NULL, NULL);

if (SDL_BUTTON_LMASK & mouseFlags) {
  aCamera->mRotation.x += aFrameMouseMove.y * aSpeed * aDt * .05f;
  aCamera->mRotation.y += aFrameMouseMove.x * aSpeed * aDt * .05f;
}

if (SDL_BUTTON_MMASK & mouseFlags) {
  movementDirection = Float3_Add(
    movementDirection,
    Float3_Scalar_Multiply(orientation.mRight, aFrameMouseMove.x * -.1f));
  movementDirection = Float3_Add(
    movementDirection,
    Float3_Scalar_Multiply(orientation.mUp, aFrameMouseMove.y * .1f));
}
```

## Updating the Camera Each Frame

```c
FlybyCamera(&cameraTransform, key_map, mouseMove, speed, dt);
```
