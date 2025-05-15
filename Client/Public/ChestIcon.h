#pragma once
#include "Client_Defines.h"
#include "Item.h"
#include "UIObject.h"

BEGIN (Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CChestIcon final : public CGameObject
{
public:
    typedef struct CHEST_ICON_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _uint       iPrototypeLevelIndex;
        _float4     worldPosition = { 0.f, 0.f, 0.f, 1.f };

        CItem::ITEM_DESC   ItemDesc = {};

        //// 복사 생성자
        //CHEST_ICON_DESC(const CHEST_ICON_DESC& other)
        //	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

        ~CHEST_ICON_DESC() override = default;

    }CHEST_ICON_DESC;

private:
    CChestIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CChestIcon() override = default;

public:
    CHEST_ICON_DESC*    Get_Icon_Desc() const { return m_pDesc; }

    void    Set_ColliderActivated(_bool bActivateCollider) { m_bActivated = bActivateCollider; }

public:
    HRESULT		Initialize_Prototype()					override;
    HRESULT		Initialize(void* pArg)					override;
    void		Priority_Update(_float fTimeDelta)		override;
    void		Update(_float fTimeDelta)				override;
    void		Late_Update(_float fTimeDelta)			override;
    HRESULT		Render()								override;

private:
    CHEST_ICON_DESC*    m_pDesc     = { nullptr };

    CTexture*           m_pTextureCom   = { nullptr };
    CCollider*          m_pColliderCom = { nullptr };
    CShader*            m_pShaderCom    = { nullptr };
    CVIBuffer_Rect*     m_pVIBufferCom  = { nullptr };

    _bool			    m_bActivated = { true };
    _float4x4		    m_IdentityWorldMatrix = {};
    _wstring		    m_strTexPrototypeTag = {};

private:
    HRESULT		Ready_Components();
    HRESULT     Bind_ShaderResources();

public:
    static  CChestIcon*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    CGameObject*    Clone(void* pArg)	override;
    void            Free()				override;
};

END