float       g_fCutoffY = 0.f; // 닳은 정도를 나타내는 값 (0.0 ~ 1.0)
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
    if (vTextureInfo.a < 0.55f)
        discard;

    if (g_fCutoffY == 0.f)
    {
	    // 원래 텍스처 색상 유지
        Out.vColor = vTextureInfo;

        return Out;
    }
    
    // 깎인 부분 처리
    if (In.vTexcoord.y < g_fCutoffY)
    {
        discard; // 나머지는 버림
    }

    if (g_fCutoffY <= In.vTexcoord.y && In.vTexcoord.y < g_fCutoffY + (2.0f / 65.0f))  // 윗 테두리 표시
    {
    	// HP가 줄어들수록 색이 어두워지는 효과 적용
        float fLerpPoint = g_fCutoffY; // HP가 줄어들수록 0 → 1 값으로 증가

        // 원래 색상
        float4 baseColor = float4(203.0f / 255.0f, 36.0f / 255.0f, 57.0f / 255.0f, 1.0f);

    	// 어두운 색상
        float4 darkColor = float4(118.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f);

        float4 finalColor = lerp(baseColor, darkColor, fLerpPoint);

    	if (g_fCutoffY == 1.0f)
        {
            discard;
        }

    	Out.vColor = finalColor;
    }
    else if (g_fCutoffY + (2.0f / 65.0f) <= In.vTexcoord.y && In.vTexcoord.y < g_fCutoffY + (4.0f / 65.0f))  // 윗 테두리 표시
    {
        // 이미지 가운데 기준으로 좌우로 갈수록 흰색이 연해지도록
        float distanceFromCenter = abs(In.vTexcoord.x - 0.5f); // 이미지 중앙에서의 거리 (0.0f ~ 0.5f)

        float yScaleFactor;
        // 아래로 갈수록 그라데이션 범위가 좁아지도록 y 값에 따라 스케일링
        if (g_fCutoffY < 0.75f)
	        yScaleFactor = 1.f - (In.vTexcoord.y - g_fCutoffY * 2.f); // y 값에 비례해서 범위 좁아짐
        else
            yScaleFactor = 1.f - (In.vTexcoord.y - g_fCutoffY * 6.f); // y 값에 비례해서 범위 좁아짐

        // 그라데이션 범위와 결합
        float alphaBlendFactor = 1.f - (distanceFromCenter * 2.f * yScaleFactor);

        // alphaBlendFactor가 0보다 작은 값이 나올 경우 보정 (0 ~ 1 사이로 제한)
        alphaBlendFactor = max(0.f, min(1.f, alphaBlendFactor)); // 0과 1 사이로 값을 제한

        // 흰색
        float4 whiteColor = float4(1.f, 1.f, 1.f, 1.f);

        // 원래 색상
        float4 baseColor = vTextureInfo;

        // 흰색 알파값과 원래 색상 알파값을 블렌딩
        Out.vColor = lerp(baseColor, whiteColor, alphaBlendFactor);

        if (g_fCutoffY >= 0.9f)
        {
	    // 원래 텍스처 색상 유지
            Out.vColor = vTextureInfo;

            return Out;
        }

    }
    else
    	// 원래 텍스처 색상 유지
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

