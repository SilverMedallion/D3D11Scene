// depth shader.cpp
#include "HeightDepthshader.h"

HeightDepthShader::HeightDepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"heightDepth_vs.cso", L"heightDepth_ps.cso");
}

HeightDepthShader::~HeightDepthShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//release the vs sampler state
	if (sampleStateVS)
	{
		sampleStateVS->Release();
		sampleStateVS = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void HeightDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerVSDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// create a texture sampler state description for second sample for VS
	samplerVSDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerVSDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerVSDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerVSDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerVSDesc.MipLODBias = 0.0f;
	samplerVSDesc.MaxAnisotropy = 1;
	samplerVSDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerVSDesc.MinLOD = 0;
	samplerVSDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerVSDesc, &sampleStateVS);


}

void HeightDepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// set shader texture resource in the vertex shader.
	deviceContext->VSSetShaderResources(0, 1, &texture);
	deviceContext->VSSetSamplers(0, 1, &sampleStateVS);
}
