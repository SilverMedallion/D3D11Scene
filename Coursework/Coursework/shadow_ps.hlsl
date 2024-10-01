
Texture2D shaderTexture : register(t0);

Texture2D depthMapTexture[4] : register(t1);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

struct lightData
{
    matrix lightView;
    matrix lightProjection;
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
    float4 lightViewPos[4] : TEXCOORD2;
};


// checks if the gemoetry is in our shadow map. if values are between 0 and 1 then we are inside our shadow map. returns false then whatever we are looking at is not in lights view and 
//can just return texture colour 
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    //here we tranlate the origin to the top left conrner. the rasterizer usually does this when we pass in an SV_POSITION semantic however this is not the case here 
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 calculateSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	//blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}


//modified for multiple lights 
//Calculate lighting intensity based on angle between normal and light vector
float4 calculateDirectionalLightLighting(float3 lDirection, float3 pNormal, float4 lDiffuse, float4 lAmbient, float shadowMapBias, float4 lightViewPos, int lightIndex)
{
   
    
	// Calculate the projected texture coordinates.
    float2 pTexCoord = getProjectiveCoords(lightViewPos);
	
    
    // Shadow test. Is or isn't in shadow
    if (hasDepthData(pTexCoord))
    {
        // Has depth map data. at this point we know what we are looking at is in the view of our light 
        if (!isInShadow(depthMapTexture[lightIndex], pTexCoord, lightViewPos, shadowMapBias))
        {
            
            // is NOT in shadow, therefore direcional  light
            float intensity = saturate(dot(pNormal, lDirection)); //saturate is used so that if the angle is less than 0, 0 will still be returned 
            float4 colour = lAmbient + saturate(lDiffuse * intensity); //ambient colour added here, remember need to pass it in to make sure we are getting the data for the correct light
            return colour;

        }
    }
    
    
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

//modified for multiple lights
float4 calculatePointLightLighting(float3 lightVec, float3 pixelNormal, float4 lightDif, float4 lAmbient)
{
    float intensity = saturate(dot(pixelNormal, lightVec));
    float4 colour = lAmbient + saturate(lightDif * intensity);
    return colour;
}

float4 calculateSpotLightLighting(float3 lightVec, float3 pixelNormal, float4 lightDif, float4 lightAmbient, float3 spotDirection, float spotAngle, float shadowMapBias, float4 lightViewPos, int lightIndex)
{
    
    
    
	// Calculate the projected texture coordinates. 
    float2 pTexCoord = getProjectiveCoords(lightViewPos);
	
    // check if there is depth data for pixel 
    if (hasDepthData(pTexCoord))
    {
        // Has depth map data. at this point we know what we are looking at is in the view of our light 
        if (!isInShadow(depthMapTexture[lightIndex], pTexCoord, lightViewPos, shadowMapBias))
        {
            // is NOT in shadow, therefore spot light
          
            
            // Calculate the dot product(cosign of the angle) between the light direction and the spot direction, to give the 
            float spotFactor = dot(normalize(lightVec), normalize(-spotDirection));   //remember negative becuase we don't want vector from surface to light

    // Check if the pixel is within the spot angle 
            if (spotFactor > cos(spotAngle / 2))   //using angle from centre of light to edge of cone to give threshold value to compare spot factor agains 
            {
                float intensity = saturate(dot(pixelNormal, lightVec));
                float4 colour = lightAmbient + saturate(lightDif * intensity);
                return colour;
            }

        }
    }
    
    
    return 0;
}

//modified to work for multiple lights
float calculateAttenuation(float distance, float constantA, float linearA, float quadraticA, float4 ldiffuse)
{
    float attenuation = 1 / (constantA + (linearA * ldiffuse) + (quadraticA * pow(distance, 2)));
    return attenuation;
}



float4 main(InputType input) : SV_TARGET
{
    float4 lightColour = 0;
    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    //sample texture of object at the location we are currently looking at 
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);


    
    for (int i = 0; i < 4; ++i)
    {
        
        if (lightsInfo[i].state == 1)
        {

         //caclculate distance between the light and the fragment.
            float distance = length(lightsInfo[i].position - input.worldPosition);
        
        //calculate the attenuation for each light 
            float lightAttenuation = calculateAttenuation(distance, lightsInfo[i].constantAttenuation, lightsInfo[i].linearAttenuation, lightsInfo[i].quadraticAttenuation, lightsInfo[i].diffuse);
        
        //normalise distance to create appropriate lght vector 
            float3 lightVector = normalize(lightsInfo[i].position - input.worldPosition);
    
        //decide which lighting calculation based on type
            
            switch (lightsInfo[i].type)
            {
                case 1:
            //calculate for directional light
                    lightColour += calculateDirectionalLightLighting(-lightsInfo[i].direction, input.normal, lightsInfo[i].diffuse, lightsInfo[i].ambient, shadowMapBias, input.lightViewPos[i], i);

                
                
                    break;
            
                case 2:
            //calculate for point light
                    lightColour += calculatePointLightLighting(lightVector, input.normal, lightsInfo[i].diffuse * lightAttenuation, lightsInfo[i].ambient);

                    break;
            
                case 3:
            //calculate for spot light
                    lightColour += calculateSpotLightLighting(lightVector, input.normal, lightsInfo[i].diffuse * lightAttenuation, lightsInfo[i].ambient, lightsInfo[i].direction, lightsInfo[i].spotAngle, shadowMapBias, input.lightViewPos[i], i);
                    break;
            
            }
        
        }
        
    }
    
    return float4(lightColour.rgb * textureColour.rgb, 1.0f);
}