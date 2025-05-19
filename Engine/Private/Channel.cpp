#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
	{
		if (0 == strcmp(pBone->Get_Name(), m_szName))
			return true;

		++m_iBoneIndex;

		return false;
	});
	
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3		vScale{};
	_float4		vRotation{};
	_float3		vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
		}
		
		if (i < pAIChannel->mNumRotationKeys)
		{
			/*memcpy(&KeyFrame.vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));*/
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

    return S_OK;
}

void CChannel::Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _bool animationChanged, _float fTimeDelta)
{
    if (fCurrentTrackPosition == 0.f)
        *pCurrentKeyFrameIndex = 0;

    _vector vScale, vRotation, vTranslation;
    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    const _float fBlendDuration = 0.2f;

    if (animationChanged)
    {
        m_bInBlending = true;
        m_fBlendElapsedTime = 0.f;

        // 현재 뼈의 기존 최종 행렬 기준 보간 시작값 저장
        XMStoreFloat4x4(&m_PrevTransformMatrix, Bones[m_iBoneIndex]->Get_TransformationMatrix());
        XMMatrixDecompose(&m_vLeftScale, &m_vLeftRotation, &m_vLeftTranslation, XMLoadFloat4x4(&m_PrevTransformMatrix));
    }

    if (m_bInBlending)
    {
        m_fBlendElapsedTime += fTimeDelta;
        _float fRatio = m_fBlendElapsedTime / fBlendDuration;
        fRatio = min(max(fRatio, 0.f), 1.f);

        // 오른쪽(새 애니메이션의 첫 키프레임)
        _vector vRightScale = XMLoadFloat3(&m_KeyFrames[0].vScale);
        _vector vRightRotation = XMQuaternionNormalize(XMLoadFloat4(&m_KeyFrames[0].vRotation));
        _vector vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[0].vTranslation), 1.f);

        m_vLeftRotation = XMQuaternionNormalize(m_vLeftRotation);

        vScale = XMVectorLerp(m_vLeftScale, vRightScale, fRatio);
        vRotation = XMQuaternionSlerp(m_vLeftRotation, vRightRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(m_vLeftTranslation, vRightTranslation, fRatio), 1.f);

        if (fRatio >= 1.f)
        {
            m_bInBlending = false;
        }
    }
    else if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        // 마지막 키프레임 유지
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMQuaternionNormalize(XMLoadFloat4(&LastKeyFrame.vRotation));
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);

        *pCurrentKeyFrameIndex = m_iNumKeyFrames - 1;
    }
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
            ++(*pCurrentKeyFrameIndex);

        _float t1 = m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition;
        _float t2 = m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition;
        _float fRatio = (fCurrentTrackPosition - t1) / (t2 - t1);
        fRatio = min(max(fRatio, 0.f), 1.f);

        _vector vLeftScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        _vector vRightScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

        _vector vLeftRotation = XMQuaternionNormalize(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation));
        _vector vRightRotation = XMQuaternionNormalize(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation));

        _vector vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
        _vector vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

        vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);
        vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio), 1.f);
    }

    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}




CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pGameInstance = new CChannel();

	if (FAILED(pGameInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed to Create : CChannel");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CChannel::Free()
{
    __super::Free();

	m_KeyFrames.clear();
}
