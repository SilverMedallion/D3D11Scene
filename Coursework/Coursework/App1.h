// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "CustomLight.h"
#include "DepthShader.h"
#include "HeightDepthShader.h"
#include "ShadowShader.h"
#include "HeightShadowShader.h"
#include "BloomShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "PostProcessShader.h"



class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();


protected:

	bool render();
	void depthPass();  //generate depth information for objects 
	void firstPass();  //render the scene with lighting and shadows to render to texture 
	void bloomPass();  //extract bright parts from scene render to textuer 
	void horizontalBlur(); //apply blur to bright areas
	void verticalBlur();   //apply blur to bright areas 
	void finalPass();      //apply shadow map to scene and bloom effect 
	void gui();

private:

	//pointers are used for shaderwrapper and the new keyword is used to alloacte memory for objects on the heap
	//gives control over object's lifetime as objects will still exist when we go out of scope until delete is called
	DepthShader* depthShader;
	HeightDepthShader* heightDepthShader; 
	ShadowShader* shadowShader;
	HeightShadowShader* heightShadowShader; 
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;
	BloomShader* bloomShader;
	PostProcessShader* postProcessShader;

	//Array of shadowmaps for multiple lights 
	ShadowMap* shadowMap[4]; 

	//render texturers for multiple passes 
	RenderTexture* horizontalBlurTexture; 
	RenderTexture* verticalBlurTexture; 
	RenderTexture* bloomTexture; 
	RenderTexture* shadowTexture; 

	//orthomesh to apply render to textures to 
	OrthoMesh* orthoMesh; 

	//geometry 
	PlaneMesh* planeMesh; 
	SphereMesh* pointLightDebugMesh; 
	CubeMesh* cubeMesh; 


	
	//array's are technically pointers so when passed in instead of passing the entire array we pass in a pointer to the first position in memory.
	//if we added a "*" in the same way as other objects we woudl declare an array of 10 POINTERS to CustomLights not a single pointer to an array of 10 CustomLights
	CustomLight lights[4]; 
	int maxNumberOfLights = 4;

	//varaibles used to be applied to the world matrix before rendering the cube. must be stored at floats to be passed in to XMMatrixTranslation 
	float cubeTranslationX = 21.0f, cubeTranslationY = 10.0f, cubeTranslationZ = -14.0f;
	float heightTranslationX = -50.0f, heightTranslationY = -1.0f, heightTranslationZ = -50.0f; 
	
	//values for perpective matrix generation 
	float aspectRatio = 1.0f;

	float nearClip = 0.1f;
	float farClip = 100.0f; 
};

#endif