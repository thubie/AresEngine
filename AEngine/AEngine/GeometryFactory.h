#pragma once

#include<Windows.h>
#include<xnamath.h>
#include<D3D11.h>
#include<D3DX11.h>

class GeometryFactory
{
public:
    GeometryFactory();
    GeometryFactory(const GeometryFactory& other);
    ~GeometryFactory();

    void Initialize();
    void Shutdown();


};