#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"

#include "Material.h"
#include "VIBuffer_Cube.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent (pDevice, pContext)
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent(Prototype)
	, m_eModelType { Prototype.m_eModelType }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_Meshes { Prototype.m_Meshes }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	/*, m_Animations{ Prototype.m_Animations }*/
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
{
	for (auto& pAnimation : Prototype.m_Animations)
		m_Animations.push_back(pAnimation->Clone());

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pMesh : m_Meshes)	
		Safe_AddRef(pMesh);	

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

}

const _float4x4* CModel::Get_CombinedTransformationMatrix(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
	{
		if (false == strcmp(pBone->Get_Name(), pBoneName))
			return true;

		return false;
	});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrix_Ptr();
}

_float CModel::Get_AnimCurrentTrackPosition() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();
}

void CModel::Set_AnimCurrentTrackPosition(_float fAnimCurTrackPos)
{
	m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fAnimCurTrackPos);
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	/* 어떤 설정? */
	/* 데이터를 읽을 때 설정값에 따라서 데이터를 조작하여 로드해준다. */
	_uint	iFlag = /*aiProcess_PreTransformVertices | */aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;	

	if (TYPE_NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_eModelType = eModelType;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	MODEL_DESC* pDesc = static_cast<MODEL_DESC*>(pArg);
	
	for (auto& Mesh : m_Meshes)
	{
		Mesh->Set_Pickable(pDesc->bPickable);
	}

    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Input_Assembler();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	_bool	isFinished = { false };
	_bool	animationChanged = { false };

 	if (m_iCurrentAnimIndex != m_iNextAnimIndex)
	{
		animationChanged = true;
		m_iCurrentAnimIndex = m_iNextAnimIndex;
	}

	/* 뼈들의 m_TransformationMatrix를 애니메이터분들이 제공해준 시간에 맞는 뼈의 상태로 갱신해준다. */
	isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones, m_isLoop, m_fSpeedFactor, animationChanged);

	/* 모든 뼈들의 CombinedTransformationMatrix를 셋한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));

	return isFinished;
}

_bool CModel::Picking_Model(const _float4& worldMousePos, const _float3& worldMouseRay,
	_float3& localPickedPos, const _float4x4& WorldMatrix) const
{
	_float		fMinDist = FLT_MAX;
	_bool		bHit = false;

	_matrix		matInvWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix));
	_vector		vLocalOrigin = XMVector3TransformCoord(XMLoadFloat4(&worldMousePos), matInvWorld);
	_vector		vLocalDir = XMVector3TransformNormal(XMLoadFloat3(&worldMouseRay), matInvWorld);
	vLocalDir = XMVector3Normalize(vLocalDir);

	_float3 localMousePos, localMouseRay;
	XMStoreFloat3(&localMousePos, vLocalOrigin);
	XMStoreFloat3(&localMouseRay, vLocalDir);

	for (auto& pMesh : m_Meshes)
	{
		if (false == pMesh->Check_BoundingBox_AABB(localMousePos, localMouseRay))
			continue;

		_float3		localPickedPosition = {};
		_bool		bMeshHit = false;
		_float		fOutDist = {};

		bMeshHit = pMesh->Picking_In_Mesh(localMousePos, localMouseRay, localPickedPosition, fOutDist);

		if (bMeshHit)
		{
			//pMesh->Check_BoundingBox_AABB(localMousePos, localMouseRay);

			if (fOutDist < fMinDist)
			{
				fMinDist = fOutDist;
				localPickedPos = localPickedPosition;
				bHit = true;
			}
		}
	}

	return bHit;
}


_bool CModel::Picking_Vertex(const _float4& worldMousePos, const _float3& worldMouseRay, _float3& vOutPickedVertex, const _float4x4& WorldMatrix) const
{
	_float		fMinDist = FLT_MAX;
	_bool		bHit = false;

	// 월드 -> 로컬 좌표로 마우스 정보 변환
	_matrix		matInvWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix));
	_vector		vLocalOrigin = XMVector3TransformCoord(XMLoadFloat4(&worldMousePos), matInvWorld);
	_vector		vLocalDir = XMVector3TransformNormal(XMLoadFloat3(&worldMouseRay), matInvWorld);
	vLocalDir = XMVector3Normalize(vLocalDir);

	_float3 localMousePos, localMouseRay;
	XMStoreFloat3(&localMousePos, vLocalOrigin);
	XMStoreFloat3(&localMouseRay, vLocalDir);

	for (auto& pMesh : m_Meshes)
	{
		if (false == pMesh->Check_BoundingBox_AABB(localMousePos, localMouseRay))
			continue;

		_float3		vLocalPickedVertice = {};
		_bool		bMeshHit = false;
		_float		fOutDist = {};

		bMeshHit = pMesh->Picking_Vertex(localMousePos, localMouseRay, vLocalPickedVertice, fOutDist, 0.3f);

		if (bMeshHit)
		{
			/*_float3 vWorldPickedPos;
			XMStoreFloat3(&vWorldPickedPos, XMVector3TransformCoord(XMLoadFloat3(&vLocalPickedPos), XMLoadFloat4x4(&WorldMatrix)));*/

			if (fOutDist < fMinDist)
			{
				fMinDist = fOutDist;
				vOutPickedVertex = vLocalPickedVertice;
				bHit = true;
			}
		}
	}
	return bHit;
}


HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;


	/* 현재 렌더링하고자하는 메시의 머테리얼 정보를 얻어온다. */
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eMaterialType, iTextureIndex);	
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}


HRESULT CModel::Ready_Meshes()
{
	/* 메시 파츠의 교체를 용이하게 만들어주기위해서. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		/* VertexBuffer, IndexBuffer */
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_Bones, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], pModelFilePath);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iParentIndex = static_cast<_int>(m_Bones.size()) - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParentIndex);
	} 

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType,
					   const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pGameInstance = new CModel(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Create : CModel");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CComponent* CModel::Clone(void* pArg)
{
	CModel* pGameInstance = new CModel(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CModel::Free()
{
    __super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);

	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

}
