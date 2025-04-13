#pragma once

#include <unordered_set>

#include "Component.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	~CNavigation() override = default;

public:
	HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	HRESULT		Initialize_Prototype()		override;
	HRESULT		Initialize(void* pArg)		override;
	void		Update(const _float4x4* pWorldMatrix);

	void	SetUp_CurrentCellIndex(_int iCellIndex)
	{
		m_iCurrentCellIndex = iCellIndex;
	}

public:
	std::string Make_Cell_Key(const _float3* fCellPoints) const;

	HRESULT		Make_Cell(const _float3* fCellPoints);
	void		Erase_Cell_Pick(const _matrix& WorldMatrixInverse);
	void		Erase_Cell_Last();

	HRESULT		Read_Cell(const _tchar* pNavigationDataFilePath);
	void		Resave_Files();
	void		Sort_Clockwise(const _float3* pInPoints, _float3* pOutSorted);
	_bool		Can_Move(_fvector vWorldPos);
	HRESULT		SetUp_Neighbors();
	void		SetUp_On_Navigation(CTransform* pTransform);

#ifdef _DEBUG
public:
	HRESULT		Render();
#endif

private:
	_int							m_iPointNum = {};
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>			m_Cells;

	static const _float4x4*			m_pWorldMatrix;

	static	_bool					m_bLineRender;
	static	_uint					m_iShaderPass;  // ¼±¾ð

	std::unordered_set<std::string>		m_TriangleSet = {};

#ifdef _DEBUG
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg) override;
	void  Free()	override;
};


END
