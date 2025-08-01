
static const float3 cColors[2] =
{
    { 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
};


float4 main(float2 aTextureCoordinates : TEXCOORD0) : SV_Target0
{
    // We want easy values to manipulate into uints so we can do some indexing.
    // The NDC of Vulkan is (-1, -1) -> (1, 1), so we'll first add 1 to both components
    // which will bring it up to (0,0) -> (2,2). Then we'll multiply by 50 to get up to
    // (0,0) -> (100,100), some values a little friendlier if we need indexing.
    float2 friendlyCoodinates = (aTextureCoordinates.xy + float2(1.0f, 1.0f)) * 50.0f;
    
    // I need a diagram to explain why this works easily.
    uint colorIndex = ((uint) floor(friendlyCoodinates.x) + (uint) floor(friendlyCoodinates.y)) % 2;
    return float4(cColors[colorIndex], 1.0f);
}
