#include "ShaderWrapper.h"

ShaderWrapper::ShaderWrapper(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"VertexShader.cso", L"PixelShader.cso");
}

ShaderWrapper::~ShaderWrapper()
{
	//release the sampler state
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0; 
	}

	//release the vs sampler state
	if (sampleStateVS)
	{
		sampleStateVS->Release();
		sampleStateVS = 0;
	}
	
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

	

	//release the multiple lights constant buffer
	if (multipleLightsBuffer)
	{
		multipleLightsBuffer->Release();
		multipleLightsBuffer = 0;
	}


	//release the camera buffer 
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();	

}

void ShaderWrapper::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{


	D3D11_BUFFER_DESC matrixBufferDesc; 
	D3D11_SAMPLER_DESC samplerDesc; 

	D3D11_BUFFER_DESC cameraBufferDesc; 
	D3D11_BUFFER_DESC multipleLightsBufferDesc; 
	D3D11_SAMPLER_DESC samplerVSDesc;

	//Load (+compile) shader files and store results in HRESULT's for error checking
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	//setup the description of the dynaminc matrix constant bufer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	//Create the constant buffer pointer so we can accesss the vertex shader constant buffer from within this class
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	//create a texture sampler state description
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);


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


	//setup buffer to handle multiple lights 
	multipleLightsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multipleLightsBufferDesc.ByteWidth = sizeof(MultipleLightsBufferType);
	multipleLightsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multipleLightsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multipleLightsBufferDesc.MiscFlags = 0;
	multipleLightsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&multipleLightsBufferDesc, NULL, &multipleLightsBuffer);

	//setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
}


void ShaderWrapper::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture,  CustomLight* lights)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	//matrices to hold the transposed world, view and projection matrices 
	XMMATRIX tworld, tview, tproj;

	//transpose the matrices to prepare them for the shader 
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	//lock the constant buffer so it can be written to 
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	//now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);



	//send multiple lights data to pixel shader 
	MultipleLightsBufferType* multipleLightsPtr; 
	deviceContext->Map(multipleLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	multipleLightsPtr = (MultipleLightsBufferType*)mappedResource.pData;
    for (int i = 0; i < 10; ++i)           //NOTE come back and change later to calculate the size of the array and loop for number of elements
	{
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



	//send camera data to the vertex shader
	/*CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData; 
	cameraPtr.

	deviceContext->Unmap(cameraBuffer, 0); 
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);
	*/

	//set shader texture resource in the vertex shader.
	deviceContext->VSSetShaderResources(0, 1, &texture);
	deviceContext->VSSetSamplers(0, 1, &sampleStateVS);


	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);


}


