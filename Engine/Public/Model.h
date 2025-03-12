#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{ 
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

private:
	/* aiScene : 파일을 읽은 결과 */
	/* 모델 로드에 필요한 모든 데이터를 다 들고 있다. */
	const aiScene*				m_pAIScene = { nullptr };

	/* 파일을 읽어준다. aiScene을 리턴한다. */
	Assimp::Importer			m_Importer = {};

private:
	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

private:
	HRESULT Ready_Meshes();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END