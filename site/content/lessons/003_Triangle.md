---
title: Triangle and a "Fullscreen Triangle"
description: After clearing the screen, the next milestone is rendering a triangle Triangle. Will cover the creation of Vertex and Fragment shaders and a simple demonstration of "Vertex Pulling".
template: lesson_template.html
example_status: Finished
chapter_status: Not Finished
collections: ["lessons"]
---

The big one, it's time to draw a Triangle. This is a text heavy one, we need to get into a bit of the high level details on how GPUs work, as well as some geometry. Lets start by adding to the previous example

## The Triangle Context

Thankfully, drawing a triangle is relatively simple, so we can try to take things one step at a time. And the first is to encapsulate everything needed to draw one:

```c
typedef struct TriangleContext {
  SDL_GPUGraphicsPipeline* mPipeline;
} TriangleContext;
```

We actually only need a pipeline to draw a triangle, but as we proceed, we'll need much more than that, so we're going to structure this as if we were doing something more complex. 

Before we discuss creating one, we should go over what a pipeline is and how it fits into the rendering puzzle.

## Shaders and Pipelines

When we look at how we do real-time rendering today, it's essentially a combination of generally tweakable fixed stages and programmable stages. We got a taste of this fixed functionality when we set-up and executed a RenderPass in the previous chapter. We were able to clear the screen just by tweaking the initialization and target of the RenderPass, with no need to submit geometry to proceed through the rest of the stages.

Pipelines are how we configure everything else not encapsulated in a RenderPass. You have to explain the layout of your data and how they flow from stage to stage, along with what you want the GPU to react to and modify data between and after the stages. A rough outline of the stages you'd typically see is the following:

```
+-----------------+    +-----------------+    +--------------------+ 
| Input Assembler | -> | Vertex Shader   | -> | Tesselation Shader | --+
+-----------------+    +-----------------+    +--------------------+   |
 +---------------------------------------------------------------------+
 |    +-----------------+    +---------------+    +------------------+
 + -> | Geometry Shader | -> | Rasterization | -> | Fragment Shaders |
      +-----------------+    +---------------+    +------------------+ 
```

We should first mention the Input Assembler which is something that's technically always happening, but we won't interact with much for some time. Essentially it takes data we describe in the Pipeline, and sets it up to be used in the subsequent stages.

All that said, SDL GPU doesn't give us access to every programmable stage you see above, many aren't relevant or performant today and some are too new to have been added for compatability reasons. What it does give us access to are the classics, Vertex Shaders and Fragment Shaders. It also has Compute Shaders to boot, but they're not part of the Render Pipeline, though we'll find they're still very useful for rendering later on. With regards to our concerns here, we'll write Vertex Shaders to output individual vertices, and Fragment Shaders to output colors.

> Note: If you've heard of Mesh Shaders, they effectively replace all of the geometry related stages. What we do in Vertex shaders would likely be sort of the base functionality of learning Mesh Shaders. That said, they're designed to be smarter around data management, letting you implement techniques that we'll have to split between Compute and Vertex shaders when we get there.

So if you're unfamiliar, when I say "programmable" I mean it literally. Shaders are programs that execute on your GPU. Each shader stage tends to have slightly different requirements, you can think about it like function signatures, although it's not a perfect analogy. In general they all have things they need to output, but they can optionally output additional information to be used in the next stage. In some cases that information may be interpolated, such as coordinates along the surface of a triangle. We'll get into the details later!

Each Graphics API takes a different "native" shader input, Vulkan takes SPIR-V, DirectX takes DXIL, and Metal takes MSL. I should mention that when you get into extensions or development only situations there's slightly more flexibility than that, but for our purposes this is good enough to think about. Each of these are an intermediate representation of your program, lying somewhere between the text you wrote in the language you chose, and machine code that can run on your GPU. When implementing the respective Graphics API you'll need to pass it the intermediate it expects. This is also true of SDL_GPU, in which you need declare which ones you can pass, and it can fail if there isn't a backend which can accept it. 

There's many languages specifically for shaders, and even general purpose programming languages that can be compiled to, but as stated in the intro, we'll be using HLSL as we have SDL_shadercross to use with well documented binding and set descriptions.


## Vertices, Briefly

For this chapter, to not _completely_ overload on information, we're only thinking about triangles, which are each made up of three Vertices, essentially positions on a 2D plane for now, that will look like this in code:

```
typedef struct float2 {
    float x, y;
}
```

In practice, we're actually going to want several more of these position/vector types:

```
typedef struct float3 {
    float x, y, z;
}

typedef struct float4 {
    float x, y, z, w;
}
```

These are each useful for various tasks, such as representing the previous dimensional vector in homogenous coordinates, we'll get into them more as we discuss some of the math we use later on. 

Although for right now we won't need the above definitions in our C code, you can add it if you like, but the sample won't have them yet.

We'll just be using static geometry within the shader, with no buffers. We'll get into these details later, but for now, just know that if the individual vertices are within the box below, the triangle will proceed to be shaded. 

```
(-1, 1)                       (1, 1)
    +---------------------------+
    |                           |
    |                           |
    |                           |
    |                           |
    |                           |
    |                           |
    |                           |
    +---------------------------+
(-1, -1)                     (1, -1)
```

We'll go into _much_ greater detail on the Vertex pipeline, Vertex shaders, and techniques that can be used with them in a chapter or two. Right now though, these are the minimum details you'll need to get going.

## The Vertex Shader

Now that we've discussed the theory, let's get back to the code a bit, but before we can make a pipeline, we'll need to write both a Vertex and Fragment shader. We'll create a couple of files [`Triangle.vert.hlsl` and `Triangle.frag.hlsl`] to start working on, starting with the Vertex shader.

To display a triangle, we must produce geometry. For very simple shapes like Triangle, and even cubes, we can get away with storing this data within the shader itself. And it's helpful to learn this how to do this because this isn't unlike how you might pull geometry out of a storage buffer. Or for 2D you may store the transformations and UV info for sprites into a storage buffer but produce geometry like we will here.

Our first set of geometry will be a triangle that looks roughly like this:

```

(-1, 1)                       (1, 1)
    +--------------0--------------+
    |             / \ (0, 1)      |
    |           /     \           |
    |         /         \         |
    |       /             \       |
    |     /                 \     |
    |   /                     \   |
    | /                         \ |
    2-----------------------------1
(-1, -1)                     (1, -1)
  
```

With each number at the points representing a vertex in an array. We're specifically laying out the geometry clockwise, which won't come up in this particular example, but will be relevant later in the cube example.

```hlsl
static const float2 cVertexPositions[3] = {
    { 0.0f,  1.0f},
    { 1.0f, -1.0f},
    {-1.0f, -1.0f},
};
```

We're going to give each coordinate a color as well, so it'll stand out a little more. We'll be able to pass this data from the vertex shader to the pixel shader, we'll go into more detail in a moment. 

```hlsl
static const float3 cColors[3] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
};
```

Now we've discussed that Vertex Shaders have to output geometry, we do this through returning a value from our `main` function which we'll be writing in a moment. That said, we do not need to return just a single value. Vertex Shaders can also output data for Fragment Shaders to use. In general these values will be interpolated based on the values of the vertices output by this function based on the position of the pixel within the triangle those vertices make up. We'll discuss this in a bit more detail during the FullScreen Triangle example in the next chapter.

So, to return multiple values, you create a struct that contains those values. The shader needs to know which field means what and the method by which we do this are called "Semantics" in HLSL. 

```hlsl
struct Output
{
  float4 Position : SV_Position;
  float3 Color : TEXCOORD0;
};
```
Here we're telling our shader compiler that we're outputting the vertex position through the required built-in `SV_Position` semantic. Similarly we're going to want to output the color we've assigned to this vertex and send it along to the Fragment shader. We do this via `TEXCOORD0`. Now it should be noted that all non-built-in input/output variables will use this semantic with an incrementing `n` in `TEXCOORDn`, so `TEXCOORD0`, `TEXCOORD1` and so on. The `n`s being unique to the input or output struct they're in. This is essentially just a quirk of the shader compiler we're using as it makes it easier for them to transpile our shaders to the other shader languages, but it should be noted that even natively in DX12, the other semantic names are equally unneeded, they're more for the programmer.

Now let's take a look at our main function, it's fairly simple.

```hlsl
Output main(uint id : SV_VertexID)
{
  uint vertexIndex = id % 3;

  Output output;
  output.Position = float4(cVertexPositions[vertexIndex], 0.0f, 1.0f);
  output.Color = cColors[vertexIndex];
  return output;
}
```

Right off the bat we see two interesting things, the return value is using our `Output` struct, and we're taking a paramter and it's marked with a built-in semantic. When we mark inputs with semantics, this means that we're asking the shader runtime to let us use that built-in variable. In this case `SV_VertexID` represents the ID/index of the vertex we're currently processing in this shader. Remember that the Vertex Shader will run for every vertex you request to draw, so in this case it's purely an ID and not an index, as we're not traversing any buffers.

Next we compute the index into our constant buffers for position and color by using modulo to ensure we're always looking within our 3 vertx/colors. The values at that index will be used to fill in our `Output` instance. Notice that we're passing this vertex position into a `float4` along with two other values. Because the existing position is a `float2`, this means we're extending it to be a `float4`, with the latter values filing in for `z` and `w`.

Now you might wonder why we're using modulo here, since we know we're only going to try rendering 3 vertices. Honestly, you'd be right to think it's unneeded. We're not going to need this for this chapter. But we'll be extending this shader in future chapters, and it's a good habit to get into, as you'll probably not always be rendering only one thing if you're using Vertex Pulling like this.

## Fragments (and Pixels)



## Compiling your Shaders

The samples all build their shader code as part of their CMake build, and you're encouraged to copy the sample from Chapter 1 and reuse it for all of them to sidestep the details here, but let's cover this briefly.

As discussed previously, we use [`SDL_shadercross`](https://github.com/libsdl-org/SDL_shadercross) for compiling our shaders. We'll cover compiling all three types of shaders here, but you'll only be compiling your vertex and pixel shaders in this chapter.

The anatomy of a call into `SDL_shadercross` is as such:

```bash
SDL_shadercross <InputShader> -g --source <SourceFormat> --dest <DestinationFormat> --stage <ShaderStage> --output <OutputShader>
```
The `SourceFormat` will always be HLSL for also, but you can also pass SPIRV into it. Obviously it must match the format of the `InputShader`. Similarly `DestinationFormat` is the format we're compiling for, and it's waht the `OutputShader` will come out as. And you must also let the tool know which `ShaderStage` you're compiling this shader for. Finally one addition here that hasn't been mentioned is the `-g` flag. This ensures the compiler emits debug info into the compiled shader. This isn't something you'd want to use in a released game or for a release build, but it's useful for debugging and trying to figure out why you're seeing what you're seeing.

When we automate this, we generally generate shaders for all backends of SDL_GPU, but if you'd like to simplify things for yourself, you can choose to only do the ones availible to you, or choose one among those and only compile that one. Just ensure you adjust your call to SDL_CreateGPUDevice to only take the ones you're compiling.

Lets see what compiling the vertex shader for all of our backends looks like.

```bash
SDL_shadercross Triangle.vert.hlsl -g --source HLSL --dest SPIRV --stage vertex --output Triangle.vert.spv
SDL_shadercross Triangle.vert.hlsl -g --source HLSL --dest MSL --stage vertex --output Triangle.vert.msl
SDL_shadercross Triangle.vert.hlsl -g --source HLSL --dest DXIL --stage vertex --output Triangle.vert.dxil
```

And the Fragment shader

```bash
SDL_shadercross Triangle.frag.hlsl -g --source HLSL --dest SPIRV --stage fragment --output Triangle.frag.spv
SDL_shadercross Triangle.frag.hlsl -g --source HLSL --dest MSL --stage fragment --output Triangle.frag.msl
SDL_shadercross Triangle.frag.hlsl -g --source HLSL --dest DXIL --stage fragment --output Triangle.frag.dxil
```
And a theoretical, but nonexistance, compute shader

```bash
SDL_shadercross Triangle.comp.hlsl -g --source HLSL --dest SPIRV --stage compute --output Triangle.comp.spv
SDL_shadercross Triangle.comp.hlsl -g --source HLSL --dest MSL --stage compute --output Triangle.comp.msl
SDL_shadercross Triangle.comp.hlsl -g --source HLSL --dest DXIL --stage compute --output Triangle.comp.dxil
```

Assuming your shaders were correct, you should now have those output files and be able to load them as we're setting up our pipeline.

## Back to Creating the Pipeline



