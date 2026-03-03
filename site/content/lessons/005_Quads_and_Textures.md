---
title: Quads and Textures
description: Extending off of Uniform Buffers, we'll learn about Texture Buffers, so we can finally display images beyond colored shapes.
template: lesson_template.html
example_status: Finished
chapter_status: Not Started
collections: ["lessons"]
---

Next up we'll cover rendering textures, and extending our geometry into quads. This'll get us most of the way to rendering simple 2D scenes.

## Textures

We'll be using SDL's built-in support for BMPs, but it also supports PNGs as of 3.4! First though, lets discuss some issues with texture formats you may not be familiar with if this is your first low-level graphics API.

Typically games use gpu compressed texture formats rather than things like PNG, BMP, TIFF, or JPEG. This comes down to data layouts and hardware support. The typical image formats you're familiar with are designed around minimizing file size, but this means it's essentially impossible for hardware to trivially sample individual pixels. This means that to use these formats on the gpu, we need to decompress them to a format such as RGBA where each component is 1 byte, with each Pixel being the Uint32 of those components combined side-by-side. This means the texture is significantly larger in memory, and slower to sample. 

We have compressed formats such as various versions of DXT, ETC, and ATSC that compress textures as a series of blocks, which gpus have hardware support for sampling. There's various tradeoffs between the different formats, the hardware you're targeting, and what needs to be done on load time to use them. We'll look into some options further on in the series.

For now we'll not be worrying too much about these details, and just use the built-in formats.

First we'll start with a small function to create a Texture resource. We'll need to create textures outside of loading up files from time to time, so it's nice to have a little function to simplify creation a bit. As usual we'll pass a name in and set it in the properties, the rest is quite straightforward.

```c
SDL_GPUTexture* CreateTexture(Uint32 aWidth, Uint32 aHeight, Uint32 layers, Uint32 levels, SDL_GPUTextureUsageFlags aUsage, SDL_GPUTextureFormat aFormat, const char* aName)
{
  SDL_SetStringProperty(gContext.mProperties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, aName);

  SDL_GPUTextureCreateInfo textureCreateInfo;
  SDL_zero(textureCreateInfo);
  textureCreateInfo.width = aWidth;
  textureCreateInfo.height = aHeight;
  textureCreateInfo.layer_count_or_depth = layers;
  textureCreateInfo.num_levels = levels;
  textureCreateInfo.usage = aUsage;
  textureCreateInfo.format = aFormat;
  textureCreateInfo.props = gContext.mProperties;
  return SDL_CreateGPUTexture(gContext.mDevice, &textureCreateInfo);
}
```
Next we'll do something similar for creating `SDL_GPUTransferBuffer`s. 

```c
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

```

For the most part we'll be usually be loading files and dumping them into textures, so we'll write a function that does this for us. Like with shaders, we'll load them from a specific place, and for now we'll assume everything is BMP. We can compose a path given a texture name, and then use `SDL_LoadBMP` to load an `SDL_Surface` with the texture data. For the sake of simplicity, we'll ensure we use RGBA32 format as discussed above, and if it's not, we can convert to it using `SDL_ConvertSurface`, and destroy the original afterword.

```c
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
```

Now that we've got the texture data in CPU memory in an expected format we can start working on uploading it to the GPU. For that we'll need a transfer buffer

```c
  Uint32 textureSize = surface->h * surface->pitch;

  char tranferBufferName[4096];
  SDL_snprintf(tranferBufferName, SDL_arraysize(tranferBufferName), "CreateAndUploadTexture Transfer Buffer for %s", aTextureName);

  SDL_GPUTransferBuffer* transferBuffer = CreateTransferBuffer(textureSize, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, tranferBufferName);

  void* transferPtr = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
  memcpy(transferPtr, surface->pixels, textureSize);
  SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer)
```


```c
SDL_GPUTexture* texture = CreateTexture(surface->w, surface->h, 1, 1, SDL_GPU_TEXTUREUSAGE_SAMPLER, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, aTextureName);
SDL_assert(texture);
```

```c
SDL_GPUCommandBuffer* commandBuffer = NULL;
SDL_GPUCopyPass* copyPass = aCopyPass;
bool needsToSubmit = NULL == copyPass;
if (needsToSubmit) {
  commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
  copyPass = SDL_BeginGPUCopyPass(commandBuffer);
}
```

```c
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
```
Finally we can close out the copypass and commandbuffer if we needed to make them, as well as releasing the TransferBuffer. You don't need to worry about releasing it before submitting the command buffer if we _had_ passed one, as it's internally refcounted by SDL.

```c
if (needsToSubmit) {
  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(commandBuffer);
}

SDL_ReleaseGPUTransferBuffer(gContext.mDevice, transferBuffer);

return texture;
```


## Transfer Buffers


## Uploads


## The Vertex Shader

### The Quad and an index array

Up until now, we've only stored arrays of vertices, this made sense because we've only been dealing with a single triangle. Once you want to render a texture however, you'll immediately realize that you'd be duplicating two vertices for that quad you want to display it on!

```

 (-1, 1)                 (1, 1)
     0------------------1    1
     |                 /    /|
     |  Triangle 1   /    /  |
     |             /    /    |
     |           /    /      |
     |         /    /        |
     |       /    /          |
     |     /    /            |
     |   /    /  Triangle 2  |
     | /    /                |
     2     2-----------------3
    (-1, -1)              (1, -1)
```

See how the first and second vertex are the same on each triangle? It doesn't look like much now, but when you start rendering models, this would really start increasing the size of them. So lets learn about indexing now, ahead of when we'll need them for some of the more serious 3D applications later on.

Similar to last time, we'll be declaring some static data in the shader, but this time with an added vertex, as well as an array of indices to index with.

```
static const float2 cVertexPositions[4] = {
  {-1.0f,  1.0f},
  { 1.0f,  1.0f},
  {-1.0f, -1.0f},
  { 1.0f, -1.0f},
};

static const uint cVertexIndices[6] = {
  0, 1, 2, // Triangle 1
  1, 3, 2  // Triangle 2
};
```

We can see our indices align with the diagram above. An astute reader might also notice that these vertices match the dimensions of the screen. Don't worry, we'll learn how to adjust the size of it in this chapter so it won't take up the whole screen!

### UV Coordinates

## The Pixel Shader