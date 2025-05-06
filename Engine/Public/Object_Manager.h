#pragma once

#include "Base.h"
/* 게임내에 사용하기위한 사본객체들을 레벨별로 레이어로 구분하여 보관한다. */

namespace Engine
{
	class CLayer;
	class CComponent;
	class CGameObject;
}

BEGIN(Engine)
	class CObject_Manager final : public CBase
{
public:
	struct OBJECT_BACKUP_DATA
	{
		void* pObjectData1 = nullptr;
		void* pObjectData2 = nullptr;
	};

private:
	CObject_Manager();
	~CObject_Manager() override = default;

public:
	CGameObject*	Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
							   _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	//CComponent*		Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	CGameObject*	Find_GameObject(const _wstring& strGameObjectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	CLayer*			Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	void			Set_Object_Backup_Data(void* pObjectData1, void* pObjectData2);
	HRESULT			Set_Layer_Persistent(_uint iLevelIndex, const wstring& strLayerTag);
	CLayer*			Get_Persistent_Layer(const wstring& strLayerTag);
	HRESULT			Attach_Persistent_Layer_To_Level(_uint iTargetLevelIndex, const wstring& strLayerTag);
	HRESULT			Attach_Persistent_Layers_To_Level(_uint iLevelIndex);

	//unordered_map<_wstring, CGameObject*>*	Get_LayerObjects(_uint iLayerLevelIndex, const _wstring& strLayerTag);

public:
	HRESULT		Initialize(_uint iNumLevels);
	void		Priority_Update(_float fTimeDelta);
	void		Update(_float fTimeDelta);
	void		Late_Update(_float fTimeDelta);
	void		Clear(_uint iLevelIndex);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	_uint					m_iNumLevels = {};

	map<const _wstring, CLayer*>*		m_pLayers			= { nullptr };
	map<const _wstring, CLayer*>*		m_pPersistentLayers = { nullptr };

	OBJECT_BACKUP_DATA		m_tObjectDataBackup;

private:
	CGameObject*	Add_To_Layer(CGameObject* pGameObject, _uint iLayerLevelIndex,
								 const _wstring& strLayerTag, const _wstring& strGameObjectTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	void	Free()		override;
};


END
