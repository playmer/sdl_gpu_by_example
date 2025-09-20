Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Output
{
    float4 Color : SV_Target0;
};

Output main(float2 uv : TEXCOORD0, float4 Position : SV_Position)
{
    Output output;
    output.Color = Texture.Sample(Sampler, uv);
    return output;
}
