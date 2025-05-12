#pragma once
#include "Client_Defines.h"
#include "Armor.h"

BEGIN(Client)

class CWolf_Armor final : public CArmor
{
public:
    enum PART { MASK, BODY, L_ARM, R_ARM, L_LEG, R_LEG, PART_END };

private:
    CWolf_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWolf_Armor(const CWolf_Armor& Prototype);
    ~CWolf_Armor() override = default;

public:
    HRESULT     Initialize_Prototype() override;
    HRESULT     Initialize(void* pArg) override;
    void        Priority_Update(_float fTimeDelta) override;
    void        Update(_float fTimeDelta) override;
    void        Late_Update(_float fTimeDelta) override;
    HRESULT     Render() override;

private:
    HRESULT     Ready_Components() override;
    HRESULT     Bind_ShaderResources(_uint iPartIndex);

private:
    CTransform*     m_pTransformCom[PART_END] = {};
    CModel*         m_pModelCom[PART_END] = {};
    _float4x4*      m_pSocketMatrix[PART_END] = {};
    _float4x4       m_CombinedWorldMatrix[PART_END] = {};

    _uint           m_iRenderingPartIndex = 0;

public:
    static  CWolf_Armor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject* Clone(void* pArg) override;
    void            Free() override;
};

END
