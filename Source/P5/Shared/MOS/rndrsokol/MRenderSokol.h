#ifndef _INC_MRenderSokol
#define _INC_MRenderSokol

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*\
	File:			Render context implementation for Sokol Gfx
\*____________________________________________________________________________________________*/

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| Includes
|__________________________________________________________________________________________________
\*************************************************************************************************/
#include "../../MSystem/Raster/MRender.h"
#include "../../MSystem/Raster/MRCCore.h"
#include <sokol_gfx.h>

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| Enums
|__________________________________________________________________________________________________
\*************************************************************************************************/
enum
{
	// Buffer access
	CRC_BUFFER_ACCESS_IMMUTABLE,
	CRC_BUFFER_ACCESS_DYNAMIC,

	// Buffer type
	CRC_BUFFER_TYPE_VERTEX,
	CRC_BUFFER_TYPE_ELEMENT,
	CRC_BUFFER_TYPE_UNIFORM,
};

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CRC_SokolBuffer
|__________________________________________________________________________________________________
\*************************************************************************************************/
class CRC_SokolBuffer
{
public:
	CRC_SokolBuffer();
	~CRC_SokolBuffer();

	void Create(void* _pData, uint _Size, uint _Type, uint _Access);
	void Destroy();
	void Clear();

	bool IsValid();

	void UpdateData(void* _pData, uint _Size);

public:
	sg_buffer m_VertexBuffer;
	uint m_Access;
	uint m_Type;
	int m_VBID;
	int m_nTri;
	int m_VtxFmt;
	int m_Stride;
	uint16 m_Min, m_Max;
	void* m_pVB;
	CRC_VertexBuffer m_VB;
	TArray<uint16> m_liPrim;

	fp32* m_pScaler;
	CRC_VRegTransform* m_pTransform;
	uint32 m_nTransform;

	// Weird but i don't want to refactor
	CRC_SokolBuffer* m_pIB;
};

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CRenderContextSokol
|__________________________________________________________________________________________________
\*************************************************************************************************/
class CRenderContextSokol : public CRC_Core
{
	MRTC_DECLARE;
public:

	CDisplayContext* m_pDisplayContext;

	// Internal rendering functions
	void Internal_RenderPolygon(int _nV, const CVec3Dfp32* _pV, const CVec3Dfp32* _pN, const CVec4Dfp32* _pCol = NULL, const CVec4Dfp32* _pSpec = NULL, /*const fp32* _pFog = NULL,*/
		const CVec4Dfp32* _pTV0 = NULL, const CVec4Dfp32* _pTV1 = NULL, const CVec4Dfp32* _pTV2 = NULL, const CVec4Dfp32* _pTV3 = NULL, int _Color = 0xffffffff);

	void Internal_IndxTriangles(uint16* _pIndices, int _nTriangles, const CRC_VertexBuffer& _VB, int _bAllUsed);

	void Internal_VA_SetArrays(int _VBID, int _VtxFmt, int _Stride, uint8* _pBase);
	void Internal_VA_SetArrays(const CRC_VertexBuffer& _VB);

	CRenderContextSokol();
	~CRenderContextSokol();

	void Create(CObj* _pContext, const char* _pParams);

	const char* GetRenderingStatus();
	virtual void Flip_SetInterval(int _nFrames);

	virtual void BeginScene(CRC_Viewport* _pVP);
	virtual void EndScene();


	void Attrib_Set(CRC_Attributes* _pAttrib) {}
	void Attrib_SetAbsolute(CRC_Attributes* _pAttrib) {}
	void Matrix_SetRender(int _iMode, const CMat4Dfp32* _pMatrix);

	// -------------------------------------------------------------------
	// Internal texture-management functions

	void GL_InitTextureBuildBuffers(int _MaxTextureSize, int _DebugResponsibleTextureID = 0);
	void GL_InitTextures();
	void GL_DeleteTextures();

	bool LoadTexture(int _GLImageTarget, int _TextureID, CTextureContainer* _pTC, int _iLocal, uint8 _TextureVersion, const CTC_TextureProperties& _Properties, int _iFirstMip, int _Width, int _Height, int _nMip);
	bool LoadTextureBuildInto(int _GLImageTarget, int _TextureID, CTextureContainer* _pTC, int _iLocal, const CImage& _TxtDesc, const CTC_TextureProperties& _Properties, int _iFirstMip, int _Width, int _Height, int _nMip);
	bool LoadTextureRender(int _GLImageTarget, int _TextureID, CTextureContainer* _pTC, int _iLocal, const CImage& _TxtDesc, const CTC_TextureProperties& _Properties, int _iFirstMip, int _Width, int _Height, int _nMip);

	void GL_BuildTexture2D(int _RCID, int _TextureID, int _GLObjectTarget, int _GLImageTarget, const CTC_TextureProperties& _Properties);
	void GL_SetTextureParameters(int _RCID, int _TextureID, int _GLObjectTarget, const CTC_TextureProperties& _Properties);
	void GL_SetTextureParameters(int _TextureID);
	void GL_UpdateAllTextureParameters();
	void GL_BuildTexture(int _RCID, int _TextureID, const CTC_TextureProperties& _Properties);
	void GL_UnloadTexture(int _RCID);
	void GL_SetTexture(int _TextureID, int _iTexChannel, bool _bPrecache = false);

	virtual void Texture_PrecacheFlush() {}
	virtual void Texture_PrecacheBegin(int _Count) {}
	virtual void Texture_PrecacheEnd() {}
	virtual void Texture_Precache(int _TextureID) {}

	virtual int Texture_GetBackBufferTextureID() { return 0; }
	virtual int Texture_GetFrontBufferTextureID() { return 0; }
	virtual int Texture_GetZBufferTextureID() { return 0; }

	// -------------------------------------------------------------------
	// Internal vertexbuffer-management functions
	void VB_DeleteAll();
	void VB_Delete(int _VBID);
	void VB_Create(int _VBID);

	//void VBO_CreateVB(int _VBID, CRC_BuildVertexBuffer& _VB);
	//void VBO_DestroyVB(int _VBID);

	//void VBO_Begin(int _VBID);
	//void VBO_Disable();

	//void VBO_RenderVB(int _VBID);
	//void VBO_RenderVB_IndexedTriangles(int _VBID, uint16* _piPrim, int _nTri);
	//void VBO_RenderVB_IndexedPrimitives(int _VBID, uint16* _piPrim, int _nPrim);


	virtual int Geometry_GetVBSize(int _VBID) { return 0; }

	virtual void RenderTarget_SetRenderTarget(const CRC_RenderTargetDesc& _RenderTarget);
	virtual void RenderTarget_Clear(CRct _ClearRect, int _WhatToClear, CPixel32 _Color, fp32 _ZBufferValue, int _StecilValue);

	void Render_IndexedTriangles(uint16* _pTriVertIndices, int _nTriangles);
	void Render_IndexedTriangleStrip(uint16* _pIndices, int _Len) {}
	void Render_IndexedWires(uint16* _pIndices, int _Len) {}
	void Render_IndexedPolygon(uint16* _pIndices, int _Len) {}
	void Render_IndexedPrimitives(uint16* _pPrimStream, int _StreamLen) {}
	void Render_VertexBuffer(int _VBID) {}
	void Render_Wire(const CVec3Dfp32& _v0, const CVec3Dfp32& _v1, CPixel32 _Color) {}
	void Render_WireStrip(const CVec3Dfp32* _pV, const uint16* _piV, int _nVertices, CPixel32 _Color) {}
	void Render_WireLoop(const CVec3Dfp32* _pV, const uint16* _piV, int _nVertices, CPixel32 _Color) {}

	virtual void Geometry_PrecacheFlush();
	virtual void Geometry_PrecacheBegin(int _Count);
	virtual void Geometry_PrecacheEnd();
	virtual void Geometry_Precache(int _VBID);
	virtual CDisplayContext* GetDC() { return m_pDisplayContext; }
	
	// Console commands:

	void Con_ChangePicMip(int _Level, int _iPicMip);
	void Con_r_picmip(int _iPicMip, int _Level);

	void Con_gl_reloadprograms();

	void Con_gl_anisotropy(fp32 _Value);
	void Con_gl_vsync(int _Value);

	void Con_gl_logprograms(int _Value);

	void Register(CScriptRegisterContext& _RegContext);


private:
	// -------------------------------------------------------------------
	// Render Context Info

	int m_bLog : 1;
	int m_bLogUsage : 1;
	int m_bLogVP : 1;
	
	// -------------------------------------------------------------------
	// Matrix and Attrib Stack

	CMat4Dfp32 m_Transform[CRC_MATRIXSTACKS];

	// -------------------------------------------------------------------
	// GL Info

	uint8 m_lPicMips[CRC_MAXPICMIPS];			// Default: 0,0,0,0

	uint32 m_bPendingTextureParamUpdate : 1;
	uint32 m_bPendingProgramReload : 1;
	uint32 m_bResourceLog : 1;
	uint32 m_bDirtyVPCache : 1;
	uint32 m_bDirtyFPCache : 1;

	// -------------------------------------------------------------------
	// VSync
	int			m_VSync;

	// -------------------------------------------------------------------
	// EXT_TEXTURE_FILTER_ANISOTROPIC
	fp32 m_Anisotropy;
	fp32 m_MaxAnisotropy;

	// -------------------------------------------------------------------
	// Sokol stuff

	TArray<CRC_SokolBuffer*> m_lpVB;
	//TArray<CRC_SokolBuffer*> m_lpIB;
};


#endif // _INC_MRenderSokol