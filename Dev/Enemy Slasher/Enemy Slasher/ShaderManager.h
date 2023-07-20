#pragma once

enum class ShaderType : int;

class CShaderManager
{
	// 일단 쉐이더 매니저는 모든 쉐이더를 가지고 있는 것으로 개발
private:
	std::vector<CShader*> m_vShaderManager;
	ShaderType m_UsingShader;

public:
	CShaderManager();
	~CShaderManager();

	void BuildShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ShaderType pShaderType);
};