---
title: The Cube and Math
description: We're not done learning about APIs that can help us in 2D, but it's time to start learning about 3D rendering. We'll be rendering a cube, and along the way learn about properly constructing a model matrix, and expand out our math library.
template: lesson_template.html
example_status: Finished
chapter_status: Outlined
collections: ["lessons"]
---

## Vertex and Index arrays in more detail

```hlsl
static const float3 cVertexPositions[8] = {
  { -1.0f,  1.0f, -1.0f },
  {  1.0f,  1.0f, -1.0f },
  { -1.0f, -1.0f, -1.0f },
  {  1.0f, -1.0f, -1.0f },
  { -1.0f,  1.0f,  1.0f },
  {  1.0f,  1.0f,  1.0f },
  { -1.0f, -1.0f,  1.0f },
  {  1.0f, -1.0f,  1.0f },
};

static const uint cVertexIndices[36] = {
  0, 1, 2, 1, 3, 2,
  5, 4, 7, 4, 6, 7,
  4, 5, 0, 5, 1, 0,
  2, 3, 6, 3, 7, 6,
  4, 0, 6, 0, 2, 6,
  1, 5, 3, 5, 7, 3,
};
```

## Front and Back Face Culling

```c
graphicsPipelineCreateInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;
graphicsPipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;
```

```
           4---------------------5
          /|                    /|
         / |      +y           / |
        /  |       |          /  |
       /   |       |         /   |
      /    |       |        /    |
     0-----|-------|-------1     |
     |     |       |       |     |
     |     |       | +z    |     |
     |     |       | /     |     |
     |     6-------|/------|-----7
-----|----/--------*-------|----/------+x
     |   /         |       |   /
     |  /          |       |  /
     | /           |       | /
     |/            |       |/
     2---------------------3
                   |
                   |
                   |
                   |

```


### Clockwise


```
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
```

## Matrices

Unlike with direct vector math, with matrices, most math is going to be done either with 3x3 for 2D operations or 4x4 for 3D operations. We'll show this math in 3D and thus 4x4 matrices.

#### Multiplication

Before going over the code and formula, we should make sure we understand how Matrix Multiplication works. Each element of a Matrix corresponds to the dot product between a row from one matrix, and a column from the other. We can then infer from that that the Matrices must match dimentionally somehow, so that we have enough elements .


$$\begin{array}{cc}
&
{\color{#b8860b}
B =
\begin{pmatrix}
5 & 6 & 7 \\
8 & 9 & 10
\end{pmatrix}}
\\
{\color{#228b22}
A =
\begin{pmatrix}
1 & 2 \\
3 & 4
\end{pmatrix}}
&
C =
\begin{pmatrix}
(1*5)+(2*8) & (1*6)+(2*9) & (1*7)+(2*10) \\
(3*5)+(4*8) & (3*6)+(4*9) & (3*7)+(4*10)
\end{pmatrix}
\end{array}$$

```c
float4x4 Float4x4_Multiply(const float4x4* aLeft, const float4x4* aRight)
{
  float4x4 toReturn;
  SDL_zero(toReturn);

  for (size_t i = 0; i < 4; ++i)
  {
    toReturn.data[i][0] =
      aLeft->data[0][0] * aRight->data[i][0] +
      aLeft->data[1][0] * aRight->data[i][1] +
      aLeft->data[2][0] * aRight->data[i][2] +
      aLeft->data[3][0] * aRight->data[i][3];

    toReturn.data[i][1] =
      aLeft->data[0][1] * aRight->data[i][0] +
      aLeft->data[1][1] * aRight->data[i][1] +
      aLeft->data[2][1] * aRight->data[i][2] +
      aLeft->data[3][1] * aRight->data[i][3];

    toReturn.data[i][2] =
      aLeft->data[0][2] * aRight->data[i][0] +
      aLeft->data[1][2] * aRight->data[i][1] +
      aLeft->data[2][2] * aRight->data[i][2] +
      aLeft->data[3][2] * aRight->data[i][3];

    toReturn.data[i][3] =
      aLeft->data[0][3] * aRight->data[i][0] +
      aLeft->data[1][3] * aRight->data[i][1] +
      aLeft->data[2][3] * aRight->data[i][2] +
      aLeft->data[3][3] * aRight->data[i][3];
  }

  return toReturn;
}
```

##### Matrix * Vector Multiplication


##### Matrix * Matrix Multiplication


#### Transformations

##### Identity

The identity matrix is generally a "base" matrix. Multiplying against it will return the same matrix you started with. It's simply a series of 1s along it's diagonal.

Math:

$$ \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp

float4x4 IdentityMatrix() {
  float4x4 toReturn;
  SDL_zero(toReturn);

  toReturn.data[0][0] = 1.0f;
  toReturn.data[1][1] = 1.0f;
  toReturn.data[2][2] = 1.0f;
  toReturn.data[3][3] = 1.0f;

  return toReturn;
}
```

##### Translation


Math:

$$ \begin{pmatrix} 1 & 0 & 0 & x \\ 0 & 1 & 0 & y \\ 0 & 0 & 1 & z \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 TranslationMatrix(float4 aPosition) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[3][0] = aPosition.x;
  toReturn.data[3][1] = aPosition.y;
  toReturn.data[3][2] = aPosition.z;

  return toReturn;
}
```
##### Scale

Math:

$$ \begin{pmatrix} x & 0 & 0 & 0 \\ 0 & y & 0 & 0 \\ 0 & 0 & z & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 ScaleMatrix(float4 aScale) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = aScale.x;
  toReturn.data[1][1] = aScale.y;
  toReturn.data[2][2] = aScale.z;

  return toReturn;
}
```

##### Rotation

###### Rotation about X

Math:

$$ \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & \cos{a} & -\sin{a} & 0 \\ 0 & \sin{a} & \cos{a} & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 RotationMatrixX(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[1][1] = SDL_cosf(aAngle);
  toReturn.data[1][2] = SDL_sinf(aAngle);
  toReturn.data[2][1] = -SDL_sinf(aAngle);
  toReturn.data[2][2] = SDL_cosf(aAngle);

  return toReturn;
}
```


###### Rotation about Y

Math:

$$ \begin{pmatrix} \cos{a} & 0 & \sin{a} & 0 \\ 0 & 1 & 0 & 0 \\ -\sin{a} & 0 & \cos{a} & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 RotationMatrixY(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = SDL_cosf(aAngle);
  toReturn.data[0][2] = -SDL_sinf(aAngle);
  toReturn.data[2][0] = SDL_sinf(aAngle);
  toReturn.data[2][2] = SDL_cosf(aAngle);

  return toReturn;
}
```


###### Rotation about Z

Math:

$$ \begin{pmatrix} \cos{a} & -\sin{a} & 0 & 0 \\ \sin{a} & \cos{a} & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 RotationMatrixZ(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = SDL_cosf(aAngle);
  toReturn.data[0][1] = SDL_sinf(aAngle);
  toReturn.data[1][0] = -SDL_sinf(aAngle);
  toReturn.data[1][1] = SDL_cosf(aAngle);

  return toReturn;
}
```

## Model Matrix

It's time to build out some matrix math functionality. If you're already comfortable with these topics, and prefer to just use your own, or copy/paste the one from this sample, that is entirely valid. 

> Note: The implementations here are naive and intended for learning, rather than high performance work. I intend to teach you how to do things effectively and ideally in a fairly performant manner in this series, but I'm not counting CPU cycles or planning to drop down into SIMD.

```c
float4x4 CreateModelMatrix(float4 aPosition, float4 aScale, float4 aRotation) {
  float4x4 translation = TranslationMatrix(aPosition);
  float4x4 rotation = RotationMatrix(aRotation);
  float4x4 scale = ScaleMatrix(aScale);

  float4x4 scale_rotation = Float4x4_Multiply(&rotation, &scale);
  return Float4x4_Multiply(&translation, &scale_rotation);
}

float4x4 CreateModelMatrixFromTransform(const Transform* aTransform) {
  return CreateModelMatrix(aTransform->mPosition, aTransform->mScale, aTransform->mRotation);
}
```

## Perspective Projection Matrix

```c
float4x4 PerspectiveProjectionLHZO(
  float aFovY,
  float aAspectRatio,
  float aNear,
  float aFar)
{
  float4x4 toReturn;
  SDL_zero(toReturn);

  const float focalLength = 1.0f / SDL_tan(aFovY * .5f);
  const float k = aFar / (aFar - aNear);

  toReturn.data[0][0] = focalLength / aAspectRatio;
  toReturn.data[1][1] = focalLength;
  toReturn.data[2][2] = k;
  toReturn.data[2][3] = 1.0f;
  toReturn.data[3][2] = -aNear * k;

  return toReturn;
}
```

## Transforming the Cube in the Vertex Shader

```hlsl
cbuffer UBO : register(b0, space1)
{
  float4x4 ObjectToWorld;
};

cbuffer UB1 : register(b1, space1)
{
  float4x4 WorldToNDC;
};

Output main(uint id : SV_VertexID)
{
  uint vertexIndex = cVertexIndices[id % 36];
  uint uvIndex = cVertexIndices[id % 6];

  Output output;
  float3 vertex = cVertexPositions[vertexIndex];
  float2 uv = cVertexPositions[uvIndex].xy;

  output.Position = mul(WorldToNDC, mul(ObjectToWorld, float4(vertex, 1.0f)));
  output.UV = (uv + 1.0f) * 0.5f;
  return output;
}
```

## Drawing the Cube

```c
float4x4 model = CreateModelMatrixFromTransform(&aContext->mUniform);
SDL_PushGPUVertexUniformData(aCommandBuffer, 0, &model, sizeof(model));
SDL_DrawGPUPrimitives(
  aRenderPass,
  6 /* 6 per face */ * 6 /* 6 sides of our cube */,
  1,
  0,
  0);
```
