static const float2 cVertexPositions[3] = {
  { -1.0f,  3.0f },
  {  3.0f, -1.0f },
  { -1.0f, -1.0f },
};

static const float2 cTextureCoordinates[3] = {
  { 0.0f, 2.0f },
  { 2.0f, 0.0f },
  { 0.0f, 0.0f },
}; 

cbuffer UBO : register(b0, space1)
{
  float2 OvalPosition;
};

struct Output
{
  float2 TextureCoordinates : TEXCOORD0;
  float2 OvalPosition : TEXCOORD1;
  float4 Position : SV_Position;
};

Output main(uint id : SV_VertexID)
{
  uint vertexIndex = id % 3;

  Output output;
  output.Position = float4(cVertexPositions[vertexIndex], 0.0f, 1.0f);
  output.TextureCoordinates = cTextureCoordinates[vertexIndex];
  output.OvalPosition = OvalPosition;
  return output;
}
