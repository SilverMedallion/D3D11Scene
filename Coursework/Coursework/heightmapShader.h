#pragma once 

#include "DXF.h"
#include "CustomLight.h"

using namespace std;
using namespace DirectX;



class heightmapShader : public BaseShader
{
private:


	struct CameraBufferType
	{
		XMFLOAT3 cameraPositon;
		FLOAT padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

public:

	heightmapShader(ID3D11Device* device, HWND hwnd);
	~heightmapShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11SamplerState* sampleStateVS;

};

