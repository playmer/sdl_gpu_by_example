---
title: Uniform Buffers and a Fullscreen Triangle
description: More interesting rendering will require learning buffers, we'll begin with the simplest type to use, Uniform Buffers. As well as an example of why you might actually use a single triangle later on in your Graphics journey.
template: lesson_template.html
example_status: Finished
chapter_status: Not Finished
collections: ["lessons"]
---

Now lets do something a little more practical, we're going to learn a trick on how to extend this triangle to display over the entire screen. This technique is a miniscule optimization and slight quality improvement that gets used when you start learning about fullscreen effects. The GPU has to compute one less vertex, and there's no chance of artifacting along the center diagonal line if we were using a quad. 

We'll also discuss Uniform Buffers and how to transfer data between the CPU and GPU, in future chapters we'll go over different types of buffers with many use cases.

## The Fullscreen Triangle

FullScreen triangles might be used for all sorts of things, but fundamentally they're generally there for when you just want a canvas to paint on in th Fragment Shader. We'll be doing something pretty basic here, but we'll revisit it much later. We're covering it now so that you can see that sometimes, you really do need to render a single triangle, but what's special is what's within it.

### The Vertex Shader 

We know from our work above that we don't actually need geometry from the CPU to output something in the vertex stage, as long as we know which vertex we're outputting. We also know that we can interpolate values between the vertices of the triangle. Thus, a fullscreen triangle is the minimum triangle that fills the entirety of the screen (NDC space) while giving us an interpolated value between (0, 0) and (1, 1) within the screen. That interpolated value is how we'll know where we are when we're inside the pixel shader, and will allow you to do some fun things, although my demonstration will be a simple checkerboard effect.

It'll end up looking something like this, albeit with everything outside of the screen being clipped off and not rendered past the vertex stage.

```
  (-1.0, 3.0) 0
  { 0.0, 2.0} | \ 
              |   \
              |     \
              |       \
              |         \
              |           \
              |             \
              |               \
  (-1.0, 1.0) +--------+--------\ (1.0, 1.0)
              |        |        | \
              |        |(0,0)   |   \
              |--------*--------|     \
              |        |        |       \
              |        |        |         \
              |        |        |           \
              2--------+--------+------------1
        (-1.0, -1.0)       (1.0, -1.0)   (3.0, -1.0)
        { 0.0f, 0.0}                     {2.0,  0.0}

```

We'll render these clockwise, and like last time, embed them into the shader. This time however, we'll also need to account for the texture coordinates of these vertices, you can see them below each vertex of the triangle in `{}`.

Texture coordinates have their own coordinate space, thankfully it's a rather simple one. Bottom left is (0, 0) and top right is (1, 1).

```
(0, 1)                  (1, 1)
    +----------------------+
    |                      |
    |                      |
    |                      |
    |                      |
    |                      |
    |                      |
    |                      |
    +----------------------+
(0, 0)                (1, 0)
```

```hlsl
static const float2 cVertexPositions[3] = {
  { -1.0f,  3.0f },
  {  3.0f, -1.0f },
  { -1.0f, -1.0f },
};

static const float2 cTextureCoordinates[3] = {
  { 0.0f, 2.0f },
  { 2.0f, 0.0f },
  { 0.0f, 0.0f },
};
``` 


```hlsl
struct Output
{
  float2 TextureCoordinates : TEXCOORD0;
  float4 Position : SV_Position;
};

Output main(uint id : SV_VertexID)
{
  uint vertexIndex = id % 3;

  Output output;
  output.Position = float4(cVertexPositions[vertexIndex], 0.0f, 1.0f);
  output.TextureCoordinates = cTextureCoordinates[vertexIndex];
  return output;
}
```

### Drawing an Oval with a Fragment Shader

## An Aside on Buffers

Almost everything we do in graphics requires data. Sometimes this can be generated or stored in-shader, we did the most simplistic version of this in the previous chapter to do pull-style vertex rendering. We just created some constant arrays and based on the VertexID, we were able to index into those arrays for our Vertex positions and colors. 

However, most of the time when doing rendering, you'll need to pass data from the CPU to the GPU, GPU to the CPU, or GPU to the GPU. When we start drawing entire models for example, we'll need to load that data from a file and put it somewhere the GPU can see and use. You might do compute based-culling, where the compute shader is determining what objects to render before getting to the vertex stage, that'll require the vertex data, the positional/rotation data, and a place for the compute shader to output the information needed to render or not render those objects in the vertex stage.

All that said, we want to ease our way into the concept so lets try Uniform Buffers as they're the easiest to get started with in SDL_GPU. In other APIs they're a bit more similar to Storage Buffers, which we'll also get to. 

### Uniform Buffers

We're going to start off with the example from the previous chapter, as we can demonstrate some simple uses of Uniform Buffers it's shaders. Our goals are going to be to move around the Triangle, and change the color of the the black squares in the checkerboard fullscreen effect.

We've not discussed it before in this tutorial, but you may have heard about structs containing "padding" between fields for alignment reasons. When you're just writing in one language, this mostly isn't something you need to concern yourself with, unless you're trying to optimize your layouts. When you're transferring data between languages, or in this case, between languages _and_ processors, you need to ensure both sides agree on the layout of structs and their padding.

{{collapsible-card "Covered in this Section"}}

 - [OpenGL Specification](https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159)
 - [Sub-section 2.15.3.1.2 - Standard Uniform Block Layout](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt)

{{collapsible-card-end}}


### Vertex Shader with a Uniform

To get data from the CPU, lets first decide what we want it to look like. We're rendering an oval on the center of the screen, so lets have it moved around. We'll take a Position, and use that to determine the center of the oval. To do that, we need to declare a Uniform Buffer:

```hlsl
cbuffer UBO : register(b0, space1)
{
  float2 Position;
};
```

Per the SDL_CreateGPUShader documentation, we know that within Vertex shaders, Uniform Buffers are going to be in `space1`, and we'll be placing this in slot 0 on both the CPU and GPU sides, hence the `b0`. Now there's a bunch of rules about alignment for these buffers, we don't have to worry about it for now, but just know that when you start passing float3s in, you need to treat it as if it were a float4 and pad it out, specifically this is called `std140` alignment. We'll deal with this situation in a later chapter as demonstration, but I'll link various documentation around this below.

All we'll have to do with this is pass it through to the fragment shader:

```hlsl
struct Output
{
  float2 TextureCoordinates : TEXCOORD0;
  float2 OvalPosition : TEXCOORD1;
  float4 Position : SV_Position;
};

Output main(uint id : SV_VertexID)
{
  uint vertexIndex = id % 3;

  Output output;
  output.Position = float4(cVertexPositions[vertexIndex], 0.0f, 1.0f);
  output.TextureCoordinates = cTextureCoordinates[vertexIndex];
  output.OvalPosition = OvalPosition;
  return output;
}
```

And you may remember that we talked a lot about interpolation in the last chapter, and you might wonder if the position of this Oval is. The answer is both yes and no. It is interpolated, but given it will be the same for each vertex, in effect it isn't, all runs of the fragment shader below will see the same position. You might also wonder why we don't just pass this directly into the fragment shader since it's always the same, and you're right, we could've. This is more for demonstration purposes of the interpolation effects here. 

### Fragment Shader with a Uniform

For our Fragment shader, we'll need to adjust our `main` to take in the value we passed in from the Vertex shader, but we'll also take a Uniform Buffer in here as well:

```
cbuffer UBO : register(b0, space3)
{
  float4 color;
};
```

Again we'll reference the documentation for `SDL_CreateGPUShader` to see that in Fragment shaders, uniforms are in `space3`, and the slots are different between shader stages, so we'll use `b0` here as well.



```hlsl
float4 main(float2 aTextureCoordinates : TEXCOORD0, float2 aOvalPosition : TEXCOORD1) : SV_Target0
{
  const float3 cColors[2] =
  {
    color.xyz,
    { 0.0f, 0.0f, 0.0f },
  };
  
  float distanceFromCenter = length(aTextureCoordinates - aOvalPosition);

  return distanceFromCenter < .1f ?
    float4(cColors[0], 1.0f) :
    float4(cColors[1], 1.0f);
}
```

