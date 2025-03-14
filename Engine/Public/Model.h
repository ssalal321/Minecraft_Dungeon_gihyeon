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
	_uint		Get_NumMeshes() const {	return m_iNumMeshes;	}

	HRESULT		Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	HRESULT		Initialize(void* pArg)	override;
	HRESULT		Render(_uint iMeshIndex);

public:
	HRESULT		Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex);

private:
	/* aiScene : 파일을 읽은 결과 */
	/* 모델 로드에 필요한 모든 데이터를 다 들고 있다. */
	const aiScene*			m_pAIScene = { nullptr };

	/* 파일을 읽어준다. aiScene을 리턴한다. */
	Assimp::Importer		m_Importer = {};


private:
	_float4x4					m_PreTransformMatrix = {};
	_uint						m_iNumMeshes = {};
	_uint						m_iNumMaterials = {};

	vector<class CMesh*>		m_Meshes;
	vector<class CMaterial*>	m_Materials;

private:
	HRESULT		Ready_Meshes();
	HRESULT		Ready_Materials(const _char* pModelFilePath);


public:
	static CModel*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType,
						   const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	CComponent*		Clone(void* pArg)	override;
	void			Free()				override;
};

END