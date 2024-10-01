Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

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
    float weight0, weight1, weight2, weight3, weight4;
    float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
	//weight0 = 0.4062f;
    //weight1 = 0.2442f;
    //weight2 = 0.0545f;
    //weight3 = 0.005977;
    //weight4 = 0.000229;
    
    // Adjusted weights for stronger blur
    weight0 = 0.35f;
    weight1 = 0.28f;
    weight2 = 0.15f;
    weight3 = 0.12f;
    weight4 = 0.10f;

    // Ensure that the sum of weights equals 1
    float weightSum = weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4);
    weight0 /= weightSum;
    weight1 /= weightSum;
    weight2 /= weightSum;
    weight3 /= weightSum;
    weight4 /= weightSum;


	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 1.0f / screenWidth;
    // Add the horizontal pixels to the colour by the specific weight of each.
    
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -4.0f, 0.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -3.0f, 0.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -2.0f, 0.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -1.0f, 0.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 1.0f, 0.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 2.0f, 0.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 3.0f, 0.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 4.0f, 0.0f)) * weight4;

	// Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}
