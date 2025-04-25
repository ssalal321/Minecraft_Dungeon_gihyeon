#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN (Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CInventoryIcon final : public CUIObject
{
public:
    CInventoryIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CInventoryIcon() override = default;

public:
    void    Set_Item();
    void    Clear();

private:
    CTexture*           m_pTextureCom = { nullptr };
    CShader*            m_pShaderCom = { nullptr };
    CVIBuffer_Rect*     m_pVIBufferCom = { nullptr };

public:
    static  CInventoryIcon*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject*    Clone(void* pArg)	override;
    void            Free()				override;
};

END