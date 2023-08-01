#pragma once

#include "stdafx.h"

enum class LightType : int
{
	POINT_LIGHT,
	SPOT_LIGHT,
	DIRECTIONAL_LIGHT
};


class CLight
{
public:
	bool					m_bEnable;
	LightType				m_Type;
	float					m_fRange;

	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;

	XMFLOAT3				m_xmf3Position;
	XMFLOAT3				m_xmf3Direction;
	XMFLOAT3				m_xmf3Attenuation;

	float 					m_fFalloff;
	float					m_fPhi; //cos(m_fPhi)
	float 					m_fTheta; //cos(m_fTheta)

public:
	CLight(bool enable, LightType type, float range, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular,
		XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 attenuation, float falloff, float phi, float theta);
};


#define MAX_LIGHTS			16 


struct LIGHTS_INFO
{
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
	CLight* m_pLights;
};


class CLights
{
public:
	CLights();
	~CLights();

private:

	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
	CLight					*m_pLights;

public:
	void BuildLights();
	void ReleaseLights();

	XMFLOAT4 GetGloabalAmbient() { return m_xmf4GlobalAmbient; }
	int GetNumLight() { return m_nLights; }
	CLight* GetPointerLights() { return m_pLights; }
};


