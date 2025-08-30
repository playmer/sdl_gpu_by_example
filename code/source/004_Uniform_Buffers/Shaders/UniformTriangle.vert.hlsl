static const float2 cVertexPositions[3] = {
    { 0.0f,  1.0f},
    { 1.0f, -1.0f},
    {-1.0f, -1.0f},
};

static const float3 cColors[3] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
};

struct Output
{
  float3 Color : TEXCOORD1;
  float4 Position : SV_Position;
};

cbuffer UBO : register(b0, space1)
{
    float x;
    float y;
    float w;
    float h;
};

Output main(uint id : SV_VertexID)
{
  uint vertexIndex = id % 3;

  float3x3 translationScale = {
    {    w, 0.0f,    x },
    { 0.0f,    h,    y },
    { 0.0f, 0.0f, 1.0f },
  };

  Output output;
  output.Position = float4(mul(translationScale, float3(cVertexPositions[vertexIndex], 1.0f)).xy, 0.0f, 1.0f);
  output.Color = cColors[vertexIndex];
  return output;
}
