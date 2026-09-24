---
title: Depth
description: Draw overlapping quads at different depths and use a depth buffer to resolve their visibility.
template: lesson_template.html
example_status: Finished
chapter_status: Outlined
collections: ["lessons"]
---

## Choosing a Depth Format

```c
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
    if (SDL_GPUTextureSupportsFormat(
      gContext.mDevice,
      possibleFormats[i],
      SDL_GPU_TEXTURETYPE_2D,
      SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
    {
      return possibleFormats[i];
    }
  }

  SDL_assert(false);
  return SDL_GPU_TEXTUREFORMAT_INVALID;
}
```

## Enabling Depth Testing in the Pipeline

```c
graphicsPipelineCreateInfo.target_info.depth_stencil_format = aDepthFormat;
graphicsPipelineCreateInfo.target_info.has_depth_stencil_target = true;

graphicsPipelineCreateInfo.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;
graphicsPipelineCreateInfo.depth_stencil_state.enable_depth_test = true;
graphicsPipelineCreateInfo.depth_stencil_state.enable_depth_write = true;
```

## Giving Each Quad a Depth

```c
typedef struct ModelUniform {
  float2 mPosition;
  float2 mScale;
  float mDepth;
} ModelUniform;
```

```c
context.mUniform[0].mPosition.x = -0.25f;
context.mUniform[0].mDepth = 0.75f;

context.mUniform[1].mPosition.x = 0.25f;
context.mUniform[1].mDepth = 0.25f;
```

```hlsl
struct ModelUniform
{
  float2 mPosition;
  float2 mScale;
  float mDepth;
};

output.Position = float4(transformedPosition, cModelUniform.mDepth, 1.0f);
```

## Drawing the Far Quad Last

```c
SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aContext->mUniform[0], sizeof(aContext->mUniform[0]));
SDL_DrawGPUPrimitives(aRenderPass, 6, 1, 0, 0);

SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &aContext->mUniform[1], sizeof(aContext->mUniform[1]));
SDL_DrawGPUPrimitives(aRenderPass, 6, 1, 0, 0);
```

## Creating and Resizing the Depth Texture

```c
if (depthWidth != swapchainWidth || depthHeight != swapchainHeight)
{
  SDL_ReleaseGPUTexture(gContext.mDevice, depthTexture);
  depthTexture = CreateTexture(
    swapchainWidth,
    swapchainHeight,
    1,
    1,
    SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
    depthFormat,
    "DepthTexture");
  SDL_assert(depthTexture);

  depthWidth = swapchainWidth;
  depthHeight = swapchainHeight;
}
```

## Attaching the Depth Texture to the Render Pass

```c
SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo;
SDL_zero(depthStencilTargetInfo);
depthStencilTargetInfo.texture = depthTexture;
depthStencilTargetInfo.clear_depth = 0.f;
depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;

SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
  commandBuffer,
  &colorTargetInfo,
  1,
  &depthStencilTargetInfo
);
```
