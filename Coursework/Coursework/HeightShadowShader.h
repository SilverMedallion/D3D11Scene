
#pragma once
#include "DXF.h"
#include "CustomLight.h"
using namespace std;
using namespace DirectX;


class HeightShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		
	};

	

	struct lightData
	{
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT3 direction;
		FLOAT specularPower;
		XMFLOAT4 specular;
		XMFLOAT3 position;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
		float state;   //1 means light is on, 0 means light is off             //may not need this as when adding lights for return value if diffuse is set to 0 then it will not affect the calculation anyway 
		float type; //if value is 1. directional light, if 2. point light, if 3 spot light. 
		float spotAngle;
		XMFLOAT3 padding;
	};

	//multiple light implementation: 
	struct MultipleLightsBufferType
	{
		lightData lightsInfo[4];
	};

public:

	HeightShadowShader(ID3D11Device* device, HWND hwnd);
	~HeightShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ShadowMap* depthMap[4], CustomLight lights[4]);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateVS;
	ID3D11SamplerState* sampleStateShadow;
	
	ID3D11Buffer* multipleLightsBuffer;

};

