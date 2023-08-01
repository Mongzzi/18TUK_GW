#include "Light.h"

CLight::CLight(bool enable = false, LightType type = LightType::SPOT_LIGHT, float range = 600.0f,
	XMFLOAT4 ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), XMFLOAT4 diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f), XMFLOAT4 specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f),
	XMFLOAT3 position = XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3 direction = XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3 attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f),
	float falloff = 8.0f, float phi = (float)cos(XMConvertToRadians(90.0f)), float theta = (float)cos(XMConvertToRadians(30.0f)))
{
	m_bEnable = enable;
	m_Type = type;
	m_fRange = range;
	m_xmf4Ambient = ambient;
	m_xmf4Diffuse = diffuse;
	m_xmf4Specular = specular;

	m_xmf3Position = position;
	m_xmf3Direction = direction;
	m_xmf3Attenuation = attenuation;

	m_fFalloff = falloff;
	m_fPhi = phi;
	m_fTheta = theta;
}


CLights::CLights()
{
	BuildLights();
}

CLights::~CLights()
{
}

void CLights::BuildLights()
{
	m_nLights = 4;
	m_pLights = new CLight[m_nLights];
	::ZeroMemory(m_pLights, sizeof(CLight) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_Type = LightType::POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_Type = LightType::SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));


	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_Type = LightType::DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);


	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_Type = LightType::SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));


}

void CLights::ReleaseLights()
{
	if (m_pLights) delete[] m_pLights;
}
