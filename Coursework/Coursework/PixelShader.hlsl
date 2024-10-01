//texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);


struct lightData
{
    float4 diffuse;        
    float4 ambient;
    float3 direction;
    float specularPower;
    float4 specular;
    float3 position;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float state; //1 means light is on, 0 means light is off             //may not need this as when adding lights for return value if diffuse is set to 0 then it will not affect the calculation anyway 
    float type; //if value is 1. directional light, if 2. point light, if 3 spot light. 
    float spotAngle;
    float3 padding; 
};
 
//buffer contains an array of structs that contain information about each light
cbuffer MultipleLightsBuffer : register(b1)
{
    lightData lightsInfo[4];
}


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0; 
    float3 normal : NORMAL; 
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2; 
};



float4 calculateSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	//blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}


//modified for multiple lights 
//Calculate lighting intensity based on angle between normal and light vector
float4 calculateDirectionalLightLighting(float3 lDirection, float3 pNormal, float4 lDiffuse, float4 lAmbient)
{
    float intensity = saturate(dot(pNormal, lDirection));      //saturate is used so that if the angle is less than 0, 0 will still be returned 
    float4 colour = lAmbient + saturate(lDiffuse * intensity); //ambient colour added here, remember need to pass it in to make sure we are getting the data for the correct light
    return colour;
}

//modified for multiple lights
float4 calculatePointLightLighting(float3 lightVec, float3 pixelNormal, float4 lightDif, float4 lAmbient)
{
    float intensity = saturate(dot(pixelNormal, lightVec));
    float4 colour = lAmbient + saturate(lightDif * intensity);
    return colour;
}

float4 calculateSpotLightLighting(float3 lightVec, float3 pixelNormal, float4 lightDif, float4 lightAmbient, float3 spotDirection, float spotAngle)
{
    // Calculate the dot product between the light direction and the spot direction
    float spotFactor = dot(normalize(lightVec), normalize(-spotDirection));

    // Check if the pixel is within the spot angle
    if (spotFactor > cos(spotAngle / 2))
    {
        float intensity = saturate(dot(pixelNormal, lightVec));
        float4 colour = lightAmbient + saturate(lightDif * intensity);
        return colour;
    }
    else
    {
        // Pixel is outside the spot angle, so no contribution
        return float4(0, 0, 0, 1);
    }
}

//modified to work for multiple lights
float calculateAttenuation(float distance, float constantA, float linearA, float quadraticA, float4 ldiffuse)
{
    float attenuation = 1 / (constantA + (linearA * ldiffuse) + (quadraticA * pow(distance, 2)));
    return attenuation;
}


float4 main(InputType input) : SV_TARGET
{

    float4 textureColour;
    float distance;
    float lightAttenuation;
    float3 lightVector;
    float4 lightColour;
    float4 specularColour; 
    
    //calcualte light direction for point lights and store as light vector 
    /*
    lightVector = normalize(lightPosition - input.worldPosition); 
    
    textureColor = texture0.Sample(Sampler0, input.tex);
    
    
    
    lightColour = calculateDirectionalLightLighting(-lightDirection, input.normal, diffuseColour); 
    
    //for a point light instead of the lights direction we use the lightVector which is the geometry position minus the light position, which is different for eveypixel  
    lightColour = calculatePointLightLighting(-lightVector, input.normal, diffuseColour);
    
    specularColour = calculateSpecular(-lightDirection, input.normal, input.viewVector, specular, specularPower);
    
    return lightColour * textureColor ;
    
    
    */

   // from this point to return textureColor * textureColor; is lab 4 stuff will use maybe later 
   // 
    

    //the commented code chunk was for one light the code chunk after this is addpted for multiple lights
    /*  
    //sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour
    textureColour = texture0.Sample(Sampler0, input.tex);
    
    //calculate distance between the light and the fragment
    distance = length(position - input.worldPosition);
    
    //set attenuation based on quadratic attenuation 
    lightAttenuation = calculateAttenuation(distance);
    
    //normalise distance to create appropriate light vector
    lightVector = normalize(position - input.worldPosition);

    //set light colour based on lighting calculation
    lightColour = calculatePointLightLighting(lightVector, input.normal, diffuse * lightAttenuation); 
   lightColour =  lightColour;
    
    return textureColour * lightColour;  
    
    */
    
    //multiple point lights 
    //sample the texture to get the colour at the the current fragment.
    textureColour = texture0.Sample(Sampler0, input.tex); 
    
    
        for (int i = 0; i < 4; ++i)
    {
        if(lightsInfo[i].state == 1)
        {
         //caclculate distance between the light and the fragment.
            distance = length(lightsInfo[i].position - input.worldPosition);
        
        //calculate the attenuation for each light 
            lightAttenuation = calculateAttenuation(distance, lightsInfo[i].constantAttenuation, lightsInfo[i].linearAttenuation, lightsInfo[i].quadraticAttenuation, lightsInfo[i].diffuse);
        
        //normalise distance to create appropriate lght vector 
            lightVector = normalize(lightsInfo[i].position - input.worldPosition);
    
        //decide which lighting calculation based on type
            switch (lightsInfo[i].type)
            {
                case 1:
            //calculate for directional light
                    lightColour += calculateDirectionalLightLighting(-lightsInfo[i].direction, input.normal, lightsInfo[i].diffuse, lightsInfo[i].ambient);
                    break;
            
                case 2:
            //calculate for point light
                    lightColour += calculatePointLightLighting(lightVector, input.normal, lightsInfo[i].diffuse * lightAttenuation, lightsInfo[i].ambient);

                    break;
            
                case 3:
            //calculate for spot light
                    lightColour += calculateSpotLightLighting(lightVector, input.normal, lightsInfo[i].diffuse * lightAttenuation, lightsInfo[i].ambient, lightsInfo[i].direction, lightsInfo[i].spotAngle);
                    break;
            
            }
        
        }
        
    }
    
    return lightColour * textureColour;
    
}