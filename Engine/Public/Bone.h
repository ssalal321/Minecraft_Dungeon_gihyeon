#pragma once

/* 뼈의 상속관계를 표현하고 상태를 정의하기위한 뼈의 정보. aiNode */
/* 정점들의 데이터를 채워주기위한 뼈의 정보. aiBone */
/* 애니메이션을 표현하기 위한 뼈의 정보. aiNodeAnim */

#include "Base.h"

/* aiNode */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName; 
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones);

private:
	/* 뼈 이름 : 특정 뼈를 지목해서 수행해야할 일들이 많아.  */
	_char					m_szName[MAX_PATH] = {};

	/* 내 뼈만의 상태행렬. */
	/* 부모의 상태변환행렬이 곱해 질 것이기 때문에 m_TransformationMatrix이 표현하고 있는상태 변환은 */
	/* 부모 뼈기준의 상태 변환을 이야기한다. */
	_float4x4				m_TransformationMatrix = {};

	/*m_CombinedTransformationMatrix = 내 상태변환 행렬(m_TransformationMatrix) * 부모`s m_CombinedTransformationMatrix */
	_float4x4				m_CombinedTransformationMatrix = {};

	
	/* 부모 뼈의 인덱스 */
	/* 인덱스 : 모델의 전체 뼈 중. */
	_int					m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	virtual void Free() override;
};

END