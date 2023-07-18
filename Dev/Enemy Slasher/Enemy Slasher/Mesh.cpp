#include "stdafx.h"
#include "Mesh.h"



CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}


CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}


void CMesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�޽��� ������Ƽ�� ������ �����Ѵ�. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//�޽��� ���� ���� �並 �����Ѵ�. 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);


	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
	}

	else
	{
		//�޽��� ���� ���� �並 �������Ѵ�(����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}


CBoxMesh::CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;				// ������ ����
	m_nStride = sizeof(Vertex_Color); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <> urd(0.0, 1.0);

	float fx = width * 0.5f, fy = height * 0.5f, fz = depth * 0.5f;

	Vertex_Color pVertices[8];
	pVertices[0] = Vertex_Color(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[1] = Vertex_Color(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[2] = Vertex_Color(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[3] = Vertex_Color(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[4] = Vertex_Color(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[5] = Vertex_Color(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[6] = Vertex_Color(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	pVertices[7] = Vertex_Color(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));

	// ���ۻ���
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

}

CBoxMesh::~CBoxMesh()
{
}