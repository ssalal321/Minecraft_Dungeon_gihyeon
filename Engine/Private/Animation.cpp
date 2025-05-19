#include "Animation.h"

#include <iostream>
#include <ostream>

#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fCurrentTrackPosition(Prototype.m_fCurrentTrackPosition)
	, m_fDuration(Prototype.m_fDuration)
	, m_fTickPerSecond(Prototype.m_fTickPerSecond)
	, m_iNumChannels(Prototype.m_iNumChannels)
	, m_Channels(Prototype.m_Channels)
	, m_ChannelCurrentKeyFrameIndices{ Prototype.m_ChannelCurrentKeyFrameIndices }
	, m_StartLerp(Prototype.m_StartLerp)
{
	strcpy_s(m_szName, Prototype.m_szName);

	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
	m_fDuration = static_cast<_float>(pAIAnimation->mDuration);

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_ChannelCurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _float speedFactor, _bool animationChanged)
{
	_bool		isFinished = { false };  // 기본적으로는 루프를 돌도록 false로 설정

	if (animationChanged)
	{
		m_fCurrentTrackPosition = 0.f;
	}
	/* 현재 재생위치를 계산하자 */
	else
	{
		m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * speedFactor;

		if (m_fCurrentTrackPosition >= m_fDuration)  // 애니메이션 끝났을 때
		{
			if (false == isLoop)	// 루프 X
				isFinished = true;
			else                    // 루프 O
			{
				m_fCurrentTrackPosition = 0.f;
				isFinished = true;  // 루프 O지만 어쨌든 한 타임 끝났으니까 true 반환
			}
		}
	}

	_uint		iChannelIndex = { 0 };

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(m_fCurrentTrackPosition, Bones, &m_ChannelCurrentKeyFrameIndices[iChannelIndex++], animationChanged, fTimeDelta);
	}

	return isFinished;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pGameInstance = new CAnimation();

	if (FAILED(pGameInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}


void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
