// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColour;
    float3 lightDirection;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
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

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 lightColour;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    textureColour = texture0.Sample(sampler0, input.tex);
    lightColour = calculateLighting(-lightDirection, input.normal, diffuseColour);
	//lightColour = calculateSpotLightLighting()
    return lightColour * textureColour;
}



