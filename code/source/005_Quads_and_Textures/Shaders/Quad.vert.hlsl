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
  uint indiciesIndex = id % 6;
  uint vertexIndex = cVertexIndicies[indiciesIndex];
  
  float2 vertex = cVertexPositions[vertexIndex];
  float2 scaledVertex = float2(vertex.x * cModelUniform.mScale.x, vertex.y * cModelUniform.mScale.y);
  float2 translatedVertex = float2(
    scaledVertex.x + cModelUniform.mPosition.x,
    scaledVertex.y + cModelUniform.mPosition.y
  );
  
  Output output;
  output.Position = float4(translatedVertex, 0.0f,1.0f);
  output.UV = (vertex + 1.0f) * 0.5f;
  return output;
}
