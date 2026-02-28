---
title: Uniform Buffers and a Fullscreen Triangle
description: More interesting rendering will require learning buffers, we'll begin with the simplest type to use, Uniform Buffers. As well as an example of why you might actually use a single triangle later on in your Graphics journey.
template: lesson_template.html
example_status: Finished
chapter_status: Not Finished
collections: ["lessons"]
---

We'll start with a little math lesson, and then follow it with something a little more practical, we're going to learn a trick on how to extend this triangle to display over the entire screen. This technique is a miniscule optimization and slight quality improvement that gets used when you start learning about fullscreen effects. The GPU has to compute one less vertex, and there's no chance of artifacting (visual distortions) along the center diagonal line if we were using a quad. 

We'll also discuss Uniform Buffers and how to transfer data between the CPU and GPU, in future chapters we'll go over different types of buffers with many use cases.

## A Brief Introduction to Vector Math.

While not intended as a replacement for the relevant math classes, or a math text, we'll be occasionally discussing the math needed for our operations. In the spirit of that, we'll be discussing some of the basics this chapter and filling out the appropriate structs and functions. Some of this material was discussed in the previous chapter, but we'll be going into it in more detail here.

### Fundamental Types

Rather than writing a library that's very concerned about the particulars of the mathematics here, we're going for the more typical pragmatic approach. This means we won't have separate types for Homogenous Coordinates, Points, Vectors, and whatever else I might be forgetting. We'll get into some of the details there later, but for now, we'll say that we need a way to represent points in 2D, 3D, and Homogeneous Coordinate Space. This ends up being 2 through 4 float structs:

```c
typedef struct float2 {
  float x, y;
} float2;

typedef struct float3 {
  float x, y, z;
} float3;

typedef struct float4 {
  float x, y, z, w;
} float4;
```

We could do all sorts of things in terms of making this a bit fancy, but they tend to be compiler extensions, or not universally work well in C++, so we'll refrain. float2 and float3 are relatively self explanatory, they operate in 2D and 3D space. They could be a point, just a place within that space, or a vector, a direction, with potentially a magnitude. Each of those possibilities could be broken up into their own type, but as I said, we'll be going the more pragmatic approach of calling them floatX for however many components they have. float4 is a bit more strange, we'll get into it later, but we use it with our transformation matrixes, although it also comes in handy to represent Color+Transparency (RGB+A).

> Note: For the sake of consistent terminology, we'll be referring to these floatX types in general as "vectors". It's the most common term for this despite it being wildly overloaded both in this field, and it's what basically every text will refer to them as. When we need to discuss a vector being a direction, with or without magnitude, we'll specify that. 

### How points and vectors are represented in coordinate spaces.


#### Points

Points are fundamentally just a position within the coordinate space you're working in. When we discuss a Vertex, fairly universally, one of their properties is that it contains a position, which is indeed a Point. We already saw this in the previous lesson, where the vertices of our triangle was really just 3 float2s. As alluded to, vertices eventually start to contain much more data than just their position. 

We often manipulate the positions in various ways using transformations we'll learn about in future lessons. The most common you have to look forward to are translations, scales, and rotations.


### Some basic operations

We'll be using these in the upcoming chapter, albeit in the shader, but we might as well briefly discuss how they work now, with motivating examples coming later. For the sake of brevity, I'll only show these implementations in float2, but they extend trivially into float3 and float4.

### Piecewise Operations

For these operations, we'll define them as piecewise operation between components. That is to say every `a[i]` gets operated against `b[i]` to create a resulting `c[i]`. We'll show both a math representation as well as code.

#### Addition

When only a translation is what you need, you typically reach for a piecewise addition, with one argument being a point, and the other being a direction and magnitude vector.

##### Math

$$ \begin{pmatrix} x_1 y_1 \end{pmatrix} + \begin{pmatrix} x_2 \\ y_2 \end{pmatrix} = \begin{pmatrix} x_1 + x_2 \\ y_1 + y_2 \end{pmatrix} $$

##### Code

```c
float2 Float2_Add(float2 aLeft, float2 aRight) {
  float2 toReturn = { aLeft.x + aRight.x, aLeft.y + aRight.y };
  return toReturn;
}
```

#### Subtraction

##### Math

$$ \begin{pmatrix} x_1 \\ y_1 \end{pmatrix} + \begin{pmatrix} x_2 \\ y_2 \end{pmatrix} = \begin{pmatrix} x_1 + x_2 \\ y_1 + y_2 \end{pmatrix} $$

##### Code

```c
float2 Float2_Subtract(float2 aLeft, float2 aRight) {
  float2 toReturn = { aLeft.x - aRight.x, aLeft.y - aRight.y };
  return toReturn;
}
```

#### Multiplication

Similarly to addition you might reach for a piecewise multiplication for scaling a point in multiple directions.

##### Math

$$ \begin{pmatrix} x_1 \\ y_1 \end{pmatrix} * \begin{pmatrix} x_2 \\ y_2 \end{pmatrix} = \begin{pmatrix} x_1 * x_2 \\ y_1 * y_2 \end{pmatrix} $$

##### Code

```c
float2 Float2_Multiply(float2 aLeft, float2 aRight) {
  float2 toReturn = { aLeft.x * aRight.x, aLeft.y * aRight.y };
  return toReturn;
}
```

### Scalar Operations

Similarly, it's not uncommon to want to apply a single scalar value, in our case a float, to all components, so we'll define some functions for that as well:


#### Addition

##### Math

$$ a + \begin{pmatrix} x \\ y \end{pmatrix} = \begin{pmatrix} a + x \\ a + y \end{pmatrix} $$

##### Code

```c
float2 Float2_Scalar_Add(float2 aLeft, float aRight) {
  float2 toReturn = { aLeft.x + aRight, aLeft.y + aRight };
  return toReturn;
}
```

#### Multiplication

##### Math

$$ a * \begin{pmatrix} x \\ y \end{pmatrix} = \begin{pmatrix} a * x \\ a * y \end{pmatrix} $$

##### Code

```c
float2 Float2_Scalar_Multiply(float2 aLeft, float aRight) {
  float2 toReturn = { aLeft.x * aRight, aLeft.y * aRight };
  return toReturn;
}
```

#### Division

##### Math

$$ a * \begin{pmatrix} x \\ y \end{pmatrix} = \begin{pmatrix} a * x \\ a * y \end{pmatrix} $$

##### Code

```c
float2 Float2_Scalar_Division(float2 aLeft, float aRight) {
  float2 toReturn = { aLeft.x / aRight, aLeft.y / aRight };
  return toReturn;
}
```

We'll be introducing more math as time goes on, so expect some more sections in future chapters covering new functions we need.

## The Fullscreen Triangle

FullScreen triangles might be used for all sorts of things, but fundamentally they're generally there for when you just want a canvas to paint on in the Fragment Shader. We'll be doing something pretty basic here, but we'll revisit it much later. We're covering it now so that you can see that sometimes, you really do need to render a single triangle, but what's special is what's within it.

### The Initial Vertex Shader 

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

So we'll make two arrays storing each of these.

```cpp
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

And our shader will otherwise look _nearly_ identical to the one from last chapter, this time just substituting in texture coordinates instead of color.

```cpp
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

This sets up our fragment shader to know where on the screen it's drawing by looking at the texture coordinates, and altering it's output based on that position. But we're not quite done yet, as we've got to discuss what we'll be doing in the fragment shader, and how we'll use uniform buffers in both the vertex and fragment shaders to make that happen. If you were just displaying a fullscreen texture, you might stop here, we'll demonstrate such a shader in the future.

### Drawing an Oval with a Fragment Shader

Fragment shaders can be used for a lot of techniques, which makes sense, they are the final arbiter control of pixel output. They can of course be used for things that don't go directly to the screen, but they're pretty fancy. If you've ever been on [shadertoy](https://www.shadertoy.com/) almost all the work of showing those images is done within this type of shader.

We're not going to be doing anything wild yet, but running a fullscreen shader like we're using the Fullscreen triangle lets us get our first taste of what we can do here. For now, we'll draw a simple oval in the Fragment shader, passing it's position through the vertex shader.

## An Aside on Buffers

Almost everything we do in graphics requires data. Sometimes this can be generated or stored in-shader, we did the most simplistic version of this in the previous chapter to do pull-style vertex rendering. We just created some constant arrays and based on the VertexID, we were able to index into those arrays for our Vertex positions and colors. 

However, most of the time when doing rendering, you'll need to pass data from the CPU to the GPU, GPU to the CPU, or GPU to the GPU. When we start drawing entire models for example, we'll need to load that data from a file and put it somewhere the GPU can see and use. You might do compute based-culling, where the compute shader is determining what objects to render before getting to the vertex stage, that'll require the vertex data, the positional/rotation data, and a place for the compute shader to output the information needed to render or not render those objects in the vertex stage.

All that said, we want to ease our way into the concept so lets try Uniform Buffers as they're the easiest to get started with in SDL_GPU. In other APIs their ergonomics are a bit more similar to Storage Buffers, which we'll also get to.

### Uniform Buffers

We're going to start off with the example from the previous chapter, as we can demonstrate some simple uses of Uniform Buffers it's shaders. Our goals are going to be to move around the Triangle, and change the color of the the black squares in the checkerboard fullscreen effect.

We've not discussed it before in this tutorial, but you may have heard about structs containing "padding" between fields for alignment reasons. When you're just writing in one language, this mostly isn't something you need to concern yourself with, unless you're trying to optimize your layouts. When you're transferring data between languages, or in this case, between languages _and_ processors, you need to ensure both sides agree on the layout of structs and their padding.

{{collapsible-card "Covered in this Section"}}

 - [OpenGL Specification](https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159)
 - [Sub-section 2.15.3.1.2 - Standard Uniform Block Layout](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt)

{{collapsible-card-end}}


### Vertex Shader with a Uniform

We'll cover the GPU side first by working on the shaders, and we'll want to first decide what we want the data should look like for the CPU to hand us. We're rendering an oval on the center of the screen, so lets be able to move it around. We'll take a Position, and use that to determine the center of the oval. To do that, we need to declare a Uniform Buffer:

```cpp
cbuffer UBO : register(b0, space1)
{
  float2 Position;
};
```

Per the SDL_CreateGPUShader documentation, we know that within Vertex shaders for DXBC/DXIL shaders (which are the semantics of the HLSL we're writing uses), Uniform Buffers are going to be in `space1`, and we'll be placing this in slot 0 on both the CPU and GPU sides, hence the `b0`. Now there's a bunch of rules about alignment for these buffers, we don't have to worry about it for now, but just know that when you start passing float3s in, you need to treat it as if it were a float4 and pad it out, specifically this is called `std140` alignment. We'll deal with this situation in a later chapter as demonstration, but I'll link various documentation around this below.

We don't need it in the vertex shader, so we'll pass it through to the fragment shader:

```cpp
...
struct Output
{
  ...
  float2 OvalPosition : TEXCOORD1;
  ...
};

Output main(uint id : SV_VertexID)
{
  ...
  output.OvalPosition = OvalPosition;
  ...
}
```

And you may remember that we talked a lot about interpolation in the last chapter, and you might wonder if the position of this Oval is. The answer is both yes and no. It is interpolated, but given it will be the same for each vertex, in effect it isn't, all runs of the fragment shader below will see the same position. You might also wonder why we don't just pass this directly into the fragment shader since it's always the same, and you're right, we could've. This is more for demonstration purposes of the fact that uniform buffers are different between the stages, as we'll also be using one below. 

### Fragment Shader with a Uniform

For our Fragment shader, we'll also take a Uniform Buffer, this time to adjust the color of the oval we'll draw. Again we'll reference the documentation for `SDL_CreateGPUShader` to see that in Fragment shaders, uniforms are in `space3`, and the slot numbers are different between shader stages, so we'll can use `b0` here as well.

```cpp
cbuffer UBO : register(b0, space3)
{
  float4 color;
};
```

For our `main`, we'll need to adjust our parameters to take in the new position value we passed in from the Vertex Shader. Then we can calculate if our fragment is drawing our oval, or the "background".

```cpp
float4 main(float2 aTextureCoordinates : TEXCOORD0, float2 aOvalPosition : TEXCOORD1) : SV_Target0
{
  const float3 cColors[2] =
  {
    color.xyz,
    { 0.0f, 0.0f, 0.0f },
  };
  
  float distanceFromFragment = length(aTextureCoordinates - aOvalPosition);

  return distanceFromFragment < .1f ?
    float4(cColors[0], 1.0f) :
    float4(cColors[1], 1.0f);
}
```

Since we're just trying to draw an oval, we can simply calculate the distance between the fragment/pixel (represented by `aTextureCoordinates`) we're drawing to and the position of the oval. Once we do that, we need to see how big the oval is, we'll choose 0.1f as it'll be a nice portion of the screen given we're drawing in NDC. If our distance is less than that length, then we're within the oval and should select it's color, here that's `cColors[0]`, which we take in via our uniform buffer.

