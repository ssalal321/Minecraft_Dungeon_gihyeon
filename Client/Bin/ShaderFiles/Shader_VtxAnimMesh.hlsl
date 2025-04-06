#include "Engine_Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_BoneMatrices[512]; /* 메시에게 영향을 주는 뼈 배열 */

vector      g_vLightDir = vector(1.f, -1.f, 0.f, 0.f);
vector      g_vLightPos;
float       g_fLightRange;
vector      g_vLightDiffuse;
vector      g_vLightAmbient;
vector      g_vLightSpecular;



texture2D   g_DiffuseTexture;
vector      g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 0.3f);
vector      g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector      g_vCamPosition;

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
    
    uint4  vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
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

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x + 
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y + 
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z + 
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);    
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    
    
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
    
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = normalize(In.vWorldPos - g_vCamPosition);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);
    
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }    
}

