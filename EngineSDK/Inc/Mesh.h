#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {	return m_iMaterialIndex;	}

public:
	HRESULT		Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT		Initialize(void* pArg)	override;

private:
	_uint	m_iMaterialIndex = {};

public:
	static CMesh*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	CComponent*		Clone(void* pArg)	override;
	void			Free()				override;

};

END