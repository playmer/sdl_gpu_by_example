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

First we'll start with a small function to create a Texture resource:

```c
SDL_GPUTexture* CreateTexture(Uint32 aWidth, Uint32 aHeight, SDL_GPUTextureUsageFlags aUsage, SDL_GPUTextureFormat aFormat, const char* aName)
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