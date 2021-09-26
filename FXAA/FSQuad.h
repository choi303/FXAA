#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include "Shaders.h"
#include "CBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

using namespace DirectX;
class FSQuad
{
private:

	struct fxaa
	{
		XMFLOAT4 rcpFrame;
	};
public:
	FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height);
	FSQuad(FSQuad& rhs) = default;
	~FSQuad() = default;

	void draw(ID3D11DeviceContext* pContext);
private:
	struct vertex
	{
		XMFLOAT2 pos;
	};
	std::vector<vertex> vertices;
	std::unique_ptr<VertexBuffer<vertex>> pVertexBuffer;
	std::unique_ptr<IndexBuffer> pIndexBuffer;
	std::unique_ptr<CBuffer<fxaa>> fxaaCBuffer;
	VertexShader pVSFxaa;
	PixelShader pPSFxaa;
	int width;
	int height;
};

