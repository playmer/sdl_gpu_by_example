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

Output main(uint id : SV_VertexID)
{
  uint indicesIndex = id % 6;
  uint vertexIndex = cVertexIndices[indicesIndex];
  
  float2 position = cVertexPositions[vertexIndex];
  float2 scaledPosition = position * cModelUniform.mScale;
  float2 transformedPosition = scaledPosition + cModelUniform.mPosition;
  
  Output output;
  output.Position = float4(transformedPosition, 0.0f, 1.0f);
  output.UV = (position + 1.0f) * 0.5f;
  return output;
}
