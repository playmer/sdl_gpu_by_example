struct Input
{
  float3 Position : TEXCOORD0;
  float3 Normal : TEXCOORD1;
  float4 Tangent : TEXCOORD2;
};

struct Output
{
  float2 TextureCoordinates : TEXCOORD0;
  float3 Color : TEXCOORD1;
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
  output.TextureCoordinates = (input.Position.xy + 1.0f) * 0.5f;
  output.Color = input.Normal;
  return output;
}

