---
title: Quads and Textures
description: Extending off of Uniform Buffers, we'll learn about Texture Buffers, so we can finally display images beyond colored shapes.
template: lesson_template.html
example_status: Finished
chapter_status: In progress
collections: ["lessons"]
---

Next up we'll cover rendering textures, and extending our geometry into quads. This'll get us most of the way to rendering simple 2D scenes.

## Textures

We'll be using SDL's built-in support for BMPs, but it also supports PNGs as of 3.4! First though, lets discuss some issues with texture formats you may not be familiar with if this is your first low-level graphics API.

Typically games use gpu compressed texture formats rather than things like PNG, BMP, TIFF, or JPEG. This comes down to data layouts and hardware support. The typical image formats you're familiar with are designed around minimizing file size, but this means it's essentially impossible for hardware to trivially sample individual pixels. This means that to use these formats on the gpu, we need to decompress them to a format such as RGBA where each component is 1 byte, with each Pixel being the Uint32 of those components combined side-by-side. This means the texture is significantly larger in memory, and slower to sample. 

We have compressed formats such as various versions of DXT, ETC, and ATSC that compress textures as a series of blocks, which gpus have hardware support for sampling. There's various tradeoffs between the different formats, the hardware you're targeting, and what needs to be done on load time to use them. We'll look into some options further on in the series, but for now we'll not be worrying too much about these details, and just use the built-in formats.

First we'll start with a small function to create a Texture resource. We'll need to create textures outside of loading up files from time to time, so it's nice to have a little function to simplify creation a bit. As usual we'll pass a name in and set it in the properties, the rest is _mostly_ straightforward.

```c
SDL_GPUTexture* CreateTexture(Uint32 aWidth, Uint32 aHeight, Uint32 layers_or_depth, Uint32 levels, SDL_GPUTextureUsageFlags aUsage, SDL_GPUTextureFormat aFormat, const char* aName)
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

Part of creating a texture is deciding how much memory to allocate, and for this the API will need to know nearly every other parameter here. Width and Height are self explanatory, and we've briefly discussed how format determines how the data is laid out, which should indicate that it's also relevant to the size of the data required, how much is dependent on the specific format.

Layers/Depth and Levels are a little less obvious. Layers and Depth are relatively simple to explain, they're essentially just additional textures of the same size and format. These can be used for any number of things, maybe you store the different faces of a skybox (a cube with textures on the inside that are used to display the sky), or you store several texture atlases together to reduce on texture rebindings. All we really need to consider for now is that we just need a single layer for most tasks currently, multiple levels and depth will only really come up when we use 2D Array textures or 3D textures. Levels on the other hand relate to what are referred to as mipmaps. They're somewhat similar to layers, but rather than being the same size, each additional mip level is another texture that's smaller than the last. Traditionally they're intended to be downsampled, although perhaps touched up by artists, copies of the full texture, to be automatically used when the triangle displaying the texture is of some size where the full texture's quality would go to waste. For various cache efficiency reasons this ends up being a large performance win for objects not close to the screen. We'll talk more about each of those in the future, but that should suffice for a high level explanation.

Finally there's the usage parameter, which is kind of what it sounds like. We need to tell SDL_GPU how we plan to use this texture, for now, we really only care about `SDL_GPU_TEXTUREUSAGE_SAMPLER`, but you may wish to look at the various other options of [`SDL_GPUTextureUsageFlags`](https://wiki.libsdl.org/SDL3/SDL_GPUTextureUsageFlags) on your own. We'll be discussing them as they're relevant.

Next we'll do something similar for creating `SDL_GPUTransferBuffer`s, which are intermediate buffers we'll use to copy our texture data to the GPU. And of course, we'll use them for other types of buffers in the future as well.

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

This one is a bit simpler, since we only care about usage and size. Usage just determines if this transfer buffer will be used for CPU -> GPU (upload), or GPU -> CPU (download) transfers. For now, we'll only be looking at uploading.

For the most part we'll be usually be loading files and dumping them into textures, so we'll write a function that does this for us. Like with shaders, we'll load them from a specific place. We can compose a path given a texture name, and then use `SDL_LoadSurface` to load an `SDL_Surface` with the texture data. For the sake of simplicity, we'll ensure we use RGBA32 format as discussed above, and if it's not, we can convert to it using `SDL_ConvertSurface`, and destroy the original afterword. 

Regarding the copy pass parameter, we'll discuss that a bit down below, but just know that `NULL` is a perfectly valid argument here, and it's what we'll be doing for awhile.

```c
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
```

Now that we've got the texture data in CPU memory in an expected format we can start working on uploading it to the GPU. For that we'll need a tranfer buffer, as discussed. We'll be using it on upload, and we can calculate the size we'll need by multiplying the height of the texture, by the pitch. The pitch is the size in bytes of a row (width) of pixels, which may require padding depending on the size and format of the image, not to mention the different sizes of pixels.

```c
  Uint32 textureSize = surface->h * surface->pitch;

  SDL_snprintf(stringBuffer, SDL_arraysize(stringBuffer), "CreateAndUploadTexture Transfer Buffer for %s", aTextureName);

  SDL_GPUTransferBuffer* transferBuffer = CreateTransferBuffer(textureSize, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, stringBuffer);
```

Once we have the transfer buffer we can map it, which will give us a pointer to memory we're allowed to touch, and then we can simply memcpy our texture data into it. Then we just need to unmap it. Now there's a lot of discussion we could have about keeping around our transfer buffers, and persistently mapping them, but for our case here, we're going to be comfortable just creating them and throwing them away. Regarding persistent mapping, there's not a ton of immediate need for that unless you're creating a much more complicated upload system. Perhaps we can explore that and keeping around transfer buffers later, with the latter being much more generally applicable.

```c
  void* transferPtr = SDL_MapGPUTransferBuffer(gContext.mDevice, transferBuffer, false);
  memcpy(transferPtr, surface->pixels, textureSize);
  SDL_UnmapGPUTransferBuffer(gContext.mDevice, transferBuffer)
```

Next we need to make a texture resource, we talked a about the parameters above, so the calls should seem straightforward. We don't need any additional layers, and we're not dealing with mipmaps yet, so each of those are 1. We want to display our textures from a fragment shader, so our usage will be `SDL_GPU_TEXTUREUSAGE_SAMPLER`, and since we're using a RGBA32 pixel format on our surface, this maps to `SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM` in SDL_GPU. 

```c
SDL_GPUTexture* texture = CreateTexture(surface->w, surface->h, 1, 1, SDL_GPU_TEXTUREUSAGE_SAMPLER, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, aTextureName);
SDL_assert(texture);
```

Now we get to discuss copy passes a bit. You'll remember we've been using render passes previously, this is a similar idea. Like how render passes contain many rendering operations, so too does a copy pass contain many copying operations. We'll need one of these to move data from our transfer buffer into the texture. To accommodate possible future use of this function, we took a copy pass. This is for future chapters where we might load a bunch of things all at the same time and this function is just one among many loads. In a situation like that we'd only want to create one copy pass and use it for all of our transfers. For our use case, it won't be uncommon that we want a one-off copy pass, so we'll accept a `NULL` copy pass, and create our own if so. 

Thankfully getting them is pretty easy, we're also assuming that if we weren't handed a copy pass, there's likely no command buffer as well, so we'll treat that as a one-off as well.

```c
SDL_GPUCommandBuffer* commandBuffer = NULL;
SDL_GPUCopyPass* copyPass = aCopyPass;
bool needsToSubmit = NULL == copyPass;
if (needsToSubmit) {
  commandBuffer = SDL_AcquireGPUCommandBuffer(gContext.mDevice);
  copyPass = SDL_BeginGPUCopyPass(commandBuffer);
}
```

Now that we've got a copypass, we can issue the actual copy command. In this case we're copying to a texture, so we need a `SDL_GPUTextureTransferInfo` and `SDL_GPUTextureRegion` to describe it. You can essentially think of these as the transfer info describing the source data and the region the destination texture. With that in mind, given we're not doing anything special, this should be rather straightforward. We use the surface width and height to describe the rows and columns of the source data, and the `w` and `h` of the destination texture. We need to specify that we're using the transfer buffer from above as the source, and the texture we created as the destination. The last two interesting pieces of this is the `d` field of the region and the `false` we pass to the upload command. The `d` field refers to the layer or depth we're copying to. We're only handling normal textures here, and we only passed 1 for layers, so that's the layer we're writing to. Regarding the `false`, this corresponds to something called [cycling](https://wiki.libsdl.org/SDL3/CategoryGPU#a-note-on-cycling). For now we don't want to request cycling, but it's related to doing multiple uploads to the same buffer or texture. We'll discuss it more later on when we start doing those updates.

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

Finally we can close out the copy pass and command buffer if we needed to make them, as well as releasing the TransferBuffer. You don't need to worry about releasing it before submitting the command buffer if we _had_ passed one, as it's internally refcounted by SDL.

```c
if (needsToSubmit) {
  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(commandBuffer);
}

SDL_ReleaseGPUTransferBuffer(gContext.mDevice, transferBuffer);

return texture;
```

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