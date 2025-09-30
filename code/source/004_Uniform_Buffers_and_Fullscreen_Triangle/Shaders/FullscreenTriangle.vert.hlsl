cbuffer UBO : register(b0, space1)
{
  float2 Offset;
};

struct Output
{
  float2 TextureCoordinates : TEXCOORD0;
  float2 Offset : TEXCOORD1;
  float4 Position : SV_Position;
};


Output main(uint id : SV_VertexID)
{
  Output output;
  output.TextureCoordinates = float2((id << 1) & 2, id & 2);
  output.Offset = Offset;
  output.Position = float4(output.TextureCoordinates * 2.0f + -1.0f, 0.0f, 1.0f);
  return output;
}
