#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_BoneMatrices[512];

vector g_vLightDir = vector(1.f, -1.f, 0.f, 0.f);
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 0.3f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

// 연출 제어용
float g_fAppearTime;
float g_fAppearDuration;
bool g_bAppearing;

float g_fDeathTime;
float g_fDeathDuration;
bool g_bDying;

sampler g_LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;

    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

// 공통 라이팅 함수 제거, PS만 분리
PS_OUT PS_APPEAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = normalize(In.vWorldPos - g_vCamPosition);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);

    vector vLitColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + g_vLightAmbient * g_vMtrlAmbient)
                     + g_vLightSpecular * g_vMtrlSpecular * fSpecular;

    float fAlpha = saturate(g_fAppearTime / g_fAppearDuration);
    vLitColor.a *= fAlpha;

    Out.vColor = vLitColor;
    return Out;
}

PS_OUT PS_DISAPPEAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    // 죽음엔 discard 안 함

    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = normalize(In.vWorldPos - g_vCamPosition);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);

    vector vLitColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + g_vLightAmbient * g_vMtrlAmbient)
                     + g_vLightSpecular * g_vMtrlSpecular * fSpecular;

    float fAlpha = 1.f - saturate(g_fDeathTime / g_fDeathDuration);
    vLitColor.a *= fAlpha;

    Out.vColor = vLitColor;
    return Out;
}

PS_OUT PS_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = normalize(In.vWorldPos - g_vCamPosition);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);

    vector vLitColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + g_vLightAmbient * g_vMtrlAmbient)
                     + g_vLightSpecular * g_vMtrlSpecular * fSpecular;

    vLitColor.a = 1.f;

    Out.vColor = vLitColor;
    return Out;
}


technique11 BossTechnique
{
    pass Appear
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader  = compile ps_5_0 PS_APPEAR();
    }

    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader  = compile ps_5_0 PS_DEFAULT();
    }

    pass Disappear
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader  = compile ps_5_0 PS_DISAPPEAR();
    }
}
