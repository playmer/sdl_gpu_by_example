struct Output
{
    float2 UV : TEXCOORD0;
    float4 Position : SV_Position;
};

Output main(uint id : SV_VertexID)
{
    Output output;
    output.UV = float2((id << 1) & 2, id & 2);
    output.Position = float4(output.UV * 2.0f + -1.0f, 0.0f, 1.0f);
    return output;
}
 