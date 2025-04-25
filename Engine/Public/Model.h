#pragma once

#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Cube;

class ENGINE_DLL CModel final : public CComponent
{ 
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tagModel
	{
		_bool  bPickable = { false };

	}MODEL_DESC;

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	~CModel() override = default;

public:
	_uint	Get_NumMeshes() const { return m_iNumMeshes; }

	const _float4x4*	Get_CombinedTransformationMatrix(const _char* pBoneName) const;

	_float	Get_AnimCurrentTrackPosition() const;

	void	Set_InitAnimIndex(_uint iInitAnimIndex, _bool isLoop = true)
	{
		m_iCurrentAnimIndex = iInitAnimIndex;
		m_iNextAnimIndex	= iInitAnimIndex;
		m_isLoop = isLoop;
	}

	void	Set_Animation(_uint iAnimIndex, _bool isLoop = true, _float speedFactor = 1.f)
	{
		m_iNextAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
		m_fSpeedFactor = speedFactor;
	}

	void	Link_AnimationCombo(_uint iCurAnimIndex, _float fAnimCurTrackPos, _bool isLoop = true, _float speedFactor = 1.f)
	{
		m_iCurrentAnimIndex = iCurAnimIndex;
		m_iNextAnimIndex = iCurAnimIndex;
		m_isLoop = isLoop;
		m_fSpeedFactor = speedFactor;

		Set_AnimCurrentTrackPosition(fAnimCurTrackPos);
	}

	void	Set_AnimCurrentTrackPosition(_float fAnimCurTrackPos);

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_bool		Play_Animation(_float fTimeDelta);
	_bool		Picking_Model(const _float4& worldMousePos, const _float3& worldMouseRay, _float3& localPickedPos, const _float4x4& WorldMatrix) const;
	_bool		Picking_Vertex(const _float4& worldMousePos, const _float3& worldMouseRay, _float3& vOutPickedVertex,
	                      const _float4x4& WorldMatrix) const;

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
	_uint						m_iNextAnimIndex = {};
	_bool						m_isLoop = { false };
	_float						m_fSpeedFactor = {};
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