struct Input
{
  float3 Position : TEXCOORD0;
  float3 Color : TEXCOORD1;
};

struct Output
{
  float3 Color : TEXCOORD0;
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

Output main(Input input)
{
  Output output;
  output.Position = mul(WorldToNDC, mul(ObjectToWorld, float4(input.Position, 1.0f)));
  output.Color = input.Color;
  return output;
}

