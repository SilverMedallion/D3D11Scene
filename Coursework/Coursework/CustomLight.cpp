#include "CustomLight.h"

void CustomLight::setConstantAttenuation(float attenuation)
{
	constantAttenuation = attenuation; 
}

void CustomLight::setLinearAttenuation(float attenuation)
{
	linearAttenuation = attenuation; 
}

void CustomLight::setQuadraticAttenuation(float attenuation)
{
	quadraticAttenuation = attenuation; 
}

void CustomLight::setState(float lightState)
{
	if (lightState != 0 && lightState != 1) {
		MessageBox(nullptr, L"Please enter a valid value for light status! 1 for light on or 0 for light off.", L"Error", MB_OK | MB_ICONERROR);
		//return;
	}
	else {
	state = lightState; 
	}
}

void CustomLight::setType(float lightType)
{
	if (lightType != 1 && lightType != 2 && lightType != 3)	{
		MessageBox(nullptr, L"Please enter a valid value for light type! 1 for directional. 2 for point. 3 for spot", L"Error", MB_OK | MB_ICONERROR);
	}
	else {
		type = lightType; 
	}
}

void CustomLight::setSpotAngle(float angle)
{
	spotAngle = angle; 
}

float CustomLight::getConstantsAttenuation()
{
	return constantAttenuation;
}

float CustomLight::getLinearAttenuation()
{
	return linearAttenuation;
}

float CustomLight::getQuadraticAttenuation()
{
	return quadraticAttenuation;
}

float CustomLight::getState()
{
	return state;
}

float CustomLight::getType()
{
	return type;
}

float CustomLight::getSpotAngle()
{
	return spotAngle; 
}


