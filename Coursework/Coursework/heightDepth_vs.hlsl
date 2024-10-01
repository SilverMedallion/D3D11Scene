
Texture2D heightMapTexture : register(t0);
SamplerState SamplerVS : register(s0);
//normal calculation based off https://www.youtube.com/watch?v=izsMr5Pyk2g
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
    float3 normal : NORMAL;

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
    float3 slopeV = float3(0, displacementFactor * (heightTop - heightBottom), 1);
    
    //calculate the normal for this position on the height map 
    output.normal = normalize(cross(slopeU, slopeV));

    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.normal = mul(output.normal, (float3x3)worldMatrix);

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
    
    return output;
}