#include "Monster.h"
#include "GameInstance.h"

#include "FSM.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject ( pDevice, pContext )
{

}

CMonster::CMonster(const CMonster& Prototype)
	: CContainerObject( Prototype )
{

}

HRESULT CMonster::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pNavigationCom->SetUp_CurrentCellIndex(0);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	m_pMonsterFSM->Priority_Update_State(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
	m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);

	m_pMonsterFSM->Update_State(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pMonsterFSM->Late_Update_State(fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Change_State(PLAYER_STATE playerState)
{
	m_iState = static_cast<_uint>(playerState);
	m_pMonsterFSM->Change_State(m_StatesVec[m_iState]);
}

HRESULT CMonster::Ready_Components()
{
	/* Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	
	Safe_Delete(m_pMonsterFSM);

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec != nullptr)
			Safe_Release(stateVec);
	}
}
