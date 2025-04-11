#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent ( pDevice, pContext )
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	: CComponent( Prototype )
	, m_pVB { Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_iNumVertexBuffers{ Prototype.m_iNumVertexBuffers }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_eIndexFormat{ Prototype.m_eIndexFormat }
	, m_ePrimitiveTopology{ Prototype.m_ePrimitiveTopology }

{
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pVB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}


// 버텍스 버퍼를 바인딩하여 정점 데이터를 설정
// 인덱스 버퍼를 바인딩하여 정점의 연결 순서 지정
// 프리미티브 토폴로지를 설정하여 도형의 렌더링 방식을 결정
HRESULT CVIBuffer::Input_Assembler()
{
	/* 그리고자하는 자원들을 장치에 바인딩한다. */
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint		iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);


	//m_pDevice->CreateInputLayout(, , )
	//m_pContext->IASetInputLayout();

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	
	/* 그려라! */
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppOut)
{
	/*D3D11_SUBRESOURCE_DATA*/
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialDesc, ppOut);	
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);


}
