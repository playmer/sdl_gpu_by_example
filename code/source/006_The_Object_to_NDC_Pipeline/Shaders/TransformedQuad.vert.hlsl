static const float2 cVertexPositions[4] = {
    {-1.0f, -1.0f},
    { 1.0f,  1.0f},
    { 1.0f, -1.0f},
    {-1.0f,  1.0f},
};

static const uint cVertexIndicies[6] = {
  0, 1, 2,
  0, 3, 1
};

struct Output
{
  float2 UV : TEXCOORD1;
  float4 Position : SV_Position;
};

cbuffer UBO : register(b0, space1)
{
    float x;
    float y;
    float w;
    float h;
};


cbuffer UB1 : register(b1, space1)
{
    float4x4 WorldToNDC;
};

Output main(uint id : SV_VertexID)
{
  uint indiciesIndex = id % 6;
  uint vertexIndex = cVertexIndicies[indiciesIndex];

  Output output;
  float2 vertex = cVertexPositions[vertexIndex];

  float4x4 ObjectToWorld = {
    { w / 2.0f,     0.0f, 0.0f,    x },
    {     0.0f, h / 2.0f, 0.0f,    y },
    {     0.0f,     0.0f, 1.0f, 0.0f },
    {     0.0f,     0.0f, 0.0f, 1.0f },
  };
    
  //float4 t1 = mul(mul(ObjectToWorld, WorldToNDC), float4(vertex, 0.0f, 1.0f));
  //float4 t1 = mul(mul(float4(vertex, 0.0f, 1.0f), WorldToNDC), ObjectToWorld);
    float4 t1 = mul(mul(WorldToNDC, ObjectToWorld), float4(vertex, 0.0f, 1.0f));

  output.Position = t1;
  output.UV = (vertex + 1.0f) * 0.5f;
  return output;
}
