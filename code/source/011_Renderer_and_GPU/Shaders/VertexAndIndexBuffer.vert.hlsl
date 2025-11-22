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
  float4x4 WorldToCamera;
};

cbuffer UBO : register(b1, space1)
{
  float4x4 ObjectToWorld;
};

cbuffer UB1 : register(b2, space1)
{
  float4x4 CameraToNDC;
};

Output main(Input input)
{
  Output output;
  output.Position = mul(CameraToNDC, mul(WorldToCamera, mul(ObjectToWorld, float4(input.Position, 1.0f))));
  //output.Position = mul(CameraToNDC, mul(ObjectToWorld, float4(input.Position, 1.0f)));
  output.Color = input.Color;
  return output;
}

