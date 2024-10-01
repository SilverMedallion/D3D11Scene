// texture shader.cpp
#include "shadowshader.h"


ShadowShader::ShadowShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"shadow_vs.cso", L"shadow_ps.cso");
}


ShadowShader::~ShadowShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (multipleLightsBuffer)
	{
		multipleLightsBuffer->Release();
		multipleLightsBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ShadowShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC multipleLightsBufferDesc;

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

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup light buffer
	//setup buffer to handle multiple lights 
	multipleLightsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multipleLightsBufferDesc.ByteWidth = sizeof(MultipleLightsBufferType);
	multipleLightsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multipleLightsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multipleLightsBufferDesc.MiscFlags = 0;
	multipleLightsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&multipleLightsBufferDesc, NULL, &multipleLightsBuffer);

}


void ShadowShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ShadowMap* depthMap[4], CustomLight lights[4])
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	MultipleLightsBufferType* multipleLightsPtr;

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

	//Additional
	// Send light data to pixel shader
	deviceContext->Map(multipleLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	multipleLightsPtr = (MultipleLightsBufferType*)mappedResource.pData;
	for (int i = 0; i < 4; ++i)
	{
		XMMATRIX tLightViewMatrix = XMMatrixTranspose(lights[i].getViewMatrix());
		XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(lights[i].getOrthoMatrix());
		multipleLightsPtr->lightsInfo[i].lightView = tLightViewMatrix;
		multipleLightsPtr->lightsInfo[i].lightProjection = tLightProjectionMatrix;

		multipleLightsPtr->lightsInfo[i].diffuse = lights[i].getDiffuseColour();
		multipleLightsPtr->lightsInfo[i].ambient = lights[i].getAmbientColour();
		multipleLightsPtr->lightsInfo[i].direction = lights[i].getDirection();
		multipleLightsPtr->lightsInfo[i].specularPower = lights[i].getSpecularPower();
		multipleLightsPtr->lightsInfo[i].specular = lights[i].getSpecularColour();
		multipleLightsPtr->lightsInfo[i].position = lights[i].getPosition();
		multipleLightsPtr->lightsInfo[i].constantAttenuation = lights[i].getConstantsAttenuation();
		multipleLightsPtr->lightsInfo[i].linearAttenuation = lights[i].getLinearAttenuation();
		multipleLightsPtr->lightsInfo[i].quadraticAttenuation = lights[i].getQuadraticAttenuation();
		multipleLightsPtr->lightsInfo[i].state = lights[i].getState();
		multipleLightsPtr->lightsInfo[i].type = lights[i].getType();
		multipleLightsPtr->lightsInfo[i].spotAngle = lights[i].getSpotAngle();
		multipleLightsPtr->lightsInfo[i].padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	deviceContext->Unmap(multipleLightsBuffer, 0);

	deviceContext->PSSetConstantBuffers(1, 1, &multipleLightsBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &multipleLightsBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	for (int i = 0; i < 4; i++)
	{
		//
		ID3D11ShaderResourceView* shadowMap = depthMap[i]->getDepthMapSRV();
		deviceContext->PSSetShaderResources(1 + i, 1, &shadowMap);

	}	
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}

