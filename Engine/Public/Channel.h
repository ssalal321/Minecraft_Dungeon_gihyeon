#pragma once

#include "Base.h"

/* 특정 애니메이션 내에서 사용되는 특정 뼈 하나의 상태를 표현한 객체 */
/* 시간대 별 이 뼈의 상태를 가진다. */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<class CBone*>& Bones);
private:
	_char				m_szName[MAX_PATH] = {};

	_uint				m_iBoneIndex = {};

	/* 이 뼈가 표현해야할 상태의 갯수 */
	_uint				m_iNumKeyFrames = {};

	/* 시간대 별 상태들을 모아놓은거 */
	/* KEYFRAME : 특정 시간대의 상태! */
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = {};



	
public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END