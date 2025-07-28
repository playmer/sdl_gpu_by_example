
static const float3 cColors[2] =
{
    { 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
};


float4 main(float2 aTextureCoordinates : TEXCOORD0) : SV_Target0
{
    //return float4(cColors[(uint) floor(aTextureCoordinates.x) % 2], 1.0f);
    //return float4(cColors[0], 1.0f);
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
