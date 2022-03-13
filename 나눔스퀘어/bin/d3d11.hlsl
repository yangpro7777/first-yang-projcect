struct PixelShaderInput
{
    float4 pos         : SV_POSITION;
    float2 texCoord    : TEXCOORD0;
};

Texture2D<float>  luminanceChannel : t0;
Texture2D<float2> chrominanceChannel : t1;
SamplerState      defaultSampler     : s0;

// Derived from https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
// Section: Converting 8-bit YUV to RGB888
static const float3x3 YUVtoRGBCoeffMatrix =
{
    1.164383f,  1.164383f, 1.164383f,
    0.000000f, -0.391762f, 2.017232f,
    1.596027f, -0.812968f, 0.000000f
};

float3 ConvertYUVtoRGB(float3 yuv)
{
    // Derived from https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
    // Section: Converting 8-bit YUV to RGB888

    // These values are calculated from (16 / 255) and (128 / 255)
    yuv -= float3(0.062745f, 0.501960f, 0.501960f);
    yuv = mul(yuv, YUVtoRGBCoeffMatrix);

    return saturate(yuv);
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PixelShaderInput input) : SV_Target
{
    float y = luminanceChannel.Sample(defaultSampler, input.texCoord);
    float2 uv = chrominanceChannel.Sample(defaultSampler, input.texCoord);

    return float4(ConvertYUVtoRGB(float3(y, uv)), 1.f);
}
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

// Per-vertex data used as input to the vertex shader.
struct VertexInput
{
    float3 pos : POSITION0;
    float2 texCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VertexShaderOutput VS(VertexInput input)
{
    VertexShaderOutput output;
    float4 pos = float4(input.pos, 1.f);
    output.pos = float4(pos);
    output.texCoord = input.texCoord;
    return output;
}
