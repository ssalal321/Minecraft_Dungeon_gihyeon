#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
	m_fDuration = static_cast<_float>(pAIAnimation->mDuration);

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool animationChanged)
{
	_bool		isFinished = { false };  // 기본적으로는 루프를 돌도록 false로 설정

	/* 현재 재생위치를 계산하자 */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)  // 애니메이션 끝났을 때
	{
		if (false == isLoop)	// 루프 X
			isFinished = true;
		else                    // 루프 O
		{
			m_fCurrentTrackPosition = 0.f;
		}
	}

	if (animationChanged)
	{
		m_fCurrentTrackPosition = 0.f;
	}

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(m_fCurrentTrackPosition, Bones, animationChanged);
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


void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
