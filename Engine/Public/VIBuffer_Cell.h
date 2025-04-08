#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
protected:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& Prototype);
	~CVIBuffer_Cell() override = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	HRESULT Initialize(void* pArg) override;


public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	CComponent* Clone(void* pArg)	override;
	void	Free()					override;

};

END