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
    typedef struct INVENTORY_ICON_DESC : public CUIObject::UIOBJECT_DESC
    {
        INVENTORY_ICON_DESC(const wstring& GameObjectTag, UI_STATE uiState,
            _float x, _float y, _float z, _float sizeX, _float sizeY,
            const wstring& textureTag, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
            : UIOBJECT_DESC(GameObjectTag, uiState, x, y, z, sizeX, sizeY, textureTag,
                speedPerSec, rotationPerSec)  // 부모 생성자 호출
           { }

        //// 복사 생성자
        //INVENTORY_ICON_DESC(const INVENTORY_ICON_DESC& other)
        //	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

        ~INVENTORY_ICON_DESC() override = default;

    }INVENTORY_ICON_DESC;

private:
    CInventoryIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CInventoryIcon() override = default;

public:
    HRESULT		Initialize_Prototype()					override;
    HRESULT		Initialize(void* pArg)					override;
    void		Priority_Update(_float fTimeDelta)		override;
    void		Update(_float fTimeDelta)				override;
    void		Late_Update(_float fTimeDelta)			override;
    HRESULT		Render()								override;

public:

private:
    INVENTORY_ICON_DESC*    m_pDesc     = { nullptr };

    CTexture*           m_pTextureCom   = { nullptr };
    CShader*            m_pShaderCom    = { nullptr };
    CVIBuffer_Rect*     m_pVIBufferCom  = { nullptr };

private:
    HRESULT		Ready_Components();

public:
    static  CInventoryIcon*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject*    Clone(void* pArg)	override;
    void            Free()				override;
};

END