#include "PCH.h"
#include "MRenderSokol.h"
#include "MDisplaySokol.h"

#include "../XR/XRVBContext.h"

// Sokol logger
void CRenderContextSokol_Logger(
	const char* tag,                // always "sg"
	uint32_t log_level,             // 0=panic, 1=error, 2=warning, 3=info
	uint32_t log_item_id,           // SG_LOGITEM_*
	const char* message_or_null,    // a message string, may be nullptr in release mode
	uint32_t line_nr,               // line number in sokol_gfx.h
	const char* filename_or_null,   // source filename, may be nullptr in release mode
	void* user_data)
{
	M_TRACEALWAYS( "%s: %s\n", tag, message_or_null ? message_or_null : "no error" );
}

MRTC_IMPLEMENT_DYNAMIC(CRenderContextSokol, CRC_Core);

CRenderContextSokol::CRenderContextSokol()
{
	m_pDisplayContext = NULL;

	m_VSync = 0;

	m_bLog = 0;
	m_bLogUsage = 0;
	m_bLogVP = 0;

	m_bPendingTextureParamUpdate = 0;
	m_bPendingProgramReload = 0;
	m_bResourceLog = 0;
	m_bDirtyVPCache = 0;
	m_bDirtyFPCache = 0;

	m_Anisotropy = 0.0f;
	m_MaxAnisotropy = 0.0f;
}

CRenderContextSokol::~CRenderContextSokol()
{
	if (m_iVBCtxRC >= 0) { m_pVBCtx->RemoveRenderContext(m_iVBCtxRC); m_iVBCtxRC = -1; };
	if (m_iTC >= 0) { m_pTC->RemoveRenderContext(m_iTC); m_iTC = -1; };
}

void CRenderContextSokol::Create(CObj* _pContext, const char* _pParams)
{
	CRC_Core::Create(_pContext, _pParams);

	// Link current display context to our created render context.
	m_pDisplayContext = TDynamicCast<CDisplayContextSokol>(_pContext);
	if (!m_pDisplayContext)
		Error("CRenderContextSokol::Create", "Wrong display-context.");

	MACRO_GetSystem;

	// Initialize Sokol Gfx
	sg_desc sgdesc;
	memset(&sgdesc, 0, sizeof(sgdesc));
	sgdesc.logger.func = CRenderContextSokol_Logger;
	sg_setup(&sgdesc);

	m_Caps_TextureFormats = -1;
	m_Caps_DisplayFormats = -1;
	m_Caps_ZFormats = -1;
	m_Caps_StencilDepth = 8;
	m_Caps_AlphaDepth = 8;
	m_Caps_Flags = -1;

	m_Caps_nMultiTexture = Min(16, (int)CRC_MAXTEXTURES);
	m_Caps_nMultiTextureCoords = Min(8, (int)CRC_MAXTEXCOORDS);
	m_Caps_nMultiTextureEnv = Min(8, (int)CRC_MAXTEXCOORDS);

	m_iTC = m_pTC->AddRenderContext(this);
	m_iVBCtxRC = m_pVBCtx->AddRenderContext(this);
}

const char* CRenderContextSokol::GetRenderingStatus()
{
	return "Sokol rendering...";
}

void CRenderContextSokol::Flip_SetInterval(int _nFrames)
{

}

void CRenderContextSokol::BeginScene(CRC_Viewport* _pVP)
{
	CRC_Core::BeginScene(_pVP);
}

void CRenderContextSokol::EndScene()
{
	CRC_Core::EndScene();

	sg_commit();
}

void CRenderContextSokol::Matrix_SetRender(int _iMode, const CMat4Dfp32* _pMatrix)
{
	// Don't call super, it's pure virtual.
	if (_pMatrix)
		m_Transform[_iMode] = *_pMatrix;
	else
		m_Transform[_iMode].Unit();
}

void CRenderContextSokol::RenderTarget_SetRenderTarget(const CRC_RenderTargetDesc& _RenderTarget)
{
}

void CRenderContextSokol::RenderTarget_Clear(CRct _ClearRect, int _WhatToClear, CPixel32 _Color, fp32 _ZBufferValue, int _StecilValue)
{
	sg_pass_action PassAction = {};
	PassAction.colors[0].load_action = SG_LOADACTION_CLEAR;
	PassAction.colors[0].clear_value = { 0.5f, 0.5f, 0.5f, 1.0f };

	sg_pass Pass = {};
	Pass.action = PassAction;

	// Fill swap chain
	Pass.swapchain.width = _ClearRect.GetWidth();
	Pass.swapchain.height = _ClearRect.GetHeight();
	Pass.swapchain.sample_count = 1;
	Pass.swapchain.color_format = SG_PIXELFORMAT_RGBA32UI;

	sg_begin_pass(&Pass);
	sg_end_pass();
}

void CRenderContextSokol::Render_IndexedTriangles(uint16* _pTriVertIndices, int _nTriangles)
{
	MSCOPE(CRenderContextSokol::Render_IndexedTriangles, RENDER_SOKOL);

	if (_nTriangles == 0xffff)
	{
		// Recursively call this function
		mint* pList = ((mint*)_pTriVertIndices);
		int nLists = *pList;
		++pList;
		for (int i = 0; i < nLists; ++i)
		{
			int nTri = *pList;
			++pList;
			Render_IndexedTriangles(*((uint16**)pList), nTri);
			++pList;
		}
		return;
	}


	if (m_GeomVBID)
	{
	//	Internal_VBIndxTriangles(m_GeomVBID, _pIndices, _nTriangles);
		m_nTriangles += _nTriangles;
		//m_nTriTotal += _nTriangles;
		return;
	}

	if (!m_Geom.m_pV) return;
	if (m_AttribChanged) Attrib_Update();
	if (m_MatrixChanged) Matrix_Update();
	//if (m_bUpdateVPConst) VP_Update();

	Internal_IndxTriangles(_pTriVertIndices, _nTriangles, m_Geom, m_bGeomArrayAllUsed);

	m_nTriangles += _nTriangles;
	//m_nTriTotal += _nTriangles;
}

// Console commands:

void CRenderContextSokol::Con_ChangePicMip(int _Level, int _iPicMip)
{
	if (m_lPicMips[_iPicMip] == _Level)
	{
		//		LogExtensionState();
		return;
	}

	m_lPicMips[_iPicMip] = _Level;
	{
		MACRO_GetRegisterObject(CSystem, pSys, "SYSTEM");
		if (pSys && pSys->GetEnvironment())
		{
			pSys->GetEnvironment()->SetValuei(CFStrF("R_PICMIP%d", _iPicMip), m_lPicMips[_iPicMip]);
		}

		Texture_MakeAllDirty(_iPicMip);

		// Notify all subsystems
		if (pSys) pSys->System_BroadcastMessage(CSS_Msg(CSS_MSG_PRECACHEHINT_TEXTURES));
	}

}

void CRenderContextSokol::Con_r_picmip(int _iPicMip, int _Level)
{
	Con_ChangePicMip(_Level, _iPicMip);
}

void CRenderContextSokol::Con_gl_reloadprograms()
{
	m_bPendingProgramReload = 1;
}

void CRenderContextSokol::Con_gl_anisotropy(fp32 _Value)
{
	if (m_Anisotropy == _Value)
		return;

	m_Anisotropy = _Value;
	m_bPendingTextureParamUpdate = 1;

	MACRO_GetRegisterObject(CSystem, pSys, "SYSTEM");
	if (pSys) pSys->GetEnvironment()->SetValuef("R_ANISOTROPY", m_Anisotropy);
}

void CRenderContextSokol::Con_gl_vsync(int _Value)
{
	if (m_VSync == _Value)
		return;

	m_VSync = _Value;
	// Notify all subsystems
	MACRO_GetRegisterObject(CSystem, pSys, "SYSTEM");
	if (pSys) pSys->GetEnvironment()->SetValuei("VID_VSYNC", m_VSync);
}

void CRenderContextSokol::Con_gl_logprograms(int _Value)
{
	m_bLogVP = _Value != 0;
}

void CRenderContextSokol::Register(CScriptRegisterContext& _RegContext)
{
	CRC_Core::Register(_RegContext);

	_RegContext.RegFunction("r_picmip", this, &CRenderContextSokol::Con_r_picmip);
	//_RegContext.RegFunction("gl_reloadprograms", this, &CRenderContextSokol::Con_gl_reloadprograms);
	_RegContext.RegFunction("r_anisotropy", this, &CRenderContextSokol::Con_gl_anisotropy);
	_RegContext.RegFunction("r_vsync", this, &CRenderContextSokol::Con_gl_vsync);
	//_RegContext.RegFunction("gl_logprograms", this, &CRenderContextSokol::Con_gl_logprograms);

}

void CRenderContextSokol::Internal_RenderPolygon(int _nV, const CVec3Dfp32* _pV, const CVec3Dfp32* _pN, const CVec4Dfp32* _pCol, const CVec4Dfp32* _pSpec, const CVec4Dfp32* _pTV0, const CVec4Dfp32* _pTV1, const CVec4Dfp32* _pTV2, const CVec4Dfp32* _pTV3, int _Color)
{
}

void CRenderContextSokol::Internal_IndxTriangles(uint16* _pIndices, int _nTriangles, const CRC_VertexBuffer& _VB, int _bAllUsed)
{
	Internal_VA_SetArrays(_VB);

	if (_bAllUsed)
	{
		

		//glnDrawElements(GL_TRIANGLES, _nTriangles * 3, GL_UNSIGNED_SHORT, _pIndices);
		//GLErr("Internal_IndxTriangles (glnDrawElements)");
	}
	else
	{
		//uint16 MinVal, MaxVal;
		//GetMinMax(_pIndices, _nTriangles * 3, MinVal, MaxVal);
		//glDrawRangeElements(GL_TRIANGLES, MinVal, MaxVal, _nTriangles * 3, GL_UNSIGNED_SHORT, _pIndices);
		//GLErr("Internal_IndxTriangles (glDrawRangeElements)");
	}
}

void CRenderContextSokol::Internal_VA_SetArrays(int _VBID, int _VtxFmt, int _Stride, uint8* _pBase)
{
}

void CRenderContextSokol::Internal_VA_SetArrays(const CRC_VertexBuffer& _VB)
{
}

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CRC_VertexBufferSokol
|__________________________________________________________________________________________________
\*************************************************************************************************/
CVertexBufferSokol::CVertexBufferSokol()
{
	memset(&m_VertexBuffer, 0, sizeof(m_VertexBuffer));
	m_Access = 0;
}

CVertexBufferSokol::~CVertexBufferSokol()
{
	Destroy();
}

void CVertexBufferSokol::Create(void* _pData, uint _Size, uint _Access)
{
	m_Access = _Access;

	// fill buffer data structure
	sg_range BufferRange;
	memset(&BufferRange, 0, sizeof(BufferRange));
	BufferRange.ptr = _pData;
	BufferRange.size = _Size;

	// describe buffer
	sg_buffer_desc BufferDesc;
	memset(&BufferDesc, 0, sizeof(BufferDesc));
	BufferDesc.type = SG_BUFFERTYPE_INDEXBUFFER;

	if (m_Access == CRC_SOKOL_BUFFER_ACCESS_IMMUTABLE)
		BufferDesc.usage = SG_USAGE_IMMUTABLE;
	else if (m_Access == CRC_SOKOL_BUFFER_ACCESS_DYNAMIC)
		BufferDesc.usage = SG_USAGE_DYNAMIC;

	// #TODO: if we want to create immutable buffer we should pass sg_range, 
	// otherwise set BufferDesc.size from our buffer desc!
	if (_pData)
		BufferDesc.data = BufferRange;
	else 
		BufferDesc.size = _Size;

	// create buffer
	m_VertexBuffer = sg_make_buffer(BufferDesc);
	sg_resource_state BufferState = sg_query_buffer_state(m_VertexBuffer);

	if (BufferState == SG_RESOURCESTATE_INVALID)
	{
		// ERROR: No more free space in buffer pool
		Error_static("CVertexBufferSokol::Create", "ERROR: No more free space in buffer pool");
	}
	else if (BufferState == SG_RESOURCESTATE_FAILED)
	{
		// ERROR: failed to create buffer (reason is why should be prints in log)
		Error_static("CVertexBufferSokol::Create", "ERROR: failed to create buffer. (see log)");
	}
}

void CVertexBufferSokol::Destroy()
{
	if (IsValid())
	{
		sg_destroy_buffer(m_VertexBuffer);
		memset(&m_VertexBuffer, 0, sizeof(m_VertexBuffer));

		m_Access = 0;
	}
}

bool CVertexBufferSokol::IsValid()
{
	sg_resource_state BufferState = sg_query_buffer_state(m_VertexBuffer);
	return (BufferState != SG_RESOURCESTATE_INVALID) || (BufferState != SG_RESOURCESTATE_FAILED);
}

void CVertexBufferSokol::UpdateData(void* _pData, uint _Size)
{
	if (m_Access == CRC_SOKOL_BUFFER_ACCESS_IMMUTABLE)
		Error_static("CVertexBufferSokol::UpdateData", "Failed to update immutable buffer.");

	sg_range Range = {};
	Range.ptr = _pData;
	Range.size = _Size;
	sg_update_buffer(m_VertexBuffer, Range);
}
