#include "object3d.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

PixelShaderOutput main(VertexShaderOutput input)
{
	PixelShaderOutput output;
	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	output.color = gMaterial.color * textureColor;

	if (gMaterial.enableLighting != 0)
	{
		float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	}
	else {
		output.color = gMaterial.color * textureColor;
	}
	return output;
}