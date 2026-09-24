---
title: The Object To NDC Pipeline
description: We've been skirting around it, but it's time to talk in more detail about coordinate spaces and how we transform between them. We'll also be demonstraiting the Orthographic Projection so we can display objects in a more sensible space than a [-1, -1] to [1, 1] box.
template: lesson_template.html
example_status: Finished
chapter_status: Outlined
collections: ["lessons"]
---

So far we've been drawing within the "NDC" square. All of our coordinates have been within a square with a bottom left of [-1, -1] and top right of [1, 1], except of course with our Fullscreen shader, which was really just a way to render into the entirety of that box with less geometry. Left under-discussed was that geometry partially outside the NDC square gets clipped to only have the parts within the square, and geometry entirely outside of it don't proceed past the Vertex stage at all.

Now we can't actually prevent that our coordinates need to be generally within that box if we want to see it, it's a fundamental part of all graphics APIs, even if the box might be slightly different in each one. With some relatively simple Linear Algebra though, we'll be able to work in coordinate spaces that make more sense to us, and then transform our vertices to match NDC in our Vertex Shader. 

Generally in 2D we like to work in a space not entirely dissimilar to the NDC one, but we typically want each whole number to represent something relevant. Maybe it's pixels, or faux-pixels if we want a more Pixel Art aesthetic. It could be meters or yards, but regardless, a 2 by 2 square just isn't enough for us. In 3D it's usually similar, but with an added z coordinate, and the units are typically a unit of measure.

So in this chapter, lets revisit vertices, and discuss coordinate spaces and cameras.

## Vertices, Exhaustively

As we've discussed, a lot of the GPU is dedicated to processing geometry, and while this is a bit tweakable, in general this means Triangles. Speaking to SDL_GPU, there's also Lines and Points, but we'll touch on that below. You might hear people referring to "Quads", if they're referring to real time rendering this refers to two triangles that form a rectangle. Or they're perhaps longing for GPUs to support quads, alas, we don't live in that world, sorry 3D Artists.

TDDO: Insert image of a triangle, in 2D and 3D, with labels for point positions.

So this geometry we pass to the GPU is made of many "Vertices", when using a Triangle lists we're going to need 3 verts per triangle. In 2D, an individual vertex can be two numbers, for `{x, y}`. Somewhat obviously in 3D we need 3 numbers, due to the added `z` dimension. Also these are almost always stored as floats.

Lets discuss why it needs to be processed. If you're here, you've likely played games before, you know that plenty of things are moving around all the time. Now we could reupload the geometry every frame, and for many 2D games, that's can be acceptable, and there's optimizations we'll talk about later to make this sort of thing the ideal. If you're making a 3D game, this is almost never going to work unless you've really constrained your art. There's just too many verts in modern 3D models, even indie ones, it's not practical.

To add to this, GPUs also want to do as little work as possible. So they agressively "cull" triangles that are outside of the viewport, The Viewport by the way is something you can think of as the Swapchain we discussed in second chapter, though in practice it's more abstract that that, we'll get there. Triangles fully outside of this Viewport will not proceed to be shaded by the Pixel/Fragment stage. 

So we can't rely on always being able to upload our verts so we need to manipulate them on the GPU, and we know they're checked against a viewport, how does this happen? Well the short answer...is math.

TODO: Insert image of the SDL NDC

It starts, or rather, ends here. All of our geometry and thus everything we shade must fit within this 2x2 square. And we'll use some relatively simple Linear Algebra to get there. Let us first discuss Coordinate Spaces.

TODO: Image describing the main coordinate spaces in graphics
Object Space -> World Space -> Camera Space -> NDC
             ^              ^               ^
 Transform Matrix     View Matrix    Projection Matrix

We're not going to go deep on the math involved here, that's better suited to a Linear Algebra text. You'll need to understand the basics of Points and how to transform them using Matrices


## Coordinate Spaces

### Object

### World

### Camera

### NDC

NDC space is the Unit cuboid the GPU API uses for rendering. Anything outside of it gets clipped during the vertex stage. In SDL GPU, it's a Cuboid with the bottom-left-front point is {-1, -1, 0}, it's top-right-front point is {1, 1, 0}, and it extends into the Z dimention to 1 unit which is farther away from the screen.

INSERT DIAGRAM

## Transformations

### Object to World (Model Matrix)

### World to Camera (View Matrix)

### Camera to NDC (Projection Matrix)

Finally we need a matrix that maps from the Camera space to NDC space. 

## Projections

### Orthographic

### Perspective

## Orthographic Projection in Practice

Pull up your code from the last chapter, there might've been a lot to learn in this chapter, but the changes will be fairly minor.

### Matrix Representations, Column Major or Row Major (It's Column)

```c
typedef struct float4x4 {
  union
  {
    float data[4][4];
    float4 rows[4];
  };
} float4x4;
```

### The Orthographic Projection Matrix

```c
float4x4 OrthographicProjectionLHZO(
  float aLeft,
  float aRight,
  float aBottom,
  float aTop,
  float aNear,
  float aFar)
{
  float4x4 toReturn;
  SDL_zero(toReturn);

  toReturn.data[0][0] = 2.0f / (aRight - aLeft);
  toReturn.data[1][1] = 2.0f / (aTop - aBottom);
  toReturn.data[2][2] = 1.0f / (aFar - aNear);
  toReturn.data[3][0] = -(aRight + aLeft) / (aRight - aLeft);
  toReturn.data[3][1] = -(aTop + aBottom) / (aTop - aBottom);
  toReturn.data[3][2] = -aNear / (aFar - aNear);
  toReturn.data[3][3] = 1.0f;

  return toReturn;
}
```

### Object To World, in-shader

```hlsl
float x = cModelUniform.mPosition.x;
float y = cModelUniform.mPosition.y;
float w = cModelUniform.mScale.x;
float h = cModelUniform.mScale.y;

float4x4 ObjectToWorld = {
  { w / 2.f,     0.f, 0.f,   x },
  {     0.f, h / 2.f, 0.f,   y },
  {     0.f,     0.f, 1.f, cModelUniform.mDepth },
  {     0.f,     0.f, 0.f, 1.f },
};
```

### Camera To NDC, in-shader

```hlsl
cbuffer UB1 : register(b1, space1)
{
  float4x4 WorldToNDC;
};

float4 transformedPosition =
  mul(mul(WorldToNDC, ObjectToWorld), float4(position, 0.0f, 1.0f));
```

### Pixel-space Object Transforms

```c
context.mUniform[0].mPosition.x = 384.f;
context.mUniform[0].mPosition.y = 360.f;
context.mUniform[0].mScale.x = 256.f;
context.mUniform[0].mScale.y = 256.f;
context.mUniform[0].mDepth = 0.75f;

context.mUniform[1].mPosition.x = 640.f;
context.mUniform[1].mPosition.y = 360.f;
context.mUniform[1].mScale.x = 256.f;
context.mUniform[1].mScale.y = 256.f;
context.mUniform[1].mDepth = 0.25f;
```

### Updating the Projection for the Window

```c
gContext.WorldToNDC = OrthographicProjectionLHZO(
  0.f,
  (float)w,
  0.f,
  (float)h,
  0.f,
  1.f
);
```

### Uploading Object and Projection Matrices

```c
SDL_PushGPUVertexUniformData(
  aCommandBuffer,
  1,
  &gContext.WorldToNDC,
  sizeof(gContext.WorldToNDC));

SDL_PushGPUVertexUniformData(
  aCommandBuffer,
  0,
  &aContext->mUniform[0],
  sizeof(aContext->mUniform[0]));
SDL_DrawGPUPrimitives(aRenderPass, 6, 1, 0, 0);
```
