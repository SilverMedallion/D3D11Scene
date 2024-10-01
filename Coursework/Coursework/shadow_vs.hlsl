
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


OutputType main(InputType input)
{
    OutputType output;

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
    
    //calculate the normal vector agaainst the world matrix only and noramlise 
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    return output;
}