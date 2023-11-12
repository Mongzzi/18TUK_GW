#include "stdafx.h"
#include "Mesh.h"



CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}


CMesh::~CMesh()
{
	if (m_pVertices) delete[] m_pVertices;

	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}


void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//메쉬의 프리미티브 유형을 설정한다. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//메쉬의 정점 버퍼 뷰를 설정한다. 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);


	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	}

	else
	{
		//메쉬의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력 조립기)을 작동하게 한다).
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

CMeshIlluminated::CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] =Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices,pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;

	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices,pnIndices, nIndices);
		break;

	default:
		break;
	}
}


CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) :CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nIndices = 36;
	UINT pnIndices[36];
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,&m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3 pxmf3Positions[8];
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++) pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);
	CIlluminatedVertex pVertices[8];
	for (int i = 0; i < 8; i++) pVertices[i] = CIlluminatedVertex(pxmf3Positions[i],pxmf3Normals[i]);
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}

CBoxMesh::CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth) : CAABBMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;				// 꼭지점 개수
	m_nStride = sizeof(CVertex); // x , y, z 좌표
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <> urd(0.0, 1.0);

	float fx = width * 0.5f, fy = height * 0.5f, fz = depth * 0.5f;

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[2] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[3] = CVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[4] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[5] = CVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[6] = CVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[7] = CVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));

	// 버퍼생성
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// 바인딩위해 버퍼뷰 초기화
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	MakeAABB(pd3dDevice, pd3dCommandList);
}

CBoxMesh::~CBoxMesh()
{
}


CAABBMesh::CAABBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{

}

CAABBMesh::~CAABBMesh()
{
	if(m_pAABBVertices) delete[] m_pAABBVertices;
	if(m_pnIndices) delete[] m_pnIndices;
}

void CAABBMesh::ReleaseUploadBuffers()
{
	//메쉬의 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

	//AABB의 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dAABBVertexUploadBuffer) m_pd3dAABBVertexUploadBuffer->Release();
	m_pd3dAABBVertexUploadBuffer = NULL;

	if (m_pd3dAABBIndexUploadBuffer) m_pd3dAABBIndexUploadBuffer->Release();
	m_pd3dAABBIndexUploadBuffer = NULL;
}

void CAABBMesh::MakeAABB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_pVertices) return; // 만약 본래 Mesh가 없다면 만들지 않는다.

	if (m_pAABBVertices) { // 이미 AABB가 있다면 지우고 다시 만든다.
		delete[] m_pAABBVertices;

		if(m_pnIndices) delete[] m_pnIndices;

		if (m_pd3dAABBVertexBuffer) m_pd3dAABBVertexBuffer->Release();
		m_pd3dAABBVertexBuffer = NULL;

		if (m_pd3dAABBIndexBuffer) m_pd3dAABBIndexBuffer->Release();
		m_pd3dAABBIndexBuffer = NULL;

		if (m_pd3dAABBVertexUploadBuffer) m_pd3dAABBVertexUploadBuffer->Release();
		m_pd3dAABBVertexUploadBuffer = NULL;

		if (m_pd3dAABBIndexUploadBuffer) m_pd3dAABBIndexUploadBuffer->Release();
		m_pd3dAABBIndexUploadBuffer = NULL;
	}


	m_nAABBVertices = 8;			 // 꼭지점 개수
	m_nAABBStride = sizeof(CVertex); // x , y, z 좌표
	m_d3dAABBPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	float min_x = FLT_MAX, max_x = FLT_MIN,
		min_y = FLT_MAX, max_y = FLT_MIN,
		min_z = FLT_MAX, max_z = FLT_MIN;

	XMFLOAT3 vertex;
	for (int i = 0; i < m_nVertices; ++i) {
		vertex = m_pVertices[i].m_xmf3Vertex;
		if (vertex.x < min_x) min_x = vertex.x;
		if (vertex.y < min_y) min_y = vertex.y;
		if (vertex.z < min_z) min_z = vertex.z;
		if (vertex.x > max_x) max_x = vertex.x;
		if (vertex.y > max_y) max_y = vertex.y;
		if (vertex.z > max_z) max_z = vertex.z;
	}

	m_xmf3MaxPos = XMFLOAT3(max_x, max_y, max_z);
	m_xmf3MinPos = XMFLOAT3(min_x, min_y, min_z);

	m_pAABBVertices = new CVertex[m_nAABBVertices];
	m_pAABBVertices[0] = CVertex(XMFLOAT3(min_x, max_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[1] = CVertex(XMFLOAT3(max_x, max_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[2] = CVertex(XMFLOAT3(max_x, max_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[3] = CVertex(XMFLOAT3(min_x, max_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[4] = CVertex(XMFLOAT3(min_x, min_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[5] = CVertex(XMFLOAT3(max_x, min_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[6] = CVertex(XMFLOAT3(max_x, min_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pAABBVertices[7] = CVertex(XMFLOAT3(min_x, min_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// 버퍼생성
	m_pd3dAABBVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pAABBVertices, m_nAABBStride * m_nAABBVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dAABBVertexUploadBuffer);

	// 바인딩위해 버퍼뷰 초기화
	m_d3dAABBVertexBufferView.BufferLocation = m_pd3dAABBVertexBuffer->GetGPUVirtualAddress();
	m_d3dAABBVertexBufferView.StrideInBytes = m_nAABBStride;
	m_d3dAABBVertexBufferView.SizeInBytes = m_nAABBStride * m_nAABBVertices;

	m_nAABBIndices = 24;
	int AABBIndexer = 0;
	m_pnIndices = new UINT[m_nAABBIndices];
	// 윗면
	m_pnIndices[AABBIndexer++] = 0; m_pnIndices[AABBIndexer++] = 1;
	m_pnIndices[AABBIndexer++] = 1; m_pnIndices[AABBIndexer++] = 2;
	m_pnIndices[AABBIndexer++] = 2; m_pnIndices[AABBIndexer++] = 3;
	m_pnIndices[AABBIndexer++] = 3; m_pnIndices[AABBIndexer++] = 0;

	// 아랫면
	m_pnIndices[AABBIndexer++] = 4; m_pnIndices[AABBIndexer++] = 5;
	m_pnIndices[AABBIndexer++] = 5; m_pnIndices[AABBIndexer++] = 6;
	m_pnIndices[AABBIndexer++] = 6; m_pnIndices[AABBIndexer++] = 7;
	m_pnIndices[AABBIndexer++] = 7; m_pnIndices[AABBIndexer++] = 4;

	// 기둥
	m_pnIndices[AABBIndexer++] = 0; m_pnIndices[AABBIndexer++] = 4;
	m_pnIndices[AABBIndexer++] = 1; m_pnIndices[AABBIndexer++] = 5;
	m_pnIndices[AABBIndexer++] = 2; m_pnIndices[AABBIndexer++] = 6;
	m_pnIndices[AABBIndexer++] = 3; m_pnIndices[AABBIndexer++] = 7;


	//인덱스 버퍼를 생성한다. 
	m_pd3dAABBIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nAABBIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dAABBIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dAABBIndexBufferView.BufferLocation = m_pd3dAABBIndexBuffer->GetGPUVirtualAddress();
	m_d3dAABBIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dAABBIndexBufferView.SizeInBytes = sizeof(UINT) * m_nAABBIndices;
}

void CAABBMesh::RenderAABB(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//메쉬의 프리미티브 유형을 설정한다. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dAABBPrimitiveTopology);

	//메쉬의 정점 버퍼 뷰를 설정한다. 
	pd3dCommandList->IASetVertexBuffers(m_nAABBSlot, 1, &m_d3dAABBVertexBufferView);

	if (m_pd3dAABBIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dAABBIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nAABBIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	}
	else
	{
		//메쉬의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력 조립기)을 작동하게 한다).
		pd3dCommandList->DrawInstanced(m_nAABBVertices, 1, m_nAABBOffset, 0);
	}
}



CFBXMesh::CFBXMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CAABBMesh(pd3dDevice, pd3dCommandList)
{
}

CFBXMesh::~CFBXMesh()
{
}


void CFBXMesh::LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode)
{
	FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();

#ifdef _DEBUG
	//std::cout << "Mesh Name: " << (char*)pNode->GetName() << std::endl;
#endif // _DEBUG


	//DisplayMetaDataConnections(lMesh);

	//-----------------------------------------------------------------------------------
	//DisplayControlsPoints(lMesh);
	int i, lControlPointsCount = lMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	m_nVertices = lControlPointsCount;
	m_nStride = sizeof(CVertex); // x , y, z 좌표
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <> urd(0.0, 1.0);

	float fx, fy, fz;

	m_pVertices = new CVertex[m_nVertices];

	for (i = 0; i < lControlPointsCount; i++)
	{
		fx = (float)lControlPoints[i][0];
		fy = (float)lControlPoints[i][1];
		fz = (float)lControlPoints[i][2];
		m_pVertices[i] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));

		/*for (int j = 0; j < lMesh->GetElementNormalCount(); j++)
		{
			FbxGeometryElementNormal* leNormals = lMesh->GetElementNormal(j);
			if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				char header[100];
				FBXSDK_sprintf(header, 100, "            Normal Vector: ");
				if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
					Display3DVector(header, leNormals->GetDirectArray().GetAt(i));
			}
		}*/
	}
	////////////////////////////////////////////////////////////// - 여기 문제가 있음. - //////////////////////////////////////////////////////////////
	// 버퍼생성 
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// 바인딩위해 버퍼뷰 초기화
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//DisplayPolygons(lMesh);
	int j, lPolygonCount = lMesh->GetPolygonCount();
	m_nIndices = lPolygonCount;
	m_nIndices *= 3;
	pnIndices = new UINT[m_nIndices];

	for (i = 0; i < lPolygonCount; i++)
	{
		int lPolygonSize = lMesh->GetPolygonSize(i);
		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = lMesh->GetPolygonVertex(i, j);
			pnIndices[i * 3 + j] = lControlPointIndex;
		}

	}

	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	////////////////////////////////////////////////////////////// - 여기 문제가 있음. - //////////////////////////////////////////////////////////////

	//DisplayMaterialMapping(lMesh);
	//DisplayMaterial(lMesh);
	//DisplayTexture(lMesh);
	//DisplayMaterialConnections(lMesh);

	//DisplayLink(lMesh);

	//DisplayShape(lMesh);

	//DisplayCache(lMesh);

	MakeAABB(pd3dDevice, pd3dCommandList);
}

CAABB* CFBXMesh::GetAABB(XMFLOAT4X4 m_xmf4x4World)
{
	float xMin = FLT_MAX, yMin = FLT_MAX, zMin = FLT_MAX;
	float xMax = FLT_MIN, yMax = FLT_MIN, zMax = FLT_MIN;

	XMFLOAT3 ver;

	for (int i = 0;i < m_nVertices;i++) {
		ver = m_pVertices[i].GetVertex();
		
		XMMATRIX mat = XMLoadFloat4x4(&m_xmf4x4World);
		XMStoreFloat3(&ver, XMVector3TransformCoord(XMLoadFloat3(&ver), mat));
		float x = ver.x;
		float y = ver.y;
		float z = ver.z;

		if (x < xMin) xMin = x;
		if (y < yMin) yMin = y;
		if (z < zMin) zMin = z;

		if (x > xMax) xMax = x;
		if (y > yMax) yMax = y;
		if (z > zMax) zMax = z;
	}

	CAABB* aabb = new CAABB(XMFLOAT3((xMax + xMin) / 2, (yMax + yMin) / 2, (zMax + zMin) / 2 ), XMFLOAT3((xMax - xMin) / 2, (yMax - yMin) / 2, (zMax - zMin) / 2));
	//CAABB* aabb = new CAABB(XMFLOAT3((xMax + xMin) / 2 + m_xmf4x4World._41, (yMax + yMin) / 2 + m_xmf4x4World._42, (zMax + zMin) / 2 + m_xmf4x4World._43 ), XMFLOAT3((xMax - xMin) / 2, (yMax - yMin) / 2, (zMax - zMin) / 2));
	return aabb;
}

// -------------------------------- 터레인 맵 ------------------------------------


CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength]{};

	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다. 
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x + (y * m_nWidth)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage(void)
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}


#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz)
{

	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다.
	높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는0이다.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);



	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다. 
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];

	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];


#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-좌표가 1, 3, 5, ...인 경우 인덱스가 오른쪽에서 왼쪽으로 나열된다. 
	bool bRightToLeft = ((z % 2) != 0);

	if (bRightToLeft)
	{
		/*지형의 삼각형들이 오른쪽에서 왼쪽 방향으로 나열되는 경우이다. 다음 그림의 오른쪽은 (fzPercent < fxPercent)인 경우이다.
		이 경우 TopLeft의 픽셀 값은 (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))로 근사한다.
		다음 그림의 왼쪽은 (fzPercent ≥ fxPercent)인 경우이다.
		이 경우 BottomRight의 픽셀 값은 (fBottomRight = fBottomLeft + (fTopRight - fTopLeft))로 근사한다.*/

		// 삼각형이 오른쪽에서 왼쪽으로 나열되는 경우
		if (fzPercent >= fxPercent) fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}

	else
	{
		/*지형의 삼각형들이 왼쪽에서 오른쪽 방향으로 나열되는 경우이다.
		다음 그림의 왼쪽은 (fzPercent < (1.0f - fxPercent))인 경우이다.
		이 경우 TopRight의 픽셀 값은 (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))로 근사한다.
		다음 그림의 오른쪽은 (fzPercent ≥ (1.0f - fxPercent))인 경우이다.
		이 경우 BottomLeft의 픽셀 값은 (fBottomLeft = fTopLeft + (fBottomRight - fTopRight))로 근사한다.*/

		//삼각형이 왼쪽에서 오른쪽으로 나열되는 경우
		if (fzPercent < (1.0f - fxPercent)) fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}

#endif
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다. 
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);

}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다. 
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	/*높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 픽셀 값을 사용하여 법선 벡터를 계산한다.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	//(x, z), (x+1, z), (z, z+1)의 픽셀에서 지형의 높이를 구한다. 
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	//xmf3Edge1은 (0, y3, m_xmf3Scale.z) - (0, y1, 0) 벡터이다. 
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);

	//xmf3Edge2는 (m_xmf3Scale.x, y2, 0) - (0, y1, 0) 벡터이다. 
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	//법선 벡터는 xmf3Edge1과 xmf3Edge2의 외적을 정규화하면 된다. 
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	return(xmf3Normal);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext) : CMesh(pd3dDevice, pd3dCommandList)
{
	//격자의 교점(정점)의 개수는 (nWidth * nLength)이다. 
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(Vertex_Color);

	//격자는 삼각형 스트립으로 구성한다. 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	Vertex_Color* pVertices = new Vertex_Color[m_nVertices];

	/*xStart와 zStart는 격자의 시작 위치(x-좌표와 z-좌표)를 나타낸다.
	커다란 지형은 격자들의 이차원 배열로 만들 필요가 있기 때문에
	전체 지형에서 각 격자의 시작 위치를 나타내는 정보가 필요하다.*/

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//정점의 높이와 색상을 높이 맵으로부터 구한다. 
			XMFLOAT3 xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z * m_xmf3Scale.z));
			XMFLOAT4 get_color = OnGetColor(x, z, pContext);
			XMFLOAT4 xmf3Color = Vector4::Add(get_color, xmf4Color);

			pVertices[i] = Vertex_Color(xmf3Position, xmf3Color);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	delete[] pVertices;


	// ----------------------------------------------------------인덱스 버퍼--------------------------------------------------------------------------------

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	UINT* pnIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다. 
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가한다.
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));

				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다. 
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다. 
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가한다. 
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다. 
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	delete[] pnIndices;

}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth(); float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	//조명의 방향 벡터(정점에서 조명까지의 벡터)이다. 
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();

	//조명의 색상(세기, 밝기)이다. 
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);

	/*정점 (x, z)에서 조명이 반사되는 양(비율)은 정점 (x, z)의 법선 벡터와 조명의 방향 벡터의 내적(cos)과
	인접한 3개의 정점 (x+1, z), (x, z+1), (x+1, z+1)의 법선 벡터와 조명의 방향 벡터의 내적을 평균하여 구한다.
	정점 (x, z)의 색상은 조명 색상(세기)과 반사되는 양(비율)을 곱한 값이다.*/

	//정점의 색상(정점의 법선 벡터와 조명의 방향 벡터의 내적)

	XMFLOAT3 height_map_normal = pHeightMapImage->GetHeightMapNormal(x, z);
	float fScale = Vector3::DotProduct(height_map_normal, xmf3LightDirection);

	height_map_normal = pHeightMapImage->GetHeightMapNormal(x + 1, z);
	fScale += Vector3::DotProduct(height_map_normal,xmf3LightDirection);

	height_map_normal = pHeightMapImage->GetHeightMapNormal(x + 1, z+1);
	fScale += Vector3::DotProduct(height_map_normal,xmf3LightDirection);

	height_map_normal = pHeightMapImage->GetHeightMapNormal(x, z+1);
	fScale += Vector3::DotProduct(height_map_normal,xmf3LightDirection);
	
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;

	//fScale은 조명 색상(밝기)이 반사되는 비율이다. 
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

CRayMesh::CRayMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CRay* ray) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 2;				// 꼭지점 개수
	m_nStride = sizeof(CVertex); // x , y, z 좌표
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;	// 직선

	XMFLOAT3 Ori = { 0, 0, 0 };
	XMFLOAT3 Dir = { 0, 0, 1 };
	if (ray)
	{
		 Ori = ray->GetOriginal();
		 Dir = ray->GetDir();
	}
	XMFLOAT3 End = Vector3::Add(Ori, Vector3::ScalarProduct(Dir, 1000, false));
	

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(Ori.x, Ori.y, Ori.z), XMFLOAT4(0.f, 1.f, 0.0f, 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(End.x, End.y, End.z), XMFLOAT4(1.f, 0.f, 0.0f, 1.0f));

	// 버퍼생성
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// 바인딩위해 버퍼뷰 초기화
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 2;
	UINT pnIndices[2];
	// 시작
	pnIndices[0] = 0;
	// 끝
	pnIndices[1] = 1;

	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CRayMesh::~CRayMesh()
{
}
