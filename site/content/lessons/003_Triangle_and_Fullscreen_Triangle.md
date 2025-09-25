---
title: Triangle and a "Fullscreen Triangle" (Example Done, Started Writing)
description: Finally you can see a triangle, and then a demonstration on why you might actually use a single triangle later on in your Graphics journey. Will cover the creation of Vertex and Pixel shaders, a simple demonstration of "Vertex Pulling", and then an extension of the triangle to cover the whole screen with a shader effect.
template: lesson_template.html
collections: ["lessons"]
---

The big one, it's time to draw a Triangle. And after that, we're going to learn a trick on how to extend this triangle to display over the entire screen. This technique is a small optimization that gets used when you start learning about fullscreen effects. We'll be using it in the next chapter to play around with pixel shaders to get a little more comfortable with writing transfering data to the GPU as well as writing code for it.

That said, this is a text heavy one, we need to get into a bit of the high level details on how GPUs work, as well as some geometry. If you just want to work through the examples, I understand, and you can do some fun stuff at a deep level. Just know that to have a good understanding of these, you'll want to read these sections, or similar from other sources.

Lets start adding to the previous example

## The Triangle Context <a name="the_triangle_context" id="the_triangle_context"></a>

Thankfully, drawing a triangle is relatively simple, so we can try to take things one step at a time. And the first is to encapsulate everything needed to draw one:

```c
typedef struct TriangleContext {
  SDL_GPUGraphicsPipeline* mPipeline;
} TriangleContext;
```

We actually only need a pipeline to draw a triangle, but as we proceed, we'll need much more than that, so we're going to structure this as if we were doing something more complex. 

Before we discuss creating one, we should go over what a pipeline is and how it fits into the rendering puzzle.

## Shaders and Pipelines <a name="shaders_pipelines" id="shaders_pipelines"></a>

When we look at how we do real-time rendering today, it's essentially a combination of generally tweakable fixed stages and programmable stages. We got a taste of this fixed functionality when we set-up and executed a RenderPass in the previous chapter. We were able to clear the screen just by tweaking the initialization and target of the RenderPass, with no need to submit geometry to proceed through the rest of the stages.

Pipelines are how we configure everything else not encapsulated in a RenderPass. You have to explain the layout of your data and how they flow from stage to stage, along with what you want the GPU to react to and modify data between and after the stages. A rough outline of the stages you'd typically see is the following:

```
+-----------------+    +-----------------+    +--------------------+ 
| Input Assembler | -> | Vertex Shader   | -> | Tesselation Shader | --+
+-----------------+    +-----------------+    +--------------------+   |
 +---------------------------------------------------------------------+
 |    +-----------------+  +---------------+  +---------------+
 + -> | Geometry Shader |  | Rasterization |  | Pixel Shaders |
      +-----------------+  +---------------+  +---------------+ 
```

We should first mention the Input Assembler which is something that's technically always happening, but we won't interact with much for some time. Essentially it takes data we describe in the Pipeline, and sets it up to be used in the subsequent stages.

All that said, SDL GPU doesn't give us access to every programmable stage you see above, many aren't relevant or performant today and some are too new to have been added for compatability reasons. What it does give us access to are the classics, Vertex Shaders and Fragment Shaders. It also has Compute Shaders to boot, but they're not part of the Render Pipeline, though we'll find they're still very useful for rendering later on. With regards to our concerns here, we'll write Vertex Shaders to output individual vertices, and Fragment Shaders to output colors.

> Note: If you've heard of Mesh Shaders, they effectively replace all of the geometry related stages. What we do in Vertex shaders would likely be sort of the base functionality of learning Mesh Shaders. That said, they're designed to be smarter around data management, letting you implement techniques that we'll have to split between Compute and Vertex shaders when we get there.

So if you're unfamiliar, when I say "programmable" I mean it literally. Shaders are programs that execute on your GPU. Each shader stage tends to have slightly different requirements, you can think about it like function signatures, although it's not a perfect analogy. In general they all have things they need to output, but they can optionally output additional information to be used in the next stage. In some cases that information may be interpolated, such as coordinates along the surface of a triangle. We'll get into the details later!

Each Graphics API takes a different "native" shader input, Vulkan takes SPIR-V, DirectX takes DXIL, and Metal takes MSL. I should mention that when you get into extensions or development only situations there's slightly more flexibility than that, but for our purposes this is good enough to think about. Each of these are an intermediate representation of your program, lying somewhere between the text you wrote in the language you chose, and machine code that can run on your GPU. When implementing the respective Graphics API you'll need to pass it the intermediate it expects. This is also true of SDL_GPU, in which you need declare which ones you can pass, and it can fail if there isn't a backend which can accept it. 

There's many languages specifically for shaders, and even general purpose programming languages that can be compiled to, but as stated in the intro, we'll be using HLSL as we have SDL_shadercross to use with well documented binding and set descriptions.


## Vertices, Briefly <a name="vertices" id="vertices"></a>

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

## The Vertex Shader <a name="vertex_shader" id="vertex_shader"></a>

Now that we've discussed the theory, let's get back to the code a bit, but before we can make a pipeline, we'll need to write both a Vertex and Pixel shader. We'll create a couple of files [`Triangle.vert.hlsl` and `Triangle.frag.hlsl`] to start working on, starting with the Vertex shader.

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

Now we've discussed that Vertex Shaders have to output geometry, we do this through returning a value from our `main` function which we'll be writing in a moment. That said, we do not need to return just a single value. Vertex Shaders can also output data for Pixel Shaders to use. In general these values will be interpolated based on the values of the vertices output by this function based on the position of the pixel within the triangle those vertices make up. We'll discuss this in a bit more detail during the FullScreen Triangle section below.

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

Right off the bat we see two interesting things, the return value is using our `Output` struct, and we're taking a paramter and it's marked with a built-in semantic. When we mark inputs with semantics, this means that we're asking the shader runtime to let us use that built-in variable. In this case `SV_VertexID` represents the ID of the vertex we're currently processing in this shader. Remember that the Vertex Shader will run for every vertex you request to draw. 


## Pixels (and Fragments) <a name="pixels" id="pixels"></a>

### The Fullscreen Triangle <a name="fullscreen_triangle" id="fullscreen_triangle"></a>

Now lets do something a little more practical, or at least on the borders of practical. You can copy and paste the TriangleContext you made above, and just create some new shaders to use with this FullScreenContext.

FullScreen triangles might be used for all sorts of things, but fundamentally they're generally there for when you just want a canvas to paint on in th Pixel Shader. We'll be doing something pretty basic here, but we'll revisit it much later. We're covering it now so that you can see that sometimes, you really do need to render a single triangle, but what's special is what's within it.

#### The Vertex Shader  <a name="fullscreen_triangle_vertex_shader" id="fullscreen_triangle_vertex_shader"></a>

We know from our work above that we don't actually need geometry from the CPU to output something in the vertex stage, as long as we know which vertex we're outputting. We also know that we can interpolate values between the vertices of the triangle. Thus, a fullscreen triangle is the minimum triangle that fills the entirety of the screen (NDC space) while giving us an interpolated value between (0, 0) and (1, 1) within the screen. That interpolated value is how we'll know where we are when we're inside the pixel shader, and will allow you to do some fun things, although my demonstration will be a simple checkerboard effect.

It'll end up looking something like this, albeit with everything outside of the screen being clipped off and not rendered past the vertex stage.

```
2
| \ 
|   \
|     \
|       \
|         \
|           \
|             \
|               \
+--------+--------\
|        |        | \
|        |(0,0)   |   \
|--------*--------|     \
|        |        |       \
|        |        |         \
|        |        |           \
0--------+---------------------1

```



