---
title: Vertex and Index Buffers for Real
description: Move cube geometry out of the shader and into vertex and index buffers.
template: lesson_template.html
example_status: Finished
chapter_status: Outlined
collections: ["lessons"]
---

## Creating and Uploading a GPU Buffer

```c
SDL_GPUBuffer* CreateAndUploadBuffer(
  const void* aData,
  Uint32 aSize,
  SDL_GPUBufferUsageFlags aUsage,
  const char* aName)
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
```

## Describing Interleaved Vertex Data

```c
graphicsPipelineCreateInfo.vertex_input_state.num_vertex_buffers = 1;
graphicsPipelineCreateInfo.vertex_input_state.num_vertex_attributes = 2;

SDL_GPUVertexAttribute attributes[2];
attributes[0].location = 0;
attributes[0].buffer_slot = 0;
attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
attributes[0].offset = 0;

attributes[1].location = 1;
attributes[1].buffer_slot = 0;
attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
attributes[1].offset = sizeof(float3);

SDL_GPUVertexBufferDescription bufferDescription;
bufferDescription.slot = 0;
bufferDescription.pitch = 2 * sizeof(float3);
bufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
bufferDescription.instance_step_rate = 0;
```

## Uploading the Cube Vertices

```c
static const float3 cVertexPositions[16] = {
  /* 0 */ /* Position */ { -1.0f,  1.0f, -1.0f }, /* Color */ { 1.0f, 0.0f, 0.0f },
  /* 1 */ /* Position */ {  1.0f,  1.0f, -1.0f }, /* Color */ { 0.0f, 1.0f, 0.0f },
  /* 2 */ /* Position */ { -1.0f, -1.0f, -1.0f }, /* Color */ { 0.0f, 0.0f, 1.0f },
  /* 3 */ /* Position */ {  1.0f, -1.0f, -1.0f }, /* Color */ { 1.0f, 1.0f, 1.0f },
  /* 4 */ /* Position */ { -1.0f,  1.0f,  1.0f }, /* Color */ { 0.0f, 0.0f, 0.0f },
  /* 5 */ /* Position */ {  1.0f,  1.0f,  1.0f }, /* Color */ { 1.0f, 1.0f, 0.0f },
  /* 6 */ /* Position */ { -1.0f, -1.0f,  1.0f }, /* Color */ { 1.0f, 0.0f, 1.0f },
  /* 7 */ /* Position */ {  1.0f, -1.0f,  1.0f }, /* Color */ { 0.0f, 1.0f, 1.0f },
};

context.mVertexBuffer = CreateAndUploadBuffer(
  &cVertexPositions,
  sizeof(cVertexPositions),
  SDL_GPU_BUFFERUSAGE_VERTEX,
  "VertexBuffer");
```

## Uploading the Cube Indices

```c
static const Uint16 cVertexIndices[36] = {
  0, 1, 2, 1, 3, 2,
  5, 4, 7, 4, 6, 7,
  4, 5, 0, 5, 1, 0,
  2, 3, 6, 3, 7, 6,
  4, 0, 6, 0, 2, 6,
  1, 5, 3, 5, 7, 3,
};

context.mIndexBuffer = CreateAndUploadBuffer(
  &cVertexIndices,
  sizeof(cVertexIndices),
  SDL_GPU_BUFFERUSAGE_INDEX,
  "IndexBuffer");
```

## Reading Vertex Inputs in the Shader

```hlsl
struct Input
{
  float3 Position : TEXCOORD0;
  float3 Color : TEXCOORD1;
};

Output main(Input input)
{
  Output output;
  output.Position = mul(WorldToNDC, mul(ObjectToWorld, float4(input.Position, 1.0f)));
  output.Color = input.Color;
  return output;
}
```

## Binding and Drawing Indexed Geometry

```c
SDL_GPUBufferBinding vertexBinding;
vertexBinding.buffer = aContext->mVertexBuffer;
vertexBinding.offset = 0;
SDL_BindGPUVertexBuffers(aRenderPass, 0, &vertexBinding, 1);

SDL_GPUBufferBinding indexBinding;
indexBinding.buffer = aContext->mIndexBuffer;
indexBinding.offset = 0;
SDL_BindGPUIndexBuffer(aRenderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

SDL_DrawGPUIndexedPrimitives(aRenderPass, 36, 1, 0, 0, 0);
```
