
//D3D11_RASTERIZER_DESC

// 기본 래스터라이저 상태
RasterizerState RS_Default
{
    FillMode = Solid; // 면을 채우는 모드 (솔리드)
    CullMode = Back;  // 뒤쪽 면을 컬링 (숨김)
    FrontCounterClockwise = false; // 정점 순서가 시계방향(Clockwise)이 앞면으로 처리됨
};


RasterizerState RS_Wireframe
{
    FillMode = Wireframe;           // 선으로만 렌더링 (와이어프레임)
    CullMode = Back;                // 뒷면 컬링 유지
    FrontCounterClockwise = false; // 시계방향을 앞면으로 처리
};


RasterizerState RS_Cull_None
{
    FillMode = Solid;   // 면을 채운 렌더링 유지
    CullMode = None;    // 컬링 없음 (뒷면도 렌더링)
    FrontCounterClockwise = false;
};


DepthStencilState DSS_Default
{
    DepthEnable     = true;  // 깊이 테스트 활성화
    DepthWriteMask  = all;   // 깊이 버퍼에 쓰기 활성화
    DepthFunc       = less_equal;   // 가까운 픽셀만 렌더링 (less_equal → 작은 값이 우선)
};


DepthStencilState DSS_Draw_Priority
{
    DepthEnable     = false; // 깊이 테스트 비활성화
    DepthWriteMask  = zero;  // 깊이 버퍼에 쓰기 금지
};


BlendState BS_Default
{
    BlendEnable[0] = false; // 블렌딩 비활성화 (그냥 덮어씀)
};


BlendState BS_AlphaBlend
{
    BlendEnable[0] = true; // 블렌딩 활성화

    SrcBlend   = Src_Alpha;     // 소스(새로 그리는 픽셀)의 알파 값 사용
    DestBlend  = Inv_Src_Alpha; // 기존 색상의 (1 - 소스 알파) 값 사용
    BlendOp    = Add;           // 두 색상을 더함
};

