#pragma once

#include <unordered_set>

#include "Component.h"

BEGIN(Engine)
	class CCell;
	class CTransform;

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	~CNavigation() override = default;

	enum WALKABLE_TYPE
	{
		WALKABLE = 0,
		WALL = 1,
		INVALID = 2
	};

public:
	HRESULT		Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	HRESULT		Initialize_Prototype()		override;
	HRESULT		Initialize(void* pArg)		override;
	void		Update(const _float4x4* pWorldMatrix);
	
	_int	Get_CurrentCellIndex() const { return m_iCurrentCellIndex; }

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
	_int		Find_CellIndex(_vector vWorldPos);

	_bool		Can_Move(_fvector vMovingWorldPos);
	_bool		Can_Slide(_fvector vPrevWorldPos, _fvector vMovingWorldPos, _vector& vSlidingPosition);
	HRESULT		SetUp_Neighbors();
	void		SetUp_On_Navigation(CTransform* pTransform);
	_bool		Check_If_Grounded(CTransform* pObjectTransformCom);


	void		Lock_Cell(_int iCellIndex) { m_LockedCells.insert(iCellIndex); }
	void		Unlock_Cell(_int iCellIndex) { m_LockedCells.erase(iCellIndex); }
	_bool		Is_Locked(_int iCellIndex) const {
		return m_LockedCells.find(iCellIndex) != m_LockedCells.end();
	}


#ifdef _DEBUG
public:
	HRESULT		Render();
#endif

private:
	_bool			Decided_Next_Index(_int iCurIndex, _int iNeighborIndex, _int* pOutNextIndex);
	WALKABLE_TYPE	Classify_Cell(CCell* pCell) const;
	_bool Is_Wall(_float normal) const 
	{
		return (normal < 0.5f);
	}


private:
	_int							m_iPointNum = {};
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>			m_Cells;

	static const _float4x4*			m_pWorldMatrix;

#ifdef _DEBUG
	static	_bool					m_bLineRender;
#endif
	static	_uint					m_iShaderPass;  // ¼±¾ð

	std::unordered_set<std::string>		m_TriangleSet = {};

	std::unordered_set<_int>			m_LockedCells;  // Àá±ä ¼¿

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
