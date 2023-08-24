

cbuffer cbCameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b1)
{
    matrix gmtxGameObject : packoffset(c0);
    uint gnMaterial : packoffset(c4);
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


#include "Light.hlsl"


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



//VS_STANDARD_OUTPUT VSLighting(VS_STANDARD_INPUT input)
//{
//    VS_STANDARD_OUTPUT output;

//    //output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
//    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
//    output.color = input.color;

//    return (output);
//}

//float4 PSLighting(VS_STANDARD_OUTPUT input) : SV_TARGET
//{
//    //float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    //float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    //float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

//    //float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;

//    //cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

//    return (input.color);
//}


#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#else
float3 normalW : NORMAL;
#endif
};


VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
    
#ifdef _WITH_VERTEX_LIGHTING
    output.color = Lighting(output.positionW, normalize(normalW));
#else
output.normalW = normalW;
#endif
    return (output);
}



//ÇÈ¼¿ ½¦ÀÌ´õ ÇÔ¼ö
float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
float3 normalW = normalize(input.normalW);
float4 color = Lighting(input.positionW, normalW);
return(color);
#endif
}