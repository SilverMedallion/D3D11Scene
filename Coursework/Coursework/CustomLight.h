#pragma once  //felt pragma once is more appropriate than ifdef for a header guard but didn't want to change other classes based on examples just incase 

#include "DXF.h"	

using namespace DirectX; 

class CustomLight : public Light
{
public: 

	//setters
	void setConstantAttenuation(float attenuation);  //set constant attenuation for quadratic calcuation 
	void setLinearAttenuation(float attenuation);    //set linear attenuation for linear and quadratic calcuation 
	void setQuadraticAttenuation(float attenuation); //set quadratic attenuation for quadratic calculation; 
	void setState(float lightState);          //set value to check if light is on or off 
	void setType(float lightType);            //set value to check type of light 
	void setSpotAngle(float angle);  //set angle for spotlights

	//getters 
	float getConstantsAttenuation();  //get constant attenuation factor, returns float 
	float getLinearAttenuation();     //get linear attenuaion factor, returns float
	float getQuadraticAttenuation();  //get quadratic attenuation facotr, returns float
	float getState();           //get if light is turned on or not, retrns float, could have been bool but stops problems with bit lengths in buffer
	float getType();             //get value to represent what the light type is returns float
	float getSpotAngle();        //get value to represent the angel of the spotlight to determine how wide it is 

protected: 

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float state;   //1 means light is on, 0 means light is off  //may not need this as when adding lights for return value if diffuse is set to 0 then it will not affect the calculation anyway 
	float type; //if value is 1. directional light, if 2. point light, if 3 spot light. 
	float spotAngle; 

};

