
// An rough representation of the cube we're using as well as the object space it's within.
// 
//           4---------------------5
//          /|                    /|
//         / |      +y           / |
//        /  |       |          /  |
//       /   |       |         /   |
//      /    |       |        /    |
//     0-----|-------|-------1     |
//     |     |       |       |     |
//     |     |       | +z    |     |
//     |     |       | /     |     |
//     |     6-------|/------|-----7
//-----|----/--------*-------|----/------+x
//     |   /         |       |   /
//     |  /          |       |  /
//     | /           |       | /
//     |/            |       |/
//     2---------------------3
//                   |
//                   |
//                   |
//                   |
//                   |
//                   |
//
//



static const float3 cVertexPositions[8] = {
    /* 0 */ { -1.0f,  1.0f, -1.0f }, // top left forward
    /* 1 */ {  1.0f,  1.0f, -1.0f }, // top right forward
    /* 2 */ { -1.0f, -1.0f, -1.0f }, // bottom left foward
    /* 3 */ {  1.0f, -1.0f, -1.0f }, // bottom right forward
    /* 4 */ { -1.0f,  1.0f,  1.0f }, // top left back
    /* 5 */ {  1.0f,  1.0f,  1.0f }, // top right back
    /* 6 */ { -1.0f, -1.0f,  1.0f }, // bottom left back
    /* 7 */ {  1.0f, -1.0f,  1.0f }, // bottom right back
};

static const uint cVertexIndices[36] = {
  // Front Face
  0, 1, 2,
  1, 3, 2,

  // Back Face
  5, 4, 7,
  4, 6, 7,

  // Top Face
  4, 5, 0,
  5, 1, 0,

  // Bottom Face
  2, 3, 6,
  3, 7, 6,

  // Left Face:
  4, 0, 6,
  0, 2, 6,

  // Right Face: 
  1, 5, 3,
  5, 7, 3,
};

struct Output
{
  float2 UV : TEXCOORD1;
  float4 Position : SV_Position;
};

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
