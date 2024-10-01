
#include "App1.h"

App1::App1()
{
    //initialize objects to nullptr 
	//avoid issues if init is called multiple times or any other possible errors in the init process
	//also useful for debugging to can check if they are still set to nulllptr 
	planeMesh = nullptr;
	cubeMesh = nullptr; 
	pointLightDebugMesh = nullptr; 
	shadowShader = nullptr; 
	heightShadowShader = nullptr; 
	depthShader = nullptr; 
	heightDepthShader = nullptr; 
	horizontalBlurShader = nullptr;
	verticalBlurShader = nullptr; 
	bloomShader = nullptr;
	postProcessShader = nullptr; 

}


void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	//initialise geometry 
	planeMesh = new PlaneMesh (renderer->getDevice(), renderer->getDeviceContext());
	cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	

	//create sphere mesh for light debug
	pointLightDebugMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext()); 

	//Load textures 
	textureMgr->loadTexture(L"height", L"res/height.png");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");
	
	//initialise shaders 
	
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	heightDepthShader = new HeightDepthShader(renderer->getDevice(), hwnd); 
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	heightShadowShader = new HeightShadowShader(renderer->getDevice(), hwnd); 
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	bloomShader = new BloomShader(renderer->getDevice(), hwnd);
	postProcessShader = new PostProcessShader(renderer->getDevice(), hwnd);


	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// initialise shadow maps
	for (int i = 0; i < maxNumberOfLights; i++)
	{
	shadowMap[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}

	//initialise render to textures 
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	shadowTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//set initial camera position 
	camera->setPosition(15.f, 3.f, -30.f);

	//set default light valeus 
	for (int i = 0; i < maxNumberOfLights; i++) 
	{
	//lights[i] = new CustomLight(); 
		lights[i].setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f); //white light
		lights[i].setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f); //no ambient light currently 
		lights[i].setDirection(0.0f, -1.0f, 0.0f);           //set light to be pointing down
		lights[i].setSpecularPower(15);                    //create a broad highlight
		lights[i].setSpecularColour(0.8f, 0.8f, 0.8f, 1.0f); // creaste a white-ish specular highlight
		lights[i].setPosition(0.0f, 40.0f, 0.0f);    //each point light will be 1 unit above the last 
		lights[i].setConstantAttenuation(0.5f);             
		lights[i].setLinearAttenuation(0.125);
		lights[i].setQuadraticAttenuation(0.0f);
		lights[i].setState(float(i == 0));               //set 1 light on       //0 to off 1 to on 
		lights[i].setType(3.0f);                   //remember 1 directional, 2 point, 3 spot
		lights[i].setSpotAngle(1.57f);              
		lights[i].generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);//shadows are not working exactly right for spot light as shadows are using
		//orhtographic projection instead of perpective projection which means that as the light chagnes distance from the object the shadow will not change size as it 
		//should 
    }


}


App1::~App1()
	{
		// Run base application destructor
		BaseApplication::~BaseApplication();

		// Release the dynamcally allocated direct3D objects
		//avoids memeory leaks 

		if (planeMesh)
		{
			delete planeMesh;
			planeMesh = 0;
		}


		if (cubeMesh)
		{
			delete cubeMesh;
			cubeMesh = 0;
		}

		if (orthoMesh)
		{
			delete orthoMesh;
			orthoMesh = 0;
		}

		if (pointLightDebugMesh)
		{
			delete pointLightDebugMesh;
			pointLightDebugMesh = 0;
		}

		
		//release shaders
		
		if (depthShader)
		{
			delete depthShader;
			depthShader = 0;
		}

		if (heightDepthShader)
		{
			delete heightDepthShader;
			heightDepthShader = 0;
		}

		if (shadowShader)
		{
			delete shadowShader;
			shadowShader = 0;
		}

		if (heightShadowShader)
		{
			delete heightShadowShader;
			heightShadowShader = 0;
		}

		if (horizontalBlurShader)
		{
			delete horizontalBlurShader;
			horizontalBlurShader = 0;
		}

		if (verticalBlurShader)
		{
			delete verticalBlurShader;
			verticalBlurShader = 0;
		}

		if (bloomShader)
		{
			delete bloomShader;
			bloomShader = 0;
		}

		if (postProcessShader)
		{
			delete postProcessShader;
			postProcessShader = 0;
		}

		// Release shadow maps.
		for (int i = 0; i < maxNumberOfLights; i++)
		{
			if (shadowMap[i])
			{
				delete shadowMap[i];
				shadowMap[i] = 0;
			}
		}

		// Release render textures.
		if (horizontalBlurTexture)
		{
			delete horizontalBlurTexture;
			horizontalBlurTexture = 0;
		}

		if (verticalBlurTexture)
		{
			delete verticalBlurTexture;
			verticalBlurTexture = 0;
		}

		if (bloomTexture)
		{
			delete bloomTexture;
			bloomTexture = 0;
		}

		if (shadowTexture)
		{
			delete shadowTexture;
			shadowTexture = 0;
		}
	}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}


void App1::depthPass()
{
	
	// get the world, view, and projection matrices from the camera and d3d objects.
	for (int i = 0; i < maxNumberOfLights; i++)
	{
	    shadowMap[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
		lights[i].generateViewMatrix();
		XMMATRIX lightViewMatrix = lights[i].getViewMatrix();
		XMMATRIX lightProjectionMatrix = lights[i].getOrthoMatrix();
		//changing projection to perspective for spotlight 
		XMMATRIX lightPerspectiveMatrix = XMMatrixPerspectiveFovLH(lights[i].getSpotAngle(), 1.0f, 0.1f, 100.0f);   //need to comback and change this to lights attenuation for far clip
		XMMATRIX worldMatrix = renderer->getWorldMatrix();

		//render geometry, shoudl be same as first pass other an using depth shaders here/////////////////////////////////
		// Render plane floor
		worldMatrix = XMMatrixTranslation(heightTranslationX, heightTranslationY, heightTranslationZ);
		planeMesh->sendData(renderer->getDeviceContext());
		heightDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"height"));
		heightDepthShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();

		// Render shape with simple lighting shader set.
		worldMatrix = XMMatrixTranslation(cubeTranslationX, cubeTranslationY, cubeTranslationZ);
		cubeMesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
	}


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::firstPass()
{
	
	shadowTexture->setRenderTarget(renderer->getDeviceContext());
	shadowTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//Render scene with shadows to a render texture to be combined with bloom 
	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//render geometry, should be the same as depth pass but using shadow shader here ///////////////////////
	// Render floor
	worldMatrix = XMMatrixTranslation(heightTranslationX, heightTranslationY, heightTranslationZ);
	planeMesh->sendData(renderer->getDeviceContext());
	heightShadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"height"), shadowMap, lights);
	heightShadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());
	worldMatrix = renderer->getWorldMatrix();

	// Render shape with shadow 
	worldMatrix = XMMatrixTranslation(cubeTranslationX, cubeTranslationY, cubeTranslationZ);
	cubeMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap, lights);
	shadowShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());
	worldMatrix = renderer->getWorldMatrix();
	/////////////////////////////////////////////////////////


	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::bloomPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)bloomTexture->getTextureWidth();
	bloomTexture->setRenderTarget(renderer->getDeviceContext());
	bloomTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = bloomTexture->getOrthoMatrix();

	// Render the bright parts of the scene 
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, shadowTexture->getShaderResourceView(), screenSizeX);
	bloomShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::horizontalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, bloomTexture->getShaderResourceView(), screenSizeX);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::verticalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::finalPass()
{

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	postProcessShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView(), shadowTexture->getShaderResourceView());
	postProcessShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	
}


bool App1::render()
{
	//shadowstuff
	depthPass();

	//render scene to render to texture 
	firstPass(); 

	//extract bright areas from the sceen to have blur applied to them 
	bloomPass();

	// Apply horizontal blur stage
	horizontalBlur();

	// Apply vertical blur to the horizontal blur stage
	verticalBlur();

	//Combine shadow render texture and bloom render texture
	finalPass(); 

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}


void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//build UI for controlling multiplle lights
	for (int i = 0; i < maxNumberOfLights; i++)
	{
		std::string windowName = "light Control " + std::to_string(i);
		ImGui::Begin(windowName.c_str());    //creates a new window for each light 

		// Ensure unique labels for each control in the window
		std::string labelPrefix = "Light " + std::to_string(i) + " ";


		//ImGui functions used here to control the light values return true when their values are changed so when changed using the interface
		// they return true go run the setters for the light variables 
		// Diffuse Color
		XMFLOAT4 diffuse = lights[i].getDiffuseColour();
		std::string diffuseLabel = labelPrefix + "Diffuse Color";
		if (ImGui::ColorEdit4(diffuseLabel.c_str(), (float*)&diffuse)) {
			lights[i].setDiffuseColour(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
		}

		// Ambient Color
		XMFLOAT4 ambient = lights[i].getAmbientColour();
		std::string ambientLabel = labelPrefix + "Ambient Color";
		if (ImGui::ColorEdit4(ambientLabel.c_str(), (float*)&ambient)) {
			lights[i].setAmbientColour(ambient.x, ambient.y, ambient.z, ambient.w);
		}

		// Specular Color
		XMFLOAT4 specular = lights[i].getSpecularColour();
		std::string specularLabel = labelPrefix + "Specular Color";
		if (ImGui::ColorEdit4(specularLabel.c_str(), (float*)&specular)) {
			lights[i].setSpecularColour(specular.x, specular.y, specular.z, specular.w);
		}

		// Specular Power
		float specularPower = lights[i].getSpecularPower();
		std::string specularPowerLabel = labelPrefix + "Specular Power";
		if (ImGui::SliderFloat(specularPowerLabel.c_str(), &specularPower, 0.0f, 100.0f)) {
			lights[i].setSpecularPower(specularPower);
		}

		// Direction for directional lights and spot lights 
		if (lights[i].getType() == 1.0f || lights[i].getType() == 3.0f) { 
			XMFLOAT3 direction = lights[i].getDirection();
			std::string directionLabel = labelPrefix + "Direction";
			if (ImGui::SliderFloat3(directionLabel.c_str(), (float*)&direction, -1.0f, 1.0f)) {
				lights[i].setDirection(direction.x, direction.y, direction.z);
			}
		}

		// Position 
		
			XMFLOAT3 position = lights[i].getPosition();
			std::string positionLabel = labelPrefix + "Position";
			if (ImGui::SliderFloat3(positionLabel.c_str(), (float*)&position, -100.0f, 100.0f)) {
				lights[i].setPosition(position.x, position.y, position.z);
			}
		

		// Constant Attenuation
		float constantAttenuation = lights[i].getConstantsAttenuation();
		std::string constAttLabel = labelPrefix + "Constant Attenuation";
		if (ImGui::SliderFloat(constAttLabel.c_str(), &constantAttenuation, 0.0f, 1.0f)) {
			lights[i].setConstantAttenuation(constantAttenuation);
		}

		// Linear Attenuation
		float linearAttenuation = lights[i].getLinearAttenuation();
		std::string linAttLabel = labelPrefix + "Linear Attenuation";
		if (ImGui::SliderFloat(linAttLabel.c_str(), &linearAttenuation, 0.0f, 1.0f)) {
			lights[i].setLinearAttenuation(linearAttenuation);
		}

		// Quadratic Attenuation
		float quadraticAttenuation = lights[i].getQuadraticAttenuation();
		std::string quadAttLabel = labelPrefix + "Quadratic Attenuation";
		if (ImGui::SliderFloat(quadAttLabel.c_str(), &quadraticAttenuation, 0.0f, 1.0f)) {
			lights[i].setQuadraticAttenuation(quadraticAttenuation);
		}

		// State
		float state = lights[i].getState();
		bool stateBool = state > 0.0f; // Convert float to bool
		std::string stateLabel = labelPrefix + "Light On/Off";
		if (ImGui::Checkbox(stateLabel.c_str(), &stateBool)) {
			lights[i].setState(stateBool ? 1.0f : 0.0f); // Convert bool back to float
		}

		// Type
		float type = lights[i].getType();
		const char* types[] = { "Directional", "Point", "Spot" };
		int typeIndex = static_cast<int>(type) - 1; // need to -1 since start at 0
		std::string typeLabel = labelPrefix + "Light Type";
		if (ImGui::Combo(typeLabel.c_str(), &typeIndex, types, IM_ARRAYSIZE(types))) {
			lights[i].setType(static_cast<float>(typeIndex + 1)); // Convert back to 1 starting position 
		}

		// Spot Angle
		float spotAngle = lights[i].getSpotAngle();
		std::string spotAngleLabel = labelPrefix + "Spot Angle";
		if (ImGui::SliderAngle(spotAngleLabel.c_str(), &spotAngle, 1.0f, 179.0f)) {
			lights[i].setSpotAngle(spotAngle);
		}
		ImGui::End(); 

	
	}

	ImGui::ShowDemoWindow(); 

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

