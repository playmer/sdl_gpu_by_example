Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Output
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};


float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0;
    return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}

Output main(float2 uv : TEXCOORD0, float4 Position : SV_Position)
{
    Output output;
    output.Color = Texture.Sample(Sampler, uv);
    output.Depth = LinearizeDepth(Position.z, 20.0f, 60.0f);
    return output;
}
