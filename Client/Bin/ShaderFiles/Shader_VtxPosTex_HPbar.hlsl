float       g_fCutoffY = 0.f; // 닳은 정도를 나타내는 값 (0.0 ~ 1.0)
float       g_fYGradationFactor;
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_HPTexture;

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

// 리턴값 VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
VS_OUT VS_MAIN(VS_IN In)
{
    /* 기타 변환들을 수행한다.*/   
    VS_OUT Out = (VS_OUT)0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
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

// Pixel Shader
PS_OUT PS_PlayerHP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 텍스처 샘플링
    vector vTextureInfo = g_HPTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    // 불필요한 픽셀 제거
    if (vTextureInfo.a < 0.55f || In.vTexcoord.y < g_fCutoffY)
        discard;

    if (g_fCutoffY == 0.f)
    {
        Out.vColor = vTextureInfo;
        return Out;
    }
       
    // HP 감소에 따른 테두리 어둡게
    if (In.vTexcoord.y < g_fCutoffY + (2.0f / 65.0f))
    {
        float4 baseColor = float4(203.0f / 255.0f, 36.0f / 255.0f, 57.0f / 255.0f, 1.0f);
        float4 darkColor = float4(118.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f);
        Out.vColor = lerp(baseColor, darkColor, g_fCutoffY);
        return Out;
    }

    // 흰색 그라데이션 효과
    if (In.vTexcoord.y < g_fCutoffY + (4.0f / 65.0f))
    {
        float fDistanceFromCenter = 1.f - abs(In.vTexcoord.x - 0.5f) * 2.f;
    	// y 값이 0.75 이상일 때 급격하게 좁아지는 그라데이션 범위
        float fYScaleFactor = saturate(1.f - (In.vTexcoord.y - g_fCutoffY) * g_fYGradationFactor); // 0.75 이상에서는 범위 좁아짐
        float alphaBlendFactor = saturate(fDistanceFromCenter * fYScaleFactor);

        Out.vColor = lerp(vTextureInfo, float4(1.f, 1.f, 1.f, 1.f), alphaBlendFactor);
        return Out;
    }

    // 원래 텍스처 유지
    Out.vColor = vTextureInfo;
    return Out;
}






technique11 DefaultTechnique
{
    pass PlayerHP
    {
        VertexShader    = compile vs_5_0 VS_MAIN();
        PixelShader     = compile ps_5_0 PS_PlayerHP();
    }
}

