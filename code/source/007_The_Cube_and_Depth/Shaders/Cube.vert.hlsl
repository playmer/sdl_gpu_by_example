


//    7----------------5                
//   /|               /|
//  / |              / |
// 3--+-------------1  |
// |  |             |  |
// |  |             |  |
// |  4-------------+--6
// | /              | /
// |/               |/
// 0----------------2

static const float3 cVertexPositions[8] = {
    {-1.0f, -1.0f,  1.0f}, // bottom left foward
    { 1.0f,  1.0f,  1.0f}, // top right forward
    { 1.0f, -1.0f,  1.0f}, // bottom right forward
    {-1.0f,  1.0f,  1.0f}, // top left forward
    {-1.0f, -1.0f, -1.0f}, // bottom left back
    { 1.0f,  1.0f, -1.0f}, // top right back
    { 1.0f, -1.0f, -1.0f}, // bottom right back
    {-1.0f,  1.0f, -1.0f}, // top left back
};

static const uint cVertexIndicies[36] = {
  // Front Face
  0, 1, 2,
  0, 3, 1,

  // Back Face
  6, 7, 4,
  6, 5, 7,

  // Top Face
  3, 5, 1,
  3, 7, 5,

  // Bottom Face
  4, 2, 6,
  4, 0, 2,

  // Left Face:
  4, 3, 0,
  4, 7, 3,

  // Right Face: 
  2, 5, 6,
  2, 1, 5,
};

struct Output
{
  float2 UV : TEXCOORD1;
  float4 Position : SV_Position;
};

cbuffer UBO : register(b0, space1)
{
  float4 Position;
  float4 Scale;
};

cbuffer UB1 : register(b1, space1)
{
    float4x4 WorldToNDC;
};

Output main(uint id : SV_VertexID)
{
  uint vertexIndex = cVertexIndicies[id % 36];
  uint uvIndex = cVertexIndicies[id % 6];

  Output output;
  float3 vertex = cVertexPositions[vertexIndex];
  float2 uv = cVertexPositions[uvIndex].xy;

  float4x4 ObjectToWorld = {
    {    Scale.x / 2.0f,           0.0f,           0.0f, Position.x },
    {              0.0f, Scale.y / 2.0f,           0.0f, Position.y },
    {              0.0f,           0.0f, Scale.z / 2.0f, Position.z },
    {              0.0f,           0.0f,           0.0f,       1.0f },
  };

  output.Position = mul(WorldToNDC, mul(ObjectToWorld, float4(vertex, 1.0f)));
  output.UV = (uv + 1.0f) * 0.5f;
  return output;
}
