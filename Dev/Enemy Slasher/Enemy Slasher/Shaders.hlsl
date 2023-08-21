cbuffer cbCameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b1)
{
    matrix gmtxGameObject : packoffset(c0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_STANDARD_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	
};




VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
	
    return (output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	//float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;

	//cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return (input.color);
}



VS_STANDARD_OUTPUT VSLighting(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    //output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;

    return (output);
}

float4 PSLighting(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    //float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    //float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;

    //cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return (input.color);
}