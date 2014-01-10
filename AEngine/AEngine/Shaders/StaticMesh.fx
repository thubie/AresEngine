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
    float4 weights : WEIGHTS;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(in VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 worldViewProj = mul(mul(World,View),Projection);
	float4x4 BoneTransform;
    float4 position = input.Pos;
    
	uint4 boneIndices = input.BoneIndices;
    float4 boneWeights = input.weights;
	
	BoneTransform = boneWeights[0] * BoneTransforms[boneIndices[0]];
	BoneTransform +=  boneWeights[1] * BoneTransforms[boneIndices[1]];
	BoneTransform +=  boneWeights[2] * BoneTransforms[boneIndices[2]];
	BoneTransform +=  boneWeights[3] * BoneTransforms[boneIndices[3]];
	
	position = mul(BoneTransform, position);
	
    output.Pos = mul(position, World);
    output.Pos = mul(output.Pos, View );
    output.Pos = mul(output.Pos, Projection );
    output.Norm = mul(input.Norm, World);
    output.Norm = mul(output.Norm, View );
    output.Norm = mul(output.Norm, Projection );
	output.texCoord = input.texCoord;

	//Working shader code
	/* VS_OUTPUT output;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View );
    output.Pos = mul(output.Pos, Projection );
    output.Norm = mul(input.Norm, World);
    output.Norm = mul(output.Norm, View );
    output.Norm = mul(output.Norm, Projection );
	output.texCoord = input.texCoord; */

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
