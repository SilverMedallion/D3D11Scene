// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D effectsTexture : register(t0);
Texture2D sceneTexture : register(t1);

SamplerState Sampler0 : register(s0);


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 effectColor = effectsTexture.Sample(Sampler0, input.tex);
    float4 sceneColor = sceneTexture.Sample(Sampler0, input.tex);


    return effectColor + sceneColor;
}