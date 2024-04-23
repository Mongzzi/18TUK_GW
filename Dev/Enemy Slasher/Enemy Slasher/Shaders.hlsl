

cbuffer cbCameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b1)
{
    matrix gmtxGameObject : packoffset(c0);
    uint gnMaterialID : packoffset(c4);
};


#include "Light.hlsl"

//------------------------텍스처-----------------------------
Texture2D gtxtTexture : register(t0);
SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);


struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;    
    float3 positionW: POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
    output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
    input.normalW = normalize(input.normalW);
    float4 cIllumination = Lighting(input.positionW, input.normalW);
    return(cColor * cIllumination);
    //return (cColor);
}
//----------------------텍스처끝-------------------------------

// ------------------------------ 스카이 박스 전용 텍스처 ----------------------------------------------

struct VS_TEXTURED_INPUT_TWO_ELEMENT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT_TWO_ELEMENT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT_TWO_ELEMENT VS_POSITION_TEXCOORD(VS_TEXTURED_INPUT_TWO_ELEMENT input)
{
    VS_TEXTURED_OUTPUT_TWO_ELEMENT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PS_POSITION_TEXCOORD(VS_TEXTURED_OUTPUT_TWO_ELEMENT input, uint primitiveID : SV_PrimitiveID) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

    return (cColor);
}



float4 PSSkyBox(VS_TEXTURED_OUTPUT_TWO_ELEMENT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gClampSamplerState, input.uv);

    return (cColor);
}
// ------------------------------ 스카이 박스 전용 텍스처 끝 ----------------------------------------------

// ------------------------------ 터레인 전용 -----------------------------------------------
Texture2D<float4> gtxtTerrainBaseTexture : register(t1);
Texture2D<float4> gtxtTerrainDetailTexture : register(t2);
Texture2D<float> gtxtTerrainAlphaTexture : register(t3);

struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW: POSITION;
    float4 color : COLOR;
    float3 normalW : NORMAL;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
    output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.color = input.color;
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return (output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
    float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
    float fAlpha = gtxtTerrainAlphaTexture.Sample(gWrapSamplerState, input.uv0);
    float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));

    //float4x4 gmtxProjectionInverse = transpose(float4x4(gmtxProjection[0], gmtxProjection[1], gmtxProjection[2], gmtxProjection[3]));
    //float4x4 gmtxViewInverse = transpose(float4x4(gmtxView[0], gmtxView[1], gmtxView[2], gmtxView[3]));
    //float3 positionW = (float3) mul(mul(input.position, gmtxProjectionInverse), gmtxViewInverse);
    //float3 normalW = normalize(mul(input.normal, (float3x3) gmtxGameObject));
    //float4 cIllumination = Lighting(positionW, normalW);
    
    input.normalW = normalize(input.normalW);
    float4 cIllumination = Lighting(input.positionW, input.normalW);
    return(cColor * cIllumination);

    
    //return (cColor*cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_STANDARD_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct VS_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
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

////////////////////////////////////////////////////////

struct VS_COLOR_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_COLOR_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;

};

VS_COLOR_OUTPUT VSColor(VS_COLOR_INPUT input)
{
    VS_COLOR_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;

    return (output);
}

float4 PSColor(VS_COLOR_OUTPUT input) : SV_TARGET
{
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


//#define _WITH_VERTEX_LIGHTING

//struct VS_LIGHTING_INPUT
//{
//    float3 position : POSITION;
//    float3 normal : NORMAL;
//};

//struct VS_LIGHTING_OUTPUT
//{
//    float4 position : SV_POSITION;
//    float3 positionW : POSITION;
//#ifdef _WITH_VERTEX_LIGHTING
//    float4 color : COLOR;
//#else
//float3 normalW : NORMAL;
//#endif
//};


//VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
//{
//    VS_LIGHTING_OUTPUT output;
//    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
//    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
//    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
    
//#ifdef _WITH_VERTEX_LIGHTING
//    output.color = Lighting(output.positionW, normalize(normalW));
//#else
//output.normalW = normalW;
//#endif
//    return (output);
//}




//float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
//{
//#ifdef _WITH_VERTEX_LIGHTING
//    return (input.color);
//#else
//float3 normalW = normalize(input.normalW);
//float4 color = Lighting(input.positionW, normalW);
//return(color);
//#endif
//}




// // 카툰 랜더링을 위한 버텍스 쉐이더
// struct VertexInput {
//     float3 position : POSITION;
//     float3 normal : NORMAL;
// };

// struct VertexOutput {
//     float4 position : SV_POSITION;
//     float3 normal : NORMAL;
// };

// VertexOutput CartoonVertexShader(VertexInput input) {
//     VertexOutput output;

//     // 기존 위치 전달
//     output.position = mul(float4(input.position, 1.0f), gWorldViewProj);

//     // 기존 노말 전달
//     output.normal = mul(input.normal, (float3x3)gWorld);

//     return output;
// }

// // 카툰 랜더링을 위한 픽셀 쉐이더
// float4 CartoonPixelShader(VertexOutput input) : SV_TARGET {
//     // 단순한 조명 계산 (여기에서는 빛의 방향이 예시로 주어짐)
//     float3 lightDir = normalize(float3(1.0f, -1.0f, 0.0f));
//     float3 normal = normalize(input.normal);
//     float lighting = max(0.0f, dot(normal, lightDir));

//     // 윤곽선을 강조하기 위해 노말 정보를 사용
//     float outline = 1.0f - smoothstep(0.95f, 1.0f, dot(normal, -lightDir));

//     // 최종 색상 계산 (여기에서는 흰색과 검은색을 사용하여 음영을 단순화)
//     float3 baseColor = float3(1.0f, 1.0f, 1.0f);
//     float3 cartoonColor = lerp(baseColor, float3(0.0f, 0.0f, 0.0f), lighting);

//     // 윤곽선 색상과 믹스
//     float3 finalColor = lerp(cartoonColor, float3(0.0f, 0.0f, 0.0f), outline);

//     return float4(finalColor, 1.0f);
// }