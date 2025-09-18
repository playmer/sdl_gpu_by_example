struct Input
{
    float3 Position : TEXCOORD0;
    float3 Color : TEXCOORD1;
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

Output main(Input input)
{
  //uint vertexIndex = cVertexIndicies[id % 36];
  //uint uvIndex = cVertexIndicies[id % 6];
  //
  //Output output;
  //float3 vertex = cVertexPositions[vertexIndex];
  //float2 uv = cVertexPositions[uvIndex].xy;
  //
  //float4x4 ObjectToWorld = {
  //  {    Scale.x / 2.0f,           0.0f,           0.0f, Position.x },
  //  {              0.0f, Scale.y / 2.0f,           0.0f, Position.y },
  //  {              0.0f,           0.0f, Scale.z / 2.0f, Position.z },
  //  {              0.0f,           0.0f,           0.0f,       1.0f },
  //};
  //
  //output.Position = mul(WorldToNDC, mul(ObjectToWorld, float4(vertex, 1.0f)));
  //output.UV = (uv + 1.0f) * 0.5f;
  //return output;

  Output output;
  return output;
}

