
Texture2D heightMapTexture : register(t0);
SamplerState SamplerVS : register(s0);
//normal calcualtion based off https://www.youtube.com/watch?v=izsMr5Pyk2g

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


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
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[4] : TEXCOORD2; 

};

//get the greyscale value for this pixel from the heightmap to adjust the y value accoringly 
float getHeight(float2 uv)
{
    float heightColour = heightMapTexture.SampleLevel(SamplerVS, uv, 1).r;
    return heightColour;

}



OutputType main(InputType input)
{
    OutputType output;
    
    float displacementFactor = 10; //facotr to multiply y position by based on the height map colour 
    //define offset for how close to the current pixel we 
    const float2 offsetU = float2(0.01, 0.0); //offset for sampling point on the height map to be used for pixels to the right and left 
    const float2 offsetV = float2(0.0, 0.01); //offset for sampling point on the height map to be used for pixels above and below 
	
	
    //calculate vertex manipulation value to be applied to the y value based on texel colour in the height map
    float heightmapColour;
    heightmapColour = getHeight(input.tex); //get height of the current texel 
    
    //move the positiono f the vertex on the y based on the texture Colour to a scale of 10
    input.position.y += displacementFactor * heightmapColour;
    
    //sample heights of surrounding pixels of current pixel 
    float heightRight = getHeight(input.tex + offsetU); //sample to the right
    float heightLeft = getHeight(input.tex - offsetU); //sample to the left 
    float heightBottom = getHeight(input.tex - offsetV); //sample below 
    float heightTop = getHeight(input.tex + offsetV); //sample above 
    
    //calculate the vectors from the right to left and top to bottom to be used in the cross product to calculate the normals  
    float3 slopeU = float3(1, displacementFactor * (heightRight - heightLeft), 0);
    float3 slopeV = float3(0, displacementFactor * (heightTop - heightBottom), -1);
    
    //calculate the normal for this position on the height map 
    output.normal = normalize(cross(slopeU, slopeV));

    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
   // Calculate the position of the vertice as viewed by the light source.
    for (int i = 0; i < 4; ++i)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightsInfo[i].lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightsInfo[i].lightProjection);
    }

    //store the texture coords for the pixel shader 
    output.tex = input.tex;
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    output.normal = mul(output.normal, (float3x3) worldMatrix);

    
    return output;
}