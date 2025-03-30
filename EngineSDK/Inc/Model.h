#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{ 
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint	Get_NumMeshes() const { return m_iNumMeshes; }

	const _float4x4*	Get_CombinedTransformationMatrix(const _char* pBoneName) const;

	void	Set_Animation(_uint iAnimIndex, _bool isLoop = true)
	{
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_bool		Play_Animation(_float fTimeDelta);

public:
	HRESULT		Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex);
	HRESULT		Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

private:
	/* aiScene : 파일을 읽은 결과 */
	/* 모델 로드에 필요한 모든 데이터를 다 들고 있다. */
	const aiScene*				m_pAIScene = { nullptr };

	/* 파일을 읽어준다. aiScene을 리턴한다. */
	Assimp::Importer			m_Importer = {};

private:
	TYPE						m_eModelType = { TYPE_END };
	_float4x4					m_PreTransformMatrix = {};
	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

	/* 내 모델 전체의 뼈 정보 */
	/* CBone : 뼈의 이름, 상태, 부모, */
	vector<class CBone*>		m_Bones;

	_uint						m_iCurrentAnimIndex = {};
	_bool						m_isLoop = { false };
	_uint						m_iNumAnimations = {};
	vector<class CAnimation*>	m_Animations;

private:
	HRESULT		Ready_Meshes();
	HRESULT		Ready_Materials(const _char* pModelFilePath);
	HRESULT		Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT		Ready_Animations();

public:
	static CModel*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	CComponent*		Clone(void* pArg)	override;
	void	Free()						override;
};

END