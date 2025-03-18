
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_Texture;

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


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vTextureInfo = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    if (vTextureInfo.a < 0.55f)
        discard;

    Out.vColor = vTextureInfo;
    
    return Out;    
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }


}

