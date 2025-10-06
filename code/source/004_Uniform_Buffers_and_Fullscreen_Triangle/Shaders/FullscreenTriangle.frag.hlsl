cbuffer UBO : register(b0, space3)
{
  float4 color;
};

float4 main(float2 aTextureCoordinates : TEXCOORD0, float2 aOvalPosition : TEXCOORD1) : SV_Target0
{
  const float3 cColors[2] =
  {
    color.xyz,
    { 0.0f, 0.0f, 0.0f },
  };
  
  float distanceFromCenter = length(aTextureCoordinates - aOvalPosition);

  return distanceFromCenter < .1f ?
    float4(cColors[0], 1.0f) :
    float4(cColors[1], 1.0f);
}
