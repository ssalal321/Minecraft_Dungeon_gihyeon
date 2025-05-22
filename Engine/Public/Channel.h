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
	HRESULT		Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void		Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _bool animationChanged, _float fTimeDelta);
private:
	_char				m_szName[MAX_PATH] = {};

	_uint				m_iBoneIndex = {};

	/* 이 뼈가 표현해야할 상태의 갯수 */
	_uint				m_iNumKeyFrames = {};

	_bool			m_AnimationChanged = {};
	_bool			m_StartedLerp = { false };
	_vector			m_vLeftScale, m_vLeftRotation, m_vLeftTranslation = {};
	_matrix			PreviousTransformMatrix = {};
	/* 시간대 별 상태들을 모아놓은거 */
	/* KEYFRAME : 특정 시간대의 상태! */
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = {};

	_float4x4 m_PrevTransformMatrix = {};

	_float	 m_fBlendElapsedTime = 0.f;
	_bool	 m_bInBlending = false;

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END