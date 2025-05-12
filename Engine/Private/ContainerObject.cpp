#include "ContainerObject.h"
#include "PartObject.h"
#include "GameInstance.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Update(fTimeDelta);
	}		
}

void CContainerObject::Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Update(fTimeDelta);
	}
}

void CContainerObject::Late_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Update(fTimeDelta);
	}
}

HRESULT CContainerObject::Render()
{
	return S_OK;
}

CPartObject* CContainerObject::Find_PartObject(const _wstring& strPartObjectTag)
{
	auto	iter = m_PartObjects.find(strPartObjectTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;	
}

CComponent* CContainerObject::Find_Part_Component(const _wstring& strPartObjectTag, const _wstring& strComponentTag)
{
	CPartObject* pPartObject = Find_PartObject(strPartObjectTag);
	if (nullptr == pPartObject)
		return nullptr;

	CComponent* pPartComponent = pPartObject->Find_Component(strComponentTag);
	if (nullptr == pPartComponent)
		return nullptr;

	return pPartComponent;
}

HRESULT CContainerObject::Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjectTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPartObjectTag))
		return E_FAIL;

	CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTOTYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartObjectTag, pPartObject);

	return S_OK;
}

HRESULT CContainerObject::Add_PartObject(CPartObject* pPartObject, const _wstring& strPartObjectTag)
{
	if (nullptr != Find_PartObject(strPartObjectTag))
		return E_FAIL;

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartObjectTag, pPartObject);

	return S_OK;
}


HRESULT CContainerObject::Delete_PartObject(const _wstring& strPartObjectTag, _bool bReleaseMemory)
{
	auto iter = m_PartObjects.find(strPartObjectTag);

	if (iter == m_PartObjects.end())
		return E_FAIL; // 못 찾았으면 실패

	if (bReleaseMemory)
		Safe_Release(iter->second); // 메모리 해제

	m_PartObjects.erase(iter);  // 맵에서 제거

	return S_OK;
}


// 겹침 해소 + 슬라이딩 처리
void CContainerObject::Resolve_Penetration_And_Slide(CCollider* pOther, _float fForce)
{
	if (pOther == nullptr)
		return;

	CTransform* pOtherTransform = dynamic_cast<CTransform*>(pOther->Get_OwnerObject()->Find_Component(TEXT("Com_Transform")));
	if (!pOtherTransform)
		return;

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vOtherPos = pOtherTransform->Get_State(CTransform::STATE_POSITION);

	_vector vToOther = vMyPos - vOtherPos;
	_float fDist = XMVectorGetX(XMVector3Length(vToOther));

	// 구체 반지름 (필요 시 파라미터로)
	const _float fRadiusSelf = 0.8f;
	const _float fRadiusOther = 0.8f;
	const _float fMinDistance = fRadiusSelf + fRadiusOther;

	if (fDist < fMinDistance && fDist > 0.001f)
	{
		_vector vDir = XMVector3Normalize(vToOther);
		_vector vSeparation = vDir * (fMinDistance - fDist + 0.001f);

		// 나만 밀림
		_vector vNewPos = vMyPos + vSeparation;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
	}
	else if (fDist < 0.001f)
	{
		// 같은 위치일 경우 랜덤 또는 고정 방향으로 약간 밀기
		_vector vPush = XMVectorSet(0.01f, 0.f, 0.01f, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMyPos + vPush);
	}

	// 슬라이딩 처리 (Look 방향 기준)
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vSlideDir = XMVector3Normalize(vLook - XMVector3Dot(vLook, XMVector3Normalize(vToOther)) * XMVector3Normalize(vToOther));

	_vector vSlideTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vSlideDir * fForce;

	CNavigation* pNav = dynamic_cast<CNavigation*>(Find_Component(TEXT("Com_Navigation")));
	if (pNav)
	{
		_vector vResult;
		if (pNav->Can_Move(vSlideTarget))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSlideTarget);
		else if (pNav->Can_Slide(vMyPos, vSlideTarget, vResult))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResult);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSlideTarget);
	}
}


void CContainerObject::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();
}
