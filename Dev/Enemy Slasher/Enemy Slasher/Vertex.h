#pragma once

class CVertex
{
private:

public:
	XMFLOAT3 m_xmf3Vertex;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT4 m_xmf4Color;
	XMFLOAT2 m_xmf2UV;

public:
	CVertex() {
		m_xmf3Vertex = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}
	CVertex(XMFLOAT3 v, XMFLOAT2 u) :m_xmf3Vertex(v), m_xmf2UV(u) {
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	CVertex(XMFLOAT3 v, XMFLOAT4 c) : m_xmf3Vertex(v), m_xmf4Color(c) {
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}

	CVertex(XMFLOAT3 v, XMFLOAT3 n, XMFLOAT4 c) : m_xmf3Vertex(v), m_xmf3Normal(n), m_xmf4Color(c) {
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}

	CVertex(XMFLOAT3 v, XMFLOAT3 n, XMFLOAT2 uv) : m_xmf3Vertex(v), m_xmf3Normal(n), m_xmf2UV(uv) {
		m_xmf4Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CVertex(XMFLOAT3 v, XMFLOAT3 n, XMFLOAT4 c, XMFLOAT2 uv) : m_xmf3Vertex(v), m_xmf3Normal(n), m_xmf4Color(c), m_xmf2UV(uv) {}

	//CVertex(XMFLOAT3 v, XMFLOAT3 n, XMFLOAT4 c, XMFLOAT2 u) : m_xmf3Vertex(v), m_xmf3Normal(n), m_xmf4Color(c), m_xmf2UV(u) { }

	CVertex(float x, float y, float z, XMFLOAT4 c) {
		m_xmf3Vertex = XMFLOAT3(x, y, z);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Color = c;
		m_xmf2UV = XMFLOAT2(0.0f, 0.0f);
	}

	CVertex(const CVertex& other) // 복사생성자
	{
		m_xmf3Vertex = other.m_xmf3Vertex;
		m_xmf3Normal = other.m_xmf3Normal;
		m_xmf4Color = other.m_xmf4Color;
		m_xmf2UV = other.m_xmf2UV;
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

class CVertex_Skining : public CVertex
{
public:
	float m_f3BlendingWeight[4];
	int m_n4BlendingIndex[4];

public:
	CVertex_Skining() {
		m_f3BlendingWeight[0] = 0.f;
		m_f3BlendingWeight[1] = 0.f;
		m_f3BlendingWeight[2] = 0.f;
		m_f3BlendingWeight[3] = 0.f;

		m_n4BlendingIndex[0] = 0;
		m_n4BlendingIndex[1] = 0;
		m_n4BlendingIndex[2] = 0;
		m_n4BlendingIndex[3] = 0;
	}
	~CVertex_Skining() {};
};

class VertexWithColorAndTexCoord
{
private:
	XMFLOAT3 vertex;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;

public:
	VertexWithColorAndTexCoord() : vertex(XMFLOAT3(0.0f, 0.0f, 0.0f)), color(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), texcoord(XMFLOAT2(0.0f, 0.0f)) {}
	VertexWithColorAndTexCoord(XMFLOAT3 v, XMFLOAT4 c, XMFLOAT2 uv) : vertex(v), color(c), texcoord(uv) {}
	VertexWithColorAndTexCoord(float x, float y, float z, XMFLOAT4 c, float u, float v) : vertex(XMFLOAT3(x, y, z)), color(c), texcoord(XMFLOAT2(u, v)) {}

	~VertexWithColorAndTexCoord() {}

	XMFLOAT3 GetVertex() const { return vertex; }
	XMFLOAT4 GetColor() const { return color; }
	XMFLOAT2 GetTexCoord() const { return texcoord; }
};

class VertexWithTexCoord
{
private:
	XMFLOAT3 vertex;
	XMFLOAT2 texcoord;

public:
	VertexWithTexCoord() : vertex(XMFLOAT3(0.0f, 0.0f, 0.0f)), texcoord(XMFLOAT2(0.0f, 0.0f)) {}
	VertexWithTexCoord(XMFLOAT3 v, XMFLOAT2 uv) : vertex(v), texcoord(uv) {}
	VertexWithTexCoord(float x, float y, float z, float u, float v) : vertex(XMFLOAT3(x, y, z)), texcoord(XMFLOAT2(u, v)) {}

	~VertexWithTexCoord() {}

	XMFLOAT3 GetVertex() const { return vertex; }
	XMFLOAT2 GetTexCoord() const { return texcoord; }
};

class VertexWithColorAnd2TexCoord
{
private:
	XMFLOAT3 vertex;
	XMFLOAT4 color;
	XMFLOAT2 texcoord1;
	XMFLOAT2 texcoord2;

public:
	VertexWithColorAnd2TexCoord() : vertex(XMFLOAT3(0.0f, 0.0f, 0.0f)), color(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), texcoord1(XMFLOAT2(0.0f, 0.0f)), texcoord2(XMFLOAT2(0.0f, 0.0f)) {}
	VertexWithColorAnd2TexCoord(XMFLOAT3 v, XMFLOAT4 c, XMFLOAT2 uv1, XMFLOAT2 uv2) : vertex(v), color(c), texcoord1(uv1), texcoord2(uv2) {}
	VertexWithColorAnd2TexCoord(float x, float y, float z, XMFLOAT4 c, float u1, float v1, float u2, float v2) : vertex(XMFLOAT3(x, y, z)), color(c), texcoord1(XMFLOAT2(u1, v1)), texcoord2(XMFLOAT2(u2, v2)) {}

	~VertexWithColorAnd2TexCoord() {}

	XMFLOAT3 GetVertex() const { return vertex; }
	XMFLOAT4 GetColor() const { return color; }
	XMFLOAT2 GetTexCoord1() const { return texcoord1; }
	XMFLOAT2 GetTexCoord2() const { return texcoord2; }
};

class VertexWithColorAnd2TexCoordAndNormal
{
private:
	XMFLOAT3 vertex;
	XMFLOAT4 color;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord1;
	XMFLOAT2 texcoord2;

public:
	VertexWithColorAnd2TexCoordAndNormal() : vertex(XMFLOAT3(0.0f, 0.0f, 0.0f)), color(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), normal(XMFLOAT3(0.0f, 0.0f, 0.0f)), texcoord1(XMFLOAT2(0.0f, 0.0f)), texcoord2(XMFLOAT2(0.0f, 0.0f)) {}
	VertexWithColorAnd2TexCoordAndNormal(XMFLOAT3 v, XMFLOAT4 c, XMFLOAT3 n, XMFLOAT2 uv1, XMFLOAT2 uv2) : vertex(v), color(c), normal(n), texcoord1(uv1), texcoord2(uv2) {}
	VertexWithColorAnd2TexCoordAndNormal(float x, float y, float z, XMFLOAT4 c, float nx, float ny, float nz, float u1, float v1, float u2, float v2) : vertex(XMFLOAT3(x, y, z)), color(c), normal(XMFLOAT3(nx, ny, nz)), texcoord1(XMFLOAT2(u1, v1)), texcoord2(XMFLOAT2(u2, v2)) {}

	~VertexWithColorAnd2TexCoordAndNormal() {}

	XMFLOAT3 GetVertex() const { return vertex; }
	XMFLOAT4 GetColor() const { return color; }
	XMFLOAT3 GetNormal() const { return normal; }
	XMFLOAT2 GetTexCoord1() const { return texcoord1; }
	XMFLOAT2 GetTexCoord2() const { return texcoord2; }
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