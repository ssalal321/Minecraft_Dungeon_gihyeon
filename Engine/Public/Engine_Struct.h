#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iViewportWidth;
		unsigned int	iViewportHeight;
		unsigned int	iNumLevels;
	}ENGINE_DESC;	

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eType;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;

	typedef struct tagKeyFrame
	{
		float		fTrackPosition;

		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;
	}KEYFRAME;

	typedef struct ENGINE_DLL tagVertexCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertexPositionTextureCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSTEX;	

	typedef struct ENGINE_DLL tagVertexPositionNormalTextureCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int				iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;


	typedef struct ENGINE_DLL tagVertexAnimationMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndex; /* 영향을 받아야할 뼈의 인덱스 최대 4개 */
		XMFLOAT4		vBlendWeight; /* 각 뼈의 가중치 네개의 합 1 */

		static const unsigned int				iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;

}


#endif // Engine_Struct_h__
