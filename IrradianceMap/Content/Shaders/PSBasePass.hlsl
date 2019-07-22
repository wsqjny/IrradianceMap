//--------------------------------------------------------------------------------------
// By XU, Tianchen
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct PSIn
{
	float4	Pos		: SV_POSITION;
	float3	WSPos	: POSWORLD;
	float4	CSPos	: POSCURRENT;
	float4	TSPos 	: POSHISTORY;
	float3	Norm	: NORMAL;
};

struct PSOut
{
	min16float4 Color		: SV_TARGET0;
	min16float4 Velocity	: SV_TARGET1;
};

//--------------------------------------------------------------------------------------
// Constant buffer
//--------------------------------------------------------------------------------------
cbuffer cbPerObject
{
	float3 g_eyePt;
};

//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
TextureCube<float3>	g_txIrradiance	: register (t0);
TextureCube<float3>	g_txRadiance	: register (t1);

//--------------------------------------------------------------------------------------
// Samplers
//--------------------------------------------------------------------------------------
SamplerState g_sampler;

//--------------------------------------------------------------------------------------
// Base geometry-buffer pass
//--------------------------------------------------------------------------------------
PSOut main(PSIn input)
{
	PSOut output;

	const float3 norm = normalize(input.Norm);
	//float3 irradiance = g_txIrradiance.Sample(g_sampler, input.Norm);
	float3 irradiance = g_txIrradiance.SampleLevel(g_sampler, input.Norm, 0.0);

	const float3 viewDir = normalize(g_eyePt - input.WSPos);
	const float3 lightDir = reflect(-viewDir, norm);
	//float3 radiance = g_txRadiance.Sample(g_sampler, lightDir);
	float3 radiance = g_txRadiance.SampleLevel(g_sampler, lightDir, 4.0);

	const float2 csPos = input.CSPos.xy / input.CSPos.w;
	const float2 tsPos = input.TSPos.xy / input.TSPos.w;
	const min16float2 velocity = min16float2(csPos - tsPos) * min16float2(0.5, -0.5);

	irradiance *= 4.0;
	radiance *= 4.0;

	//output.Color = min16float4(norm * 0.5 + 0.5, 1.0);
	output.Color = min16float4(irradiance + 0.04 * radiance, 1.0);
	output.Velocity = min16float4(velocity, 0.0.xx);

	return output;
}