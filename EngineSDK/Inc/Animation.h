#pragma once

#include "Base.h"

/* 특정 애니메이션의 시작과 끝을 관리한다. */

/* 애니메이션의 0시간에서부터 끝날때까지 */ 
/* 어떤 뼈를 움직여야 하는가 ? */
/* 뼈들은 시간에 따라 어떤 상태를 취해야하는가? */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT		Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	_bool		Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones,
											  _bool isLoop, _bool animationChanged);

private:
	_char					m_szName[MAX_PATH] = {};
	_float					m_fTickPerSecond = {}; /* 초당 재생 속도 */
	_float					m_fDuration = {}; /* 애니메이션의 총 길이 */
	_float					m_fCurrentTrackPosition = {}; /* 현재 재생 위치. */

	_uint					m_iNumChannels = {}; /* 이 애니메이션을 구동하기위해 필요한 뼈의 갯수. */

	/* 각 뼈의 정보를 가진다. */
	/* 정보 : 해당 뼈가 시간별로 취해야할 상태들. */
	vector<class CChannel*>	m_Channels;

	_bool			m_StartLerp = { false };

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END