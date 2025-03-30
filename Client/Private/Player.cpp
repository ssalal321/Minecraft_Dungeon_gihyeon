#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Weapon.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject(Prototype)
{

}

HRESULT CPlayer::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.pGameObjectTag = TEXT("GameObject_Player");
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}
	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_iState & STATE_IDLE)
			m_iState ^= STATE_IDLE;

		m_iState |= STATE_RUN;
	}

	else
		m_iState = STATE_IDLE;


	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Player::BODY_PLAYER_DESC		BodyDesc{};

	BodyDesc.pGameObjectTag = TEXT("GameObject_Body_Player");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;

	if (FAILED(__super::Add_PartObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Player"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;


	/* 무기를 추가한다. */
	CWeapon::WEAPON_DESC				WeaponDesc{};

	CModel* pBody = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr == pBody)
		return E_FAIL;

	WeaponDesc.pGameObjectTag = TEXT("GameObject_Weapon");
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pState = &m_iState;
	WeaponDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_R_Weapon_end");

	if (FAILED(__super::Add_PartObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;


	/* 이펙트를 추가한다. */

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pGameInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pGameInstance = new CPlayer(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer::Free()
{
	__super::Free();

}
