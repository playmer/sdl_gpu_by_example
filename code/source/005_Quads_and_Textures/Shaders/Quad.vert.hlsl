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

cbuffer UBO : register(b0, space1)
{
    float x;
    float y;
    float w;
    float h;
};

Output main(uint id : SV_VertexID)
{
  uint indiciesIndex = id % 6;
  uint vertexIndex = cVertexIndicies[indiciesIndex];

  // float3x3 translationScale = {
  //   {    w, 0.0f,    x },
  //   { 0.0f,    h,    y },
  //   { 0.0f, 0.0f, 1.0f },
  // };
  // output.Position = float4(mul(translationScale, float3(cVertexPositions[vertexIndex], 1.0f)).xy, 0.0f, 1.0f);

  Output output;
  float2 vertex = cVertexPositions[vertexIndex];
  float2 scaledVertex = float2(vertex.x * w, vertex.y * h);
  float2 translatedVertex = float2(scaledVertex.x + x, scaledVertex.y + y);
  output.Position = float4(translatedVertex, 0.0f,1.0f);
  output.UV = (vertex + 1.0f) * 0.5f;
  return output;
}
