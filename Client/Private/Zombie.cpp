#include "Zombie.h"
#include "GameInstance.h"

#include "Body_Zombie.h"
#include "FSM.h"
#include "Zombie_Idle.h"

CZombie::CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CZombie::CZombie(const CZombie& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CZombie::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CZombie::Initialize(void* pArg)
{
	m_pZombieInfo = new ZOMBIE_DESC(TEXT("GameObject_Zombie"), 10, 10, 2, 5.f, false, 90.f, 3.f);

	if (FAILED(__super::Initialize(m_pZombieInfo)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, -10.f, 1.f));

	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	//m_pNavigationCom->SetUp_CurrentCellIndex(0);

	return S_OK;
}

void CZombie::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CZombie::Update(_float fTimeDelta)
{
	m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);

	__super::Update(fTimeDelta);
}

void CZombie::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CZombie::Render()
{
	return S_OK;
}


HRESULT CZombie::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Zombie::BODY_ZOMBIE_DESC		BodyDesc{};

	BodyDesc.pGameObjectTag = TEXT("GameObject_Body_Zombie");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;

	if (FAILED(__super::Add_PartObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Zombie"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;


	///* 무기를 추가한다. */
	//CWeapon::WEAPON_DESC	WeaponDesc{};

	//CModel* pBody = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	//if (nullptr == pBody)
	//	return E_FAIL;

	//WeaponDesc.pGameObjectTag = TEXT("GameObject_Weapon");
	//WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	//WeaponDesc.pState = &m_iState;
	//WeaponDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_R_Weapon");

	//if (FAILED(__super::Add_PartObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
	//	return E_FAIL;


	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CZombie::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(PLAYER_STATE::STATE_END));	// state vector 자리 예약

	CBody_Zombie*	pBodyZombie = dynamic_cast<CBody_Zombie*>(Find_PartObject(TEXT("Part_Body")));
	CModel*			pZombieModel = dynamic_cast<CModel*>(pBodyZombie->Find_Component(TEXT("Com_Model")));

	m_StatesVec[static_cast<_uint>(PLAYER_STATE::IDLE)] = CZombie_Idle::Create(this, pZombieModel, m_pZombieInfo, m_pTransformCom, m_pNavigationCom);
	// m_StatesVec[static_cast<_uint>(PLAYER_STATE::WALK)] = CZombie_Walk::Create(this, pZombieModel, m_pZombieInfo, m_pTransformCom, m_pNavigationCom);
	// m_StatesVec[static_cast<_uint>(PLAYER_STATE::ROLL)] = CZombie_Roll::Create(this, pZombieModel, m_pZombieInfo, m_pTransformCom, m_pNavigationCom);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(PLAYER_STATE::IDLE)]);

	return S_OK;
}

CZombie* CZombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CZombie* pGameInstance = new CZombie(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CZombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CZombie::Clone(void* pArg)
{
	CZombie* pGameInstance = new CZombie(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CZombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie::Free()
{
	__super::Free();

	//Safe_Release(m_pNavigationCom);
	Safe_Delete(m_pZombieInfo);
	//Safe_Delete(m_pZombieFSM);

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
