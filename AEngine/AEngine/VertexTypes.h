#pragma once 

#include<xnamath.h>

struct PosNorm
{
    XMFLOAT3 pos;
    XMFLOAT3 norm;
};

struct PosNormUV
{
    XMFLOAT3 pos;
    XMFLOAT3 norm;
    XMFLOAT2 uv;
};