#include "FSQuad.h"

FSQuad::FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height)
{
	this->width = width;
	this->height = height;
	//init shaders
	pVSFxaa.Init(L"VSFXAA.cso", pDevice);
	pPSFxaa.Init(L"PSFXAA.cso", pDevice);

	//vertices
	vertices.push_back(vertex(XMFLOAT2(-1, 1)));
	vertices.push_back(vertex(XMFLOAT2(1, 1)));
	vertices.push_back(vertex(XMFLOAT2(-1, -1)));
	vertices.push_back(vertex(XMFLOAT2(1, -1)));

	//create vertex buffer
	pVertexBuffer = std::make_unique<VertexBuffer<vertex>>();
	pVertexBuffer->Init(pDevice, vertices.data(), vertices.size());

	//create index buffer
	std::vector<signed int> indices = { 0,1,2,1,3,2 };
	pIndexBuffer = std::make_unique<IndexBuffer>();
	pIndexBuffer->Init(pDevice, indices, indices.size());

	//constant buffer initialize
	fxaaCBuffer = std::make_unique<CBuffer<fxaa>>();
	fxaaCBuffer->Init(pDevice, pContext);
}

void FSQuad::draw(ID3D11DeviceContext* pContext)
{
	//vertex and index buffer bind
	pVertexBuffer->Bind(pContext);
	pIndexBuffer->Bind(pContext);
	//bind fxaa shaders
	pContext->IASetInputLayout(0);
	pVSFxaa.Bind(pContext);
	pPSFxaa.Bind(pContext);
	//fxaa cbuffer values bind
	fxaaCBuffer->data.rcpFrame = XMFLOAT4(1.0f / width, 1.0f / height, 0.0f, 0.0f);
	fxaaCBuffer->MapData();
	fxaaCBuffer->PSBind(pContext, 0, 1);
	//draw quad
	pContext->DrawIndexed(pIndexBuffer->BufferSize(),
		0, 0);
}

