//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

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
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(in VS_INPUT input)
{
    VS_OUTPUT output;// = (VS_OUTPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View );
    output.Pos = mul(output.Pos, Projection );

    output.Norm = mul(input.Norm, World);
    output.Norm = mul(output.Norm, View );
    output.Norm = mul(output.Norm, Projection );
    
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
