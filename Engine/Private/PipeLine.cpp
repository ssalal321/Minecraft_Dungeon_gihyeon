#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

void CPipeLine::Update()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i], 
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrix[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));

}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine;
}

void CPipeLine::Free()
{
	__super::Free();

}
