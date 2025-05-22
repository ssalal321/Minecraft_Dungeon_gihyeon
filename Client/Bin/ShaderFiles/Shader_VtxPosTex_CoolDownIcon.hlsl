#include "Engine_Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_IconTexture;

float       g_fCooldownRatio = 0.f; // 1.0 -> 쿨타임 시작, 0.0 -> 쿨타임 끝

sampler g_LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_CooldownIcon(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector texColor = g_IconTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (g_fCooldownRatio <= 0.f)
    {
        Out.vColor = texColor;
        return Out;
    }

    if (texColor.a < 0.1f)
        discard;

    // g_fCooldownRatio: 0.0 (쿨끝) ~ 1.0 (쿨 시작)
    // 기준선보다 위는 잘라냄 (투명)
    if (In.vTexcoord.y < (1.0f - g_fCooldownRatio))
    {
        discard;
    }
    // 흰색 바 영역 (테두리 느낌)
    else if (In.vTexcoord.y < (1.0f - g_fCooldownRatio) + 0.04f)
    {
        Out.vColor = float4(1.f, 1.f, 1.f, 0.9f);
    }
    // 회색 아이콘
    else
    {
        float gray = dot(texColor.rgb, float3(0.299f, 0.587f, 0.114f));
        Out.vColor = float4(gray * 0.6f, gray * 0.6f, gray * 0.6f, texColor.a);
    }

    return Out;
}



technique11 CooldownIconTechnique
{
    pass Cooldown
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_CooldownIcon();
    }
}
