#include "Loader.h"

#include "BabyZombie.h"
#include "Body_BabyZombie.h"
#include "Player_Arrow.h"
#include "GameInstance.h"

#include "Camera_Free.h"
#include "Body_Player.h"
#include "Body_Skeleton.h"
#include "Body_Slime_Large.h"
#include "Body_Slime_Medium.h"
#include "Body_Slime_Small.h"
#include "Body_Zombie.h"
#include "InventoryBase.h"
#include "InventoryGearSlot.h"
#include "InventoryArtifactSlot.h"
#include "InventoryIcon.h"
#include "InventoryStoreSlot.h"
#include "LoungeMap.h"
#include "Monster_Arrow.h"
#include "Sky.h"
#include "Player.h"
#include "UI_Image.h"
#include "PlayerHP.h"
#include "Skeleton.h"
#include "Slime_Large.h"
#include "Slime_Medium.h"
#include "Slime_Small.h"
#include "SoggySwampMap.h"
#include "Weapon_Bow.h"
#include "Weapon_Glaive.h"
#include "Weapon_ShortBow.h"
#include "WolfArmor_Body.h"
#include "WolfArmor_L_Arm.h"
#include "WolfArmor_L_Leg.h"
#include "WolfArmor_Mask.h"
#include "WolfArmor_R_Arm.h"
#include "WolfArmor_R_Leg.h"
#include "Wolf_Armor.h"
#include "Zombie.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint LoadingMain(void* pArg)
{
	CLoader*		pLoader = static_cast<CLoader*>(pArg);
	if (nullptr == pLoader)
		return -1;

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevelID = eNextLevelID;

	/* 스레드를 생성한다 .*/
	/* 생성한 스레드가 eNextLevelID를 준비할 수 있도록 한다 .*/
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLoader:: Loading()
{
	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_TITLE:
		hr = Loading_For_Static(); // UI_Image 원형 저장단계
		if (FAILED(hr))
			return E_FAIL;

		hr = Loading_For_Title();
		break;

	case LEVEL_LOUNGE:
		hr = Loading_For_Lounge();
		break;

	case LEVEL_SOGGYSWAMP:
		hr = Loading_For_SoggySwamp();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK; 
}

void CLoader::Show_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);	
}

_bool CLoader::Is_Finished()
{
	return m_isFinished;
}

HRESULT CLoader::Loading_For_Static()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중"));
	if (FAILED(Ready_Prototype_TextureCom_Static()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중"));
	if (FAILED(Ready_Prototype_ShaderCom_Static()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼 로딩 중"));
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중"));
	if (FAILED(Ready_Prototype_ModelCom_Static()))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션 로딩 중"));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_LoungeMap"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Lounge_Navigation.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션 로딩 중"));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_SoggySwampMap"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/SoggySwamp_Navigation.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("콜라이더 로딩 중"));
	if (FAILED(Ready_Prototype_ColliderCom_Static()))
		return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("오브젝트 로딩 중"));
	if (FAILED(Ready_Prototype_GameObject_Static()))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLoader::Loading_For_Title()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중"));
	/* For.Prototype_Component_Texture_TitleImage */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TITLE, TEXT("Prototype_Component_Texture_TitleImage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Title/LoadingTitle.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Lounge()
{
	///* For.Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOUNGE, TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_SoggySwamp()
{
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_TextureCom_Static()
{
	/* For.Prototype_Component_Texture_PlayerStateSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerStateSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerStateSlot.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PlayerHP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerHP.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_InventoryBase */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/InventoryBase.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_InventoryStoreSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryStoreSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/InventoryStoreSlot.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_InventoryGearSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryGearSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/InventoryGearSlot.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_InventoryArtifactSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryArtifactSlot_Empty"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/InventoryArtifactSlot_Empty.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Glaive_Steel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Glaive_Steel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inventory/T_Glaive_Icon_inventory.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ShortBow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShortBow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inventory/T_ShortBow_Icon_inventory.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Bow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inventory/T_Bow_Icon_inventory.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Bow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WolfArmor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inventory/T_WolfArmor_Icon_inventory.dds"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_ShaderCom_Static()
{
	/* For.Prototype_Component_Shader_VtxPosTex_HPbar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_HPbar"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_HPbar.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_ModelCom_Static()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_PlayerHex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PlayerHex"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/PlayerHex.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Zombie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zombie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Zombie/Zombie.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	/* For.Prototype_Component_Model_BabyZombie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BabyZombie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Zombie/BabyZombie.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Slime_Large */
	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slime_Large"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Slime/Slime_Large.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Skeleton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Skeleton/Skeleton.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Slime_Medium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slime_Medium"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Slime/Slime_Medium.fbx", PreTransformMatrix))))
		return E_FAIL;

	//PreTransformMatrix = XMMatrixRotationAxis({0.f, 0.1, 0.f}, 90.f);
	/* For.Prototype_Component_Model_Slime_Small */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slime_Small"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Slime/Slime_Small2.fbx", PreTransformMatrix))))
		return E_FAIL;

	//PreTransformMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_GlaiveSteel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GlaiveSteel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapon/Glaive/GlaiveSteel.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShortBow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShortBow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapon/ShortBow/ShortBow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapon/Bow/Bow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Arrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapon/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Armors_Static()))
		return E_FAIL;

	/* For.Prototype_Component_Model_LoungeMap */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LoungeMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Map/Lounge/Lobby_NoGrass_Split.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SoggySwampMap */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SoggySwampMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Map/SoggySwamp/SoggySwamp.fbx", PreTransformMatrix))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Ready_Prototype_ColliderCom_Static()
{
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER_TYPE::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER_TYPE::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER_TYPE::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_GameObject_Static()
{
	/* For.Prototype_GameObject_UIImage */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UIImage"),
		CUI_Image::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_HPbar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Player_HPbar"),
		CPlayerHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InventoryBase */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_InventoryBase"),
		CInventoryBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InventoryStoreSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_InventoryStoreSlot"),
		CInventoryStoreSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InventoryGearSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_InventoryGearSlot"),
		CInventoryGearSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InventoryArtifactSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_InventoryArtifactSlot_Empty"),
		CInventoryArtifactSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InventoryArtifactSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_InventoryIcon"),
		CInventoryIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerHex"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Zombie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Zombie"),
		CBody_Zombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_BabyZombie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_BabyZombie"),
		CBody_BabyZombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Skeleton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Skeleton"),
		CBody_Skeleton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Slime_Large */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Large"),
		CBody_Slime_Large::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Slime_Medium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Medium"),
		CBody_Slime_Medium::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Slime_Small */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Small"),
		CBody_Slime_Small::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Zombie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
		CZombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BabyZombie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
		CBabyZombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skeleton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
		CSkeleton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Slime_Large */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Large"),
		CSlime_Large::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Slime_Large */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Medium"),
		CSlime_Medium::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Slime_Large */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Small"),
		CSlime_Small::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Glaive_Steel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Glaive_Steel"),
		CWeapon_Glaive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShortBow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ShortBow"),
		CWeapon_ShortBow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Bow"),
		CWeapon_Bow::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerArrow"),
		CPlayer_Arrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MonsterArrow"),
		CMonster_Arrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoungeMap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_LoungeMap"),
		CLoungeMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoggySwampMap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_SoggySwampMap"),
		CSoggySwampMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Ready_Prototype_Armors_Static()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_WolfArmor_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_Mask"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armor/WolfArmor/WolfArmor_Mask.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WolfArmor_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_Body"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armor/WolfArmor/WolfArmor_FurArmor.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WolfArmor_L_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_L_Arm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armor/WolfArmor/WolfArmor_L_Arm.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WolfArmor_R_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_R_Arm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armor/WolfArmor/WolfArmor_R_Arm.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WolfArmor_L_Leg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_L_Leg"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armor/WolfArmor/WolfArmor_L_Leg.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WolfArmor_R_Leg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_R_Leg"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armor/WolfArmor/WolfArmor_R_Leg.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_GameObject_WolfArmor_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_Mask"),
		CWolfArmor_Mask::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WolfArmor_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_Body"),
		CWolfArmor_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WolfArmor_L_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_L_Arm"),
		CWolfArmor_L_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WolfArmor_R_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_R_Arm"),
		CWolfArmor_R_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WolfArmor_L_Leg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_L_Leg"),
		CWolfArmor_L_Leg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WolfArmor_R_Leg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_R_Leg"),
		CWolfArmor_R_Leg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WolfArmor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor"),
		CWolf_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}


CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pGameInstance = new CLoader(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
