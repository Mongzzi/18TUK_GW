#pragma once

class CVertex
{
private:

public:
	XMFLOAT3 m_xmf3Vertex;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT4 m_xmf4Color;
	//XMFLOAT2 m_xmf2UV;

public:
	CVertex() {
		m_xmf3Vertex = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		//m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}

	CVertex(XMFLOAT3 v, XMFLOAT4 c) : m_xmf3Vertex(v), m_xmf4Color(c) {
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		//m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}

	CVertex(XMFLOAT3 v, XMFLOAT3 n, XMFLOAT4 c) : m_xmf3Vertex(v), m_xmf3Normal(n), m_xmf4Color(c) {
		//m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}

	//CVertex(XMFLOAT3 v, XMFLOAT3 n, XMFLOAT4 c, XMFLOAT2 u) : m_xmf3Vertex(v), m_xmf3Normal(n), m_xmf4Color(c), m_xmf2UV(u) { }

	CVertex(float x, float y, float z, XMFLOAT4 c) {
		m_xmf3Vertex = XMFLOAT3(x, y, z);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Color = c;
		//m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}
	~CVertex() { }

	XMFLOAT3 GetVertex() { return m_xmf3Vertex; };
};

/////////////////////////////////////////////////////////////////////////////

class Vertex_Color
{
private:
	XMFLOAT3 vertex;
	XMFLOAT4 color;
public:
	Vertex_Color(XMFLOAT3 v, XMFLOAT4 c) : vertex{ v }, color{ c } {}

	Vertex_Color() {
		vertex = XMFLOAT3(0.0f, 0.0f, 0.0f);
		color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	Vertex_Color(float x, float y, float z, XMFLOAT4 c) {
		vertex = XMFLOAT3(x, y, z);
		color = c;
	}
	~Vertex_Color() { }

	XMFLOAT3 GetVertex() { return vertex; };
};

/////////////////////////////////////////////////////////////////////////////

class CIlluminatedVertex
{
protected:
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmf3Position;
public:
	CIlluminatedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) {
		m_xmf3Position = XMFLOAT3(x, y, z);
		m_xmf3Normal = xmf3Normal;
	}
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) {
		m_xmf3Position = xmf3Position;
		m_xmf3Normal = xmf3Normal;
	}
	~CIlluminatedVertex() { }
};