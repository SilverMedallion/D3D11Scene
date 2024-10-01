// colour vertex shader
// Simple geometry pass
// texture coordinates and normals will be ignored.

Texture2D heightMapTexture : register(t0);
SamplerState SamplerVS : register(s0);


cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
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
    
    float3 viewVector : TEXCOORD2;
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
    
    float displacementFactor = 10;      //facotr to multiply y position by based on the height map colour 
    //define offset for how close to the current pixel we 
    const float2 offsetU = float2(0.01, 0.0); //offset for sampling point on the height map to be used for pixels to the right and left 
    const float2 offsetV = float2(0.0, 0.01); //offset for sampling point on the height map to be used for pixels above and below 
	
	
    //calculate vertex manipulation value to be applied to the y value based on texel colour in the height map
    float heightmapColour;
    heightmapColour = getHeight(input.tex);    //get height of the current texel 
    
    //move the positiono f the vertex on the y based on the texture Colour to a scale of 10
   input.position.y += displacementFactor * heightmapColour;
    
    //sample heights of surrounding pixels of current pixel 
    float heightRight = getHeight(input.tex + offsetU);     //sample to the right
    float heightLeft = getHeight(input.tex - offsetU);      //sample to the left 
    float heightBottom = getHeight(input.tex - offsetV);    //sample below 
    float heightTop = getHeight(input.tex + offsetV);       //sample above 
    
    //calculate the vectors from the right to left and top to bottom to be used in the cross product to calculate the normals  
    float3 slopeU = float3(offsetU.x, displacementFactor * (heightRight - heightLeft), offsetU.y);
    float3 slopeV = float3(offsetV.x, displacementFactor * (heightRight - heightLeft), offsetV.y);
    
    //calculate the normal for this position on the height map 
    output.normal = normalize(cross(slopeU, slopeV)); 

    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //store the texture coords for the pixel shader 
    output.tex = input.tex;
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;
	
    //calculate the position of the vertex in the world 
    float4 worldPosition = mul(input.position, worldMatrix);
	//calculate vector that points from camera to vertex
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	//create unit vector for lighting calculations 
    output.viewVector = normalize(output.viewVector);
    
    
    return output;
}