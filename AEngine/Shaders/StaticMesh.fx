//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D textDiffuse : register(t0);
SamplerState samplerAF : register(s0);

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    float4x4 BoneTransforms[100];
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Norm : NORMAL;
    float2 texCoord : TEXCOORD0;

};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Norm : NORMAL;
    float2 texCoord : TEXCOORD0;
    uint4  BoneIndices : BONEINDICES;
    float3 weights : WEIGHTS;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(in VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 worldViewProj = mul(mul(World,View),Projection);
    float4 position = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float lastWeight = 0.0f;
    int n = 3;
    uint4 boneIndices = input.BoneIndices;
    float3  boneWeights = input.weights;

    for(int i = n; i > 0; i--)
    {
        lastWeight += boneWeight[i];
        position += weights[i] * mul(BoneTransforms[boneIndices[i]], position);
    }

    lastWeight = 1.0f - lastWeight;
    position += lastWeight * mul(BoneTransforms[boneIndices[0]],position);


    output.Pos = mul(worldViewProj, position);
    output.Norm = mul(input.Norm,worldViewProj);
    output.texCoord = input.texCoord;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 sampleColor;
    sampleColor = textDiffuse.Sample(samplerAF, input.texCoord);
    return sampleColor;
}
