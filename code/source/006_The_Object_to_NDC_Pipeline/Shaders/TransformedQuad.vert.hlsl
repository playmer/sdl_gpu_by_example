static const float2 cVertexPositions[4] = {
    {-1.0f, -1.0f},
    { 1.0f,  1.0f},
    { 1.0f, -1.0f},
    {-1.0f,  1.0f},
};

static const uint cVertexIndices[6] = {
  0, 1, 2,
  0, 3, 1
};

struct Output
{
  float2 UV : TEXCOORD1;
  float4 Position : SV_Position;
};


struct ModelUniform
{
  float2 mPosition;
  float2 mScale;
};

cbuffer UBO : register(b0, space1)
{
  ModelUniform cModelUniform;
};

cbuffer UB1 : register(b1, space1)
{
    float4x4 WorldToNDC;
};

Output main(uint id : SV_VertexID)
{
  uint indicesIndex = id % 6;
  uint vertexIndex = cVertexIndices[indicesIndex];

  Output output;
  float2 vertex = cVertexPositions[vertexIndex];
  
  float x = cModelUniform.mPosition.x;
  float y = cModelUniform.mPosition.y;
  float w = cModelUniform.mScale.x;
  float h = cModelUniform.mScale.y;

  float4x4 ObjectToWorld = {
    { w / 2.f,     0.f, 0.f,   x },
    {     0.f, h / 2.f, 0.f,   y },
    {     0.f,     0.f, 1.f, 0.f },
    {     0.f,     0.f, 0.f, 1.f },
  };

  float4 t1 = mul(mul(WorldToNDC, ObjectToWorld), float4(vertex, 0.0f, 1.0f));

  output.Position = t1;
  output.UV = (vertex + 1.0f) * 0.5f;
  return output;
}
