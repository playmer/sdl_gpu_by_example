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

TODO: Insert image of the SDL NDC

We'll go into _much_ greater detail on the Vertex pipeline, Vertex shaders, and techniques that can be used with them in a chapter or two. Right now though, these are the minimum details you'll need to get going.

## The Vertex Shader <a name="vertex_shader" id="vertex_shader"></a>

To display a triangle, we must produce geometry. For very simple shapes like Triangle, and even cubes, we can get away with storing this data within the shader itself. And it's helpful to learn this how to do this because this isn't unlike how you might pull geometry out of a storage buffer. Or for 2D you may store the transformations and UV info for sprites into a storage buffer but produce geometry like we will here.

```hlsl
static const float2 cVertexPositions[3] = {
    { 0.0f,  1.0f},
    { 1.0f, -1.0f},
    {-1.0f, -1.0f},
};
```

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

blah



## Vertices, Exhaustively <a name="vertices" id="vertices"></a>

As we'll discuss, a lot of the GPU is dedicated to processing geometry, and while this is a bit tweakable, in general this means Triangles. Speaking to SDL_GPU, there's also Lines and Points, but we'll touch on that below. You might hear people referring to "Quads", if they're referring to real time rendering this refers to two triangles that form a rectangle. Or they're perhaps longing for GPUs to support quads, alas, we don't live in that world, sorry 3D Artists.

TDDO: Insert image of a triangle, in 2D and 3D, with labels for point positions.

So this geometry we pass to the GPU is made of many "Vertices", when using a Triangle lists we're going to need 3 verts per triangle. In 2D, an individual vertex can be two numbers, for `{x, y}`. Somewhat obviously in 3D we need 3 numbers, due to the added `z` dimension. Also these are almost always stored as floats.

Lets discuss why it needs to be processed. If you're here, you've likely played games before, you know that plenty of things are moving around all the time. Now we could reupload the geometry every frame, and for many 2D games, that's can be acceptable, and there's optimizations we'll talk about later to make this sort of thing the ideal. If you're making a 3D game, this is almost never going to work unless you've really constrained your art. There's just too many verts in modern 3D models, even indie ones, it's not practical.

To add to this, GPUs also want to do as little work as possible. So they agressively "cull" triangles that are outside of the viewport, The Viewport by the way is something you can think of as the Swapchain we discussed in the last chapter, though in practice it's more abstract that that, we'll get there. Triangles fully outside of this Viewport will not proceed to be shaded by the Pixel/Fragment stage. 

So we can't rely on always being able to upload our verts so we need to manipulate them on the GPU, and we know they're checked against a viewport, how does this happen? Well the short answer...is math.

TODO: Insert image of the SDL NDC

It starts, or rather, ends here. All of our geometry and thus everything we shade must fit within this 2x2 square. And we'll use some relatively simple Linear Algebra to get there. Let us first discuss Coordinate Spaces.

TODO: Image describing the main coordinate spaces in graphics
Object Space -> World Space -> Camera Space -> NDC
             ^              ^               ^
 Transform Matrix     View Matrix    Projection Matrix

We're not going to go deep on the math involved here, that's better suited to a Linear Algebra text. You'll need to understand the basics of Points and how to transform them using Matrices










As for strips, they're just another way of passing verts so you can share them between other triangles. We tend to use index buffers for this as strips can be limiting.

