#pragma once

enum class ShaderType : int;

class CShaderManager
{
	// �ϴ� ���̴� �Ŵ����� ��� ���̴��� ������ �ִ� ������ ����
private:
	std::vector<CShader*> m_vShaderManager;
	ShaderType m_UsingShader;

public:
	CShaderManager();
	~CShaderManager();

	void ReleaseShaderVariables();

	void BuildShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ShaderType pShaderType);
};