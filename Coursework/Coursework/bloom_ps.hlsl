// bloom pixel shader 
// Calculate diffuse lighting for a single directional light (also texturing)

//based off https://learnopengl.com/Advanced-Lighting/Bloom

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;

};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float3 weights;
    //remember sum of weights must be close 1 to enusre birghtness
    //thse values are based on how the eye percevies brightness since the eye is most sensitive to green 
    weights.r = 0.3;
    weights.g = 0.59;
    weights.b = 0.11;

	//sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    textureColour = texture0.Sample(sampler0, input.tex);
    
    float brigtness = dot(textureColour.rgb, weights.rgb);
    
    
    if (brigtness > 0.5)
    {
        //if return lit pixel if bright enough
        return float4(textureColour.rgb, 1.0f);
    }
    else
    {
        //if pixel is not bright return black 
        return float4(0, 0, 0, 0);
    }
	

}

