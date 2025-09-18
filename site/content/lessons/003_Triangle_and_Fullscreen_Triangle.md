---
title: Triangle and a "Fullscreen Triangle"
description: Finally you can see a triangle, and then a demonstration on why you might actually use a single triangle later on in your Graphics journey. Will cover the creation of Vertex and Pixel shaders, a simple demonstration of "Vertex Pulling", and then an extension of the triangle to cover the whole screen with a shader effect.
template: lesson_template.html
collections: ["lessons"]
---

The big one, it's time to draw a Triangle. And after that, we're going to learn a trick on how to extend this triangle to display over the entire screen. This technique is a small optimization that gets used when you start learning fullscreen effects. We'll be using it in the next chapter to play around with pixel shaders to get a little more comfortable with writing code on the GPU.

I figured it'd be nice to go over these together, because realistically, it's not common that you need to draw a single triangle on the screen. Maybe a Quad, two triangles making the shape of a Rectangle or Square, but that requires textures, transfer buffers, and bindings. That said, if you adjust the positions of that triangle? Suddenly it has a very practical purpose! And it's something we can learn about with only a few additional function calls without needing to get into everything needed to display a texture. Of course, don't worry, we'll get there. But let's build up step by step and try to learn something with each step!

It's finally time to talk about math and how the GPU works, at a high level at least. That said, this is a text heavy one, if you just want to work through the examples, I understand, and you can do some fun stuff at a deep level. Just know that to have a good understanding of these, you'll want to read these sections, or similar from other sources.

## Shaders and Pipelines <a name="shaders_pipelines" id="shaders_pipelines"></a>

When we look at how we do real-time rendering today, it's essentially a combination of generally tweakable fixed stages and programmable stages. We got a taste of this fixed functionality when we set-up and executed a RenderPass in the previous chapter. We were able to clear the screen just by tweaking the initialization and target of the RenderPass, with no need to submit geometry to proceed through the rest of the stages.

TDOD: Insert render pipeline diagram:
Input Assembler -> Vertex Shader -> Tesselation Shader -> Geometry Shader -> Rasterization -> Fragment Shader (Aka Pixel Shaders)

SDL GPU doesn't give us access to every programmable stage, many aren't relevant or performant today and some are too new to have been added for compatability reasons. What it does give us access to are the classics, Vertex Shaders and Fragment Shaders. It also has Compute Shaders to boot, but they're not part of the Render Pipeline, though we'll find they're still very useful for rendering later on. With regards to our concerns here, we'll write Vertex Shaders to output individual vertices, and Fragment Shaders to output colors.

> Note: If you've heard of Mesh Shaders, they effectively replace all of the geometry related stages. What we do in Vertex shaders would likely be sort of the base functionality of learning Mesh Shaders. That said, they're designed to be smarter around data management, letting you implement techniques that we'll have to split between Compute and Vertex shaders when we get there.

So if you're unfamiliar, when I say "programmable" I mean it literally. Shaders are programs that execute on your GPU. Each shader stage tends to have slightly different requirements, you can think about it like function signatures albeit it's not a perfect analogy. Each Graphics API takes a different "native" shader input, Vulkan takes SPIR-V, DirectX takes DXIL, and Metal takes MSL. I should mention that when you get into extensions or development only situations there's slightly more flexibility than that, but for our purposes this is good enough to think about. Each of these are an intermediate representation of your program, lying somewhere between the text you wrote in the language you chose, and machine code that can run on your GPU. When implementing the respective Graphics API you'll need to pass it the intermediate it expects. This is also true of SDL_GPU, in which you need declare which ones you can pass, and it can fail if there isn't a backend which can accept it. 

There's many languages specifically for shaders, and even general purpose programming languages that can be compiled to, but as stated in the intro, we'll be using HLSL as we have SDL_shadercross to use with well documented binding and set descriptions.

Lets start looking at something more concrete

## Vertices, Briefly <a name="vertices" id="vertices"></a>

For this chapter, to not _completely_ overload on information, we're only thinking about triangles, which are each made up of three Vertices, essentially positions on a 2D plane for now, that look _kind of_ like this:

```
typedef struct float2 {
    float x, y;
}
```

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

To display a triangle, we must produce geometry. For very simple shapes like Triangle, and even cubes, we can get away with storing this data within the shader itself. And it's helpful to learn this how to do this because this isn't unlike how you might pull geometry out of a storage buffer. Or for 2D you may store the transformations and UV info for sprites into a storage buffer but produce geometry like we will here.

Our first set of geometry will be a triangle that looks roughly like this:

```

(-1, 1)                       (1, 1)
    +---------------------------+
    |             0  (0, 1)     |
    |           /   \           |
    |         /       \         |
    |       /           \       |
    |     /               \     |
    |   /                   \   |
    | /                       \ |
    2---------------------------1
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

```hlsl
struct Output
{
  float3 Color : TEXCOORD1;
  float4 Position : SV_Position;
};
```

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


## Pixels (and Fragments) <a name="pixels" id="pixels"></a>

### The Fullscreen Triangle

Now lets do something a little more practical, or at least on the borders of practical. You can copy and paste the TriangleContext you made above, and just create some new shaders to use with this FullScreenContext.

FullScreen triangles might be used for all sorts of things, but fundamentally they're generally there for when you just want a canvas to paint on in th Pixel Shader. We'll be doing something pretty basic here, but we'll revisit it much later. We're covering it now so that you can see that sometimes, you really do need to render a single triangle, but what's special is what's within it.

#### The Vertex Shader

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



