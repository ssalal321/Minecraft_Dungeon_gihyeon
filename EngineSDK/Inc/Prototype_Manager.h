#pragma once

/* For.Component Headers */
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"

/* GameObject, Component들의 원형 객체들을 보관하기 위한 클래스이다 */
/* 원형 객체들을 보관 시, 레벨별로 구분하여 보관한다. */

/* 선택한 원형객체를 복제하여 리턴한다. */

BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT		Initialize(_uint iNumLevels);
	HRESULT		Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*		Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	CBase*		Clone_UIPrototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const wstring& strTexturePrototypeTag, void* pArg);
	void		Clear(_uint iLevelIndex);

private:
	_uint								m_iNumLevels = { 0 };
	map<const _wstring, class CBase*>*	m_pPrototypes = { nullptr };

private:
	class	CBase*	Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static	  CPrototype_Manager*	Create(_uint iNumLevels);
	virtual   void					Free() override;
};

END