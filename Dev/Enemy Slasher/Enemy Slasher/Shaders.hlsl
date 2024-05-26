#define FRAME_BUFFER_WIDTH		1280
#define FRAME_BUFFER_HEIGHT		960

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


cbuffer cbTimeInfo : register(b4)
{
	float gfCurrentTime : packoffset(c0.x);
	float gfElapsedTime : packoffset(c0.y);
};

cbuffer cbSkinningInfo : register(b5)
{
	matrix gmtxBoneMatrices[96] : packoffset(c0);
	bool gbIsSkinningAvaliable : packoffset(c384);
}

cbuffer cb2DGameObjectInfo : register(b6)
{
	float2 position; // 오브젝트의 화면 좌표 (x, y)
	float2 size;	// 오브젝트의 가로 세로길이 // 절반 길이아님
	bool IsClicked;
}


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
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
	output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return (output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	return (cColor * cIllumination);
	//return (cColor);
}


float4 PSUITextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
	return (cColor);
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


// ------------ 타이틀 2D 메시 오브젝트 ---------------	 // 카메라계산 x
VS_TEXTURED_OUTPUT_TWO_ELEMENT VS_2D_TITLE_OBJECT(VS_TEXTURED_INPUT_TWO_ELEMENT input)
{
	VS_TEXTURED_OUTPUT_TWO_ELEMENT output;

	output.position = float4(input.position, 1.0f);
	output.uv = input.uv;

	return (output);
}

float4 PS_2D_TITLE_OBJECT(VS_TEXTURED_OUTPUT_TWO_ELEMENT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
	return (cColor);
}



float4 PS_POSITION_TEXCOORD(VS_TEXTURED_OUTPUT_TWO_ELEMENT input, uint primitiveID : SV_PrimitiveID) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

	return (cColor);
}


// ------ 2D 버튼 오브젝트 ---------------
VS_TEXTURED_OUTPUT_TWO_ELEMENT VS_2D_OBJECT(VS_TEXTURED_INPUT_TWO_ELEMENT input)
{
	VS_TEXTURED_OUTPUT_TWO_ELEMENT output;

	float2 screenPos = position + input.position.xy;

	// 눌림 상태에서 버튼을 약간 이동시킴
	if (IsClicked) {
		screenPos += float2(2.0f, -2.0f); 
	}

	screenPos = screenPos / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT) * 2.0f - 1.0f;
	screenPos.y = -screenPos.y;

	output.position = float4(screenPos, 0.0f, 1.0f);
	output.uv = input.uv;
	return output;
}

float4 PS_2D_OBJECT(VS_TEXTURED_OUTPUT_TWO_ELEMENT input) : SV_TARGET
{
	float4 cColor;
	cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

	if (IsClicked) {
		// 색상을 어둡게 하여 눌림 효과를 줌
		cColor.rgb *= 0.8f;
	}

	return cColor;
}
// ------ 2D 버튼 오브젝트 끝 ---------------


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
	float3 positionW : POSITION;
	float4 color : COLOR;
	float3 normalW : NORMAL;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
	output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;


	//float fShift = gfElapsedTime*10;
	//output.uv0.x += fShift;


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
	return (cColor * cIllumination);

	//return (cColor*cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
    float2 uv : TEXCOORD;
};






VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
    output.uv = input.uv;

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


struct VS_BILLBOARD_INSTANCING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 instancePosition : INSTANCEPOSITION;
	float4 billboardInfo : BILLBOARDINFO; //(cx, cy, type, texture)
};

struct VS_BILLBOARD_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	int textureID : TEXTUREID;
};

VS_BILLBOARD_INSTANCING_OUTPUT VSBillboardInstancing(VS_BILLBOARD_INSTANCING_INPUT input)
{
	VS_BILLBOARD_INSTANCING_OUTPUT output;

	if (input.position.x < 0.0f) input.position.x = -(input.billboardInfo.x * 0.5f);
	else if (input.position.x > 0.0f) input.position.x = (input.billboardInfo.x * 0.5f);

	if (input.position.y < 0.0f) input.position.y = -(input.billboardInfo.y * 0.5f);
	else if (input.position.y > 0.0f) input.position.y = (input.billboardInfo.y * 0.5f);

	float3 f3Look = normalize(gvCameraPosition - input.instancePosition);
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Right = normalize(cross(f3Up, f3Look));

	matrix mtxWorld;
	mtxWorld[0] = float4(f3Right, 0.0f);
	mtxWorld[1] = float4(f3Up, 0.0f);
	mtxWorld[2] = float4(f3Look, 0.0f);
	mtxWorld[3] = float4(input.instancePosition, 1.0f);

	output.position = mul(mul(mul(float4(input.position, 1.0f), mtxWorld), gmtxView), gmtxProjection);

	if (input.uv.y < 0.7f)
	{
		if (input.billboardInfo.w - 1 == 1 || input.billboardInfo.w - 1 == 2 || input.billboardInfo.w - 1 == 3 || input.billboardInfo.w - 1 == 4)
		{
			float fShift = 0.0f;
			uint nResidual = ((uint)gfCurrentTime % 4);
			if (nResidual == 1) fShift = -gfElapsedTime * 15.0f;
			if (nResidual == 3) fShift = +gfElapsedTime * 15.0f;
			input.uv.x += fShift;
		}
	}
	output.uv = input.uv;
	output.textureID = (int)input.billboardInfo.w - 1;

	return (output);
}

Texture2D<float4> gtxtBillboardTextures[7] : register(t4);

float4 PSBillboardInstancing(VS_BILLBOARD_INSTANCING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtBillboardTextures[NonUniformResourceIndex(input.textureID)].Sample(gWrapSamplerState, input.uv);

	return (cColor);
}


////////////////////////////////////////////////////////////////////////////////////////////
struct VS_SKINNING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD;
	float4 blendingWeight : BLENDWEIGHT;
	int4 blendingIndex : BLENDINDICES;
};

struct VS_SKINNING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD;
};



VS_SKINNING_OUTPUT VSSkinning(VS_SKINNING_INPUT input)
{
	VS_SKINNING_OUTPUT output;

	float4x4 skinningMatrix = 0;
	for (int i = 0; i < 4; ++i)
	{
		skinningMatrix += gmtxBoneMatrices[input.blendingIndex[i]] * input.blendingWeight[i];
	}

	float3 newNormal = input.normal;

	// apply skinning
	float4 skinningPosition = float4(input.position, 1.0f);

	if (gbIsSkinningAvaliable == true)
	{
		float3 skinningPos = float3(0.0f, 0.0f, 0.0f);
		float3 skinningNormal = float3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 4; ++i)
		{
			skinningPos += input.blendingWeight[i] * mul(float4(input.position, 1.0f), gmtxBoneMatrices[input.blendingIndex[i]]).xyz;
			skinningNormal += input.blendingWeight[i] * mul(input.normal, (float3x3) gmtxBoneMatrices[input.blendingIndex[i]]);
		}

		skinningPosition = float4(skinningPos, 1.0f);
		newNormal = skinningNormal;
	}
	// apply gameObjectMat
	float4 worldPosition = mul(skinningPosition, gmtxGameObject);
	// apply viewMat
	float4 viewPosition = mul(worldPosition, gmtxView);
	// apply projectionMat
	float4 newPosition = mul(viewPosition, gmtxProjection);


	output.position = newPosition;
	output.color = input.color;
	output.normal = newNormal;
	output.texCoord = input.texCoord;

	return (output);
}

float4 PSSkinning(VS_SKINNING_OUTPUT input) : SV_TARGET
{
    float4 texColor = gtxtTexture.Sample(gWrapSamplerState, input.texCoord);
    input.normal = normalize(input.normal);
    float4 cIllumination = Lighting(input.position.xyz, input.normal);
    return (texColor * cIllumination);
	//return (float4(input.texCoord, 0.f, 1.f));
}
////////////////////////////////////////////////////////////////////////////////////////////



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