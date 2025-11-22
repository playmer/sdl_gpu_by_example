Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Output
{
  float4 Color : SV_Target0;
};

Output main(float2 aTextureCoordinates : TEXCOORD0, float3 aColor : TEXCOORD1)
{
  Output output;
  output.Color = float4(aColor, 1.0f);
  return output;
}
