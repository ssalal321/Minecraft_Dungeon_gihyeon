#pragma once

/* 여러 클래스들의 부모가 된다. */
/* 레퍼런스 카운트의 관리를 담당한다. */
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* 레퍼런스 카운트를 증가시킨다. */
	unsigned int AddRef();

	/* 레퍼런스 카운트를 감소시킨다. or 삭제한다. */
	unsigned int Release();

protected:
	unsigned int				m_iRefCnt = {};

public:
	virtual void Free() {};
};

END