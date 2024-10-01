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
	
	// Change the position vector to be 4 units for proper matrix calculations.
   // input.position.w = 1.0f;
    
    //height map
    float heightmapColour;
    heightmapColour = getHeight(input.tex);
    
    //move the positiono f the vertex on the y based on the texture Colour to a scale of 10
    input.position.y += 10.f * heightmapColour;
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //store the texture coords for the pixel shader 
    output.tex = input.tex;
    
    //Calculate the normal vector against the world matrix only and normalise
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.worldPosition = mul(input.position, worldMatrix).xyz; 
	
    //calculate the position of the vertex in the world 
    float4 worldPosition = mul(input.position, worldMatrix);
	//calculate vector that points from camera to vertex
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	//create unit vector for lighting calculations 
    output.viewVector = normalize(output.viewVector);
    
    
    return output;
}