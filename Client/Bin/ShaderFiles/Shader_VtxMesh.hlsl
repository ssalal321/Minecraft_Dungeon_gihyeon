#include "Engine_Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector      g_vLightDir = vector(1.f, -1.f, 0.f, 0.f);
vector      g_vLightPos;
float       g_fLightRange;
vector      g_vLightDiffuse;
vector      g_vLightAmbient;
vector      g_vLightSpecular;

float       g_fAppearTime;
float       g_fAppearDuration;

float       g_fDeathTime;
float       g_fDeathDuration;


texture2D   g_DiffuseTexture;
vector      g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 0.3f);
vector      g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector      g_vCamPosition;

sampler g_LinearSampler = sampler_state 
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT    
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;    
    float4 vWorldPos : TEXCOORD1;
};

// 리턴값 VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
VS_OUT VS_MAIN(VS_IN In)
{
    /* 기타 변환들을 수행한다.*/   
    VS_OUT Out = (VS_OUT)0;
    
    matrix matWV, matWVP;
    
    matWV   = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP  = mul(matWV, g_ProjMatrix);
    
    Out.vPosition   = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal     = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord   = In.vTexcoord;
    Out.vWorldPos   = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
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


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    float   fShade      = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    vector  vReflect    = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector  vLook       = normalize(In.vWorldPos - g_vCamPosition);
    float   fSpecular   = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);

    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + g_vLightAmbient * g_vMtrlAmbient)
				 + g_vLightSpecular * g_vMtrlSpecular * fSpecular;

    return Out;
}

PS_OUT PS_AlphaBrown(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
    {
        // 알파 낮으면 갈색으로 출력
        Out.vColor = float4(0.25f, 0.14f, 0.09f, 1.0f); // 갈색
        return Out;
    }

    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = normalize(In.vWorldPos - g_vCamPosition);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);
    
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

// 등장 연출
PS_OUT PS_Appear(PS_IN In)
{
    PS_OUT  Out     = PS_MAIN(In); // 공통 조명 계산 재사용
    float   fAlpha  = saturate(g_fAppearTime / g_fAppearDuration);

	Out.vColor.a *= fAlpha;
    return  Out;
}

// 사라짐 연출
PS_OUT PS_Disappear(PS_IN In)
{
    PS_OUT Out      = PS_MAIN(In); // 공통 조명 계산 재사용
    float fAlpha    = 1.f - saturate(g_fDeathTime / g_fDeathDuration);

	Out.vColor.a *= fAlpha;
    return Out;
}

// Technique 정의
technique11 DefaultTechnique
{
    pass Solid
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass WireFrame
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBrownPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_AlphaBrown();
    }

    pass Appear
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Appear();
    }

    pass Disappear
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Disappear();
    }
}