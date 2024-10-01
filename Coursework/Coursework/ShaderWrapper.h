
#ifndef _SHADERWRAPPER_H_
#define _SHADERWRAPPER_H_


#include "DXF.h"
#include "CustomLight.h"

using namespace std;
using namespace DirectX;



class ShaderWrapper : public BaseShader
{
private: 

	
	struct CameraBufferType
	{
		XMFLOAT3 cameraPositon; 
		FLOAT padding; 
	};

	struct lightData
	{
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
		lightData lightsInfo[10]; 
	};

public: 

	ShaderWrapper(ID3D11Device* device, HWND hwnd);
	~ShaderWrapper();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, CustomLight *lights);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:

	ID3D11Buffer* matrixBuffer;
	
	ID3D11Buffer* cameraBuffer; 
	ID3D11SamplerState* sampleState; 
	ID3D11Buffer* multipleLightsBuffer; 

	ID3D11SamplerState* sampleStateVS;
	

};



#endif