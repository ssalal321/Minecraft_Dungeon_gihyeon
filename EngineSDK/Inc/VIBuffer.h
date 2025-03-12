#pragma once

#include "Component.h"

/* VertexBuffer + IndexBuffer */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Input_Assembler();
	virtual HRESULT Render();

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC			m_BufferDesc{};
	D3D11_SUBRESOURCE_DATA		m_InitialDesc{};
	_uint						m_iVertexStride = {};
	_uint						m_iNumVertices = {};
	_uint						m_iNumVertexBuffers = {};
	_uint						m_iIndexStride = {};
	_uint						m_iNumIndices = {};
	DXGI_FORMAT					m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {};

protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END