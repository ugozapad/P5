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

// Geometry and Vertex Buffer:

void CRenderContextSokol::Geometry_PrecacheFlush()
{
	VB_DeleteAll();
}

void CRenderContextSokol::Geometry_PrecacheBegin(int _Count)
{
	ConOutL("(CRenderContextSokol::Geometry_PrecacheBegin) to precache %i", _Count);
}

void CRenderContextSokol::Geometry_PrecacheEnd()
{
}

void CRenderContextSokol::Geometry_Precache(int _VBID)
{
	if (m_pVBCtx->VB_GetFlags(_VBID) & CXR_VBFLAGS_ALLOCATED)
	{
		CRC_VBIDInfo& IDInfo = m_lVBIDInfo[_VBID];
		if (!(IDInfo.m_Fresh & 1))
			VB_Create(_VBID);
	}
}

// Vertex Buffer operations:

void GetMinMax(const uint16* _pIndices, int _nCount, uint16& _Min, uint16& _Max)
{
	// Starting on a primitive restart index is not valid (i.e. first index cannot be 0xffff)
	_Min = _pIndices[0];
	_Max = _pIndices[0];
	for (int i = 1; i < _nCount; i++)
	{
		uint16 Idx = _pIndices[i];
		if (Idx != 0xffff)
		{
			_Min = Min(_Min, Idx);
			_Max = Max(_Min, Idx);
		}
	}
}

static bool IsSinglePrimType(const uint16* _piIndices, int _nPrim)
{
	CRCPrimStreamIterator StreamIterate(_piIndices, _nPrim);
	if (StreamIterate.IsValid())
	{
		int CurrentType = StreamIterate.GetCurrentType();
		do
		{
			if (CurrentType != StreamIterate.GetCurrentType())
				return false;
		} while (StreamIterate.Next());
	}

	return true;
}

static void GetPrimStats(const uint16* _piIndices, int _nPrim, int& _PrimCount, int& _IndexCount)
{
	CRCPrimStreamIterator StreamIterate(_piIndices, _nPrim);
	if (StreamIterate.IsValid())
	{
		int CurrentType = StreamIterate.GetCurrentType();
		do
		{
			_PrimCount++;
			const uint16* pPrim = StreamIterate.GetCurrentPointer();
			_IndexCount += *pPrim;
		} while (StreamIterate.Next());
	}
}

static void AssemblePrimStream(CRC_SokolBuffer& _VBI, const uint16* _piIndices, int _nPrim)
{
	int nPrimCount = 0;
	int nIndexCount = 0;
	GetPrimStats(_piIndices, _nPrim, nPrimCount, nIndexCount);
	_VBI.m_liPrim.SetLen(nIndexCount + nPrimCount - 1);

	CRCPrimStreamIterator StreamIterate(_piIndices, _nPrim);
	if (StreamIterate.IsValid())
	{
		int CurrentType = StreamIterate.GetCurrentType();
		uint16* pStream = _VBI.m_liPrim.GetBasePtr();
		int iP = 0;
		int nTri = 0;
		do
		{
			if (iP > 0)
			{
				// Insert prim restart token
				pStream[iP++] = 0xffff;
			}
			const uint16* pPrim = StreamIterate.GetCurrentPointer();
			int nV = *pPrim;
			memcpy(pStream + iP, pPrim + 1, nV * 2);
			iP += nV;
			nTri += (nV - 2);
		} while (StreamIterate.Next());
		int nPrim = _VBI.m_liPrim.Len();
		GetMinMax(_VBI.m_liPrim.GetBasePtr(), _VBI.m_liPrim.Len(), _VBI.m_Min, _VBI.m_Max);
		_VBI.m_VB.m_PrimType = CurrentType;
		_VBI.m_VB.m_nPrim = nPrim;
		_VBI.m_nTri = nTri;
	}
}

void CRenderContextSokol::VB_DeleteAll()
{
	for (int i = 0; i < m_lpVB.Len(); i++)
	{
		if (m_lpVB[i] != NULL)
			VB_Delete(i);
	}

	m_lpVB.Destroy();
}

void CRenderContextSokol::VB_Delete(int _VBID)
{
	if (!m_lpVB.ValidPos(_VBID)) return;
	if (!m_lpVB[_VBID]) return;

	delete m_lpVB[_VBID];
	m_lpVB[_VBID] = NULL;

	CRC_VBIDInfo& IDInfo = m_lVBIDInfo[_VBID];
	IDInfo.m_Fresh &= ~1;
}

void CRenderContextSokol::VB_Create(int _VBID)
{
	m_lpVB.SetLen(m_lVBIDInfo.Len());

	// Make sure it isn't created already
	VB_Delete(_VBID);
	if (!m_lpVB[_VBID])
		m_lpVB[_VBID] = DNew(CRC_SokolBuffer) CRC_SokolBuffer();
	
	CRC_SokolBuffer* pVB = m_lpVB[_VBID];
	pVB->Clear();

	CRC_BuildVertexBuffer VBB;
	VBB.Clear();
	m_pVBCtx->VB_Get(_VBID, VBB, VB_GETFLAGS_BUILD);

	// Lets create a temporary buffer
	CRC_VertexBuffer _VB;
	TThinArray<uint8> Temp;
	Temp.SetLen(VBB.CRC_VertexBuffer_GetSize());
	VBB.CRC_VertexBuffer_ConvertTo(Temp.GetBasePtr(), _VB);

	CRC_SokolBuffer& VBI = *pVB;

	int iVtxCollectFunc;
	int VtxStride;	// This might be bogus cause it will always add 4 for color, regardless of presence of a color array in _VB
	{
		// GL does not support 1D texcoords, patch patch
		Geometry_GetVertexFormat(_VB, VBI.m_VtxFmt, VtxStride, iVtxCollectFunc);
	}

	int nV = _VB.m_nV;

	// Calculate vertex size (stride)
	int Stride = sizeof(CVec3Dfp32);

	{
		for (int i = 0; i < CRC_MAXTEXCOORDS; i++)
			if (_VB.m_pTV[i]) Stride += sizeof(fp32) * _VB.m_nTVComp[i];
	}

	if (_VB.m_pN) Stride += sizeof(CVec3Dfp32);
	if (_VB.m_pCol) Stride += sizeof(CPixel32);
	if (_VB.m_pSpec) Stride += sizeof(CPixel32);
	if (_VB.m_pMI) Stride += sizeof(uint32); // MIFIXME
	if (_VB.m_pMW) Stride += sizeof(fp32) * _VB.m_nMWComp;

	VBI.m_Stride = Stride;
	int Size = Stride * nV;

	void* pBuffer = M_ALLOC(Size);
	if (!pBuffer)
	{
		VB_Delete(_VBID);
		MemError("VB_CreateVB");
		return;
	}

	{
		uint8* pWrite = (uint8*)pBuffer;
		CVec3Dfp32* pV = _VB.m_pV;
		CVec3Dfp32* pN = _VB.m_pN;
		fp32* lpTV[CRC_MAXTEXCOORDS];
		memcpy(&lpTV, &_VB.m_pTV, sizeof(lpTV));

		CPixel32* pCol = _VB.m_pCol;
		CPixel32* pSpec = _VB.m_pSpec;
		uint32* pMI = _VB.m_pMI;
		fp32* pMW = _VB.m_pMW;

		for (int iV = 0; iV < nV; iV++)
		{
			*((CVec3Dfp32*)pWrite) = *pV++; pWrite += sizeof(CVec3Dfp32);
			if (pN) { *((CVec3Dfp32*)pWrite) = *pN++; pWrite += sizeof(CVec3Dfp32); }
			if (pCol) { *((CPixel32*)pWrite) = *pCol++; pWrite += sizeof(CPixel32); }
			if (pSpec) { *((CPixel32*)pWrite) = *pSpec++; pWrite += sizeof(CPixel32); }
			for (int iTV = 0; iTV < CRC_MAXTEXCOORDS; iTV++)
			{
				if (lpTV[iTV])
				{
					for (int i = 0; i < _VB.m_nTVComp[iTV]; i++) { *((fp32*)pWrite) = *((lpTV[iTV])++); pWrite += sizeof(fp32); }
				}
			}
			if (pMI) { *((uint32*)pWrite) = *pMI++; ::SwapLE((uint32&)*pWrite); pWrite += sizeof(uint32); }
			if (pMW) { for (int i = 0; i < _VB.m_nMWComp; i++) { *((fp32*)pWrite) = *pMW++; pWrite += sizeof(fp32); } }
		}

		M_ASSERT((pWrite - (uint8*)pBuffer) == nV * Stride, "!");
	}

	pVB->Create(pBuffer, Size, CRC_BUFFER_TYPE_VERTEX, CRC_BUFFER_ACCESS_IMMUTABLE);

	VBI.m_pVB = (void*)-1;	// Indicate it's not empty

	// Copy primitives?
	if (_VB.m_piPrim)
	{
		if (_VB.m_PrimType == CRC_RIP_STREAM)
		{
			if (IsSinglePrimType(_VB.m_piPrim, _VB.m_nPrim))
			{
				AssemblePrimStream(VBI, _VB.m_piPrim, _VB.m_nPrim);
			}
			else
			{
				// Convert to triangles
				VBI.m_liPrim.Clear();

				uint16 lTriIndices[1024 * 3];

				CRCPrimStreamIterator StreamIterate(_VB.m_piPrim, _VB.m_nPrim);

				while (StreamIterate.IsValid())
				{
					int nTriIndices = 1024 * 3;
					bool bDone = Geometry_BuildTriangleListFromPrimitives(StreamIterate, lTriIndices, nTriIndices);
					if (nTriIndices)
					{
						int iDst = VBI.m_liPrim.Len();
						VBI.m_liPrim.SetLen(iDst + nTriIndices);
						memcpy(&VBI.m_liPrim[iDst], lTriIndices, nTriIndices * 2);
						//					Internal_VAIndxTriangles(lTriIndices, nTriIndices/3, 0);
					}

					if (bDone) break;
				}

				VBI.m_VB.m_nPrim = VBI.m_liPrim.Len() / 3;
				VBI.m_VB.m_PrimType = CRC_RIP_TRIANGLES;
				VBI.m_nTri = VBI.m_VB.m_nPrim;
			}
		}
		else
		{
			VBI.m_liPrim.SetLen((_VB.m_PrimType == CRC_RIP_TRIANGLES) ? _VB.m_nPrim * 3 : _VB.m_nPrim);
			VBI.m_VB.m_nPrim = _VB.m_nPrim;
			VBI.m_VB.m_PrimType = _VB.m_PrimType;
			VBI.m_nTri = _VB.m_nPrim;
			memcpy(VBI.m_liPrim.GetBasePtr(), _VB.m_piPrim, VBI.m_liPrim.Len() * 2);
		}

		// Storage for Primitives
		pVB->m_pIB = DNew(CRC_SokolBuffer) CRC_SokolBuffer();
		pVB->m_pIB->Create(VBI.m_liPrim.GetBasePtr(), VBI.m_liPrim.ListSize(), CRC_BUFFER_TYPE_ELEMENT, CRC_BUFFER_ACCESS_IMMUTABLE);

		GetMinMax(VBI.m_liPrim.GetBasePtr(), VBI.m_liPrim.Len(), VBI.m_Min, VBI.m_Max);
		VBI.m_liPrim.Destroy();
	}

	m_pVBCtx->VB_Release(_VBID);

	CRC_VBIDInfo& IDInfo = m_lVBIDInfo[_VBID];
	IDInfo.m_Fresh |= 1;
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
| CRC_SokolBuffer
|__________________________________________________________________________________________________
\*************************************************************************************************/
CRC_SokolBuffer::CRC_SokolBuffer()
{
	Clear();
}

CRC_SokolBuffer::~CRC_SokolBuffer()
{
	Destroy();
}

void CRC_SokolBuffer::Create(void* _pData, uint _Size, uint _Type, uint _Access)
{
	m_Access = _Access;
	m_Type = _Type;

	// fill buffer data structure
	sg_range BufferRange;
	memset(&BufferRange, 0, sizeof(BufferRange));
	BufferRange.ptr = _pData;
	BufferRange.size = _Size;

	// describe buffer
	sg_buffer_desc BufferDesc;
	memset(&BufferDesc, 0, sizeof(BufferDesc));

	if (m_Access == CRC_BUFFER_TYPE_VERTEX)
		BufferDesc.type = SG_BUFFERTYPE_VERTEXBUFFER;
	else if (m_Access == CRC_BUFFER_TYPE_ELEMENT)
		BufferDesc.type = SG_BUFFERTYPE_INDEXBUFFER;
	else if (m_Access == CRC_BUFFER_TYPE_UNIFORM)
		BufferDesc.type = SG_BUFFERTYPE_STORAGEBUFFER;

	if (m_Access == CRC_BUFFER_ACCESS_IMMUTABLE)
		BufferDesc.usage = SG_USAGE_IMMUTABLE;
	else if (m_Access == CRC_BUFFER_ACCESS_DYNAMIC)
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
		Error_static("CRC_SokolBuffer::Create", "ERROR: No more free space in buffer pool");
	}
	else if (BufferState == SG_RESOURCESTATE_FAILED)
	{
		// ERROR: failed to create buffer (reason is why should be prints in log)
		Error_static("CRC_SokolBuffer::Create", "ERROR: failed to create buffer. (see log)");
	}
}

void CRC_SokolBuffer::Destroy()
{
	if (m_pIB)
	{
		delete m_pIB;
		m_pIB = NULL;
	}

	if (IsValid())
	{
		sg_destroy_buffer(m_VertexBuffer);
		memset(&m_VertexBuffer, 0, sizeof(m_VertexBuffer));

		m_Access = 0;
	}

	Clear();
}

void CRC_SokolBuffer::Clear()
{
	memset(&m_VertexBuffer, 0, sizeof(m_VertexBuffer));
	m_Access = 0;
	m_Type = 0;
	m_VBID = -1;
	m_nTri = 0;
	m_Stride = 0;
	m_pVB = NULL;
	m_VB.Clear();
	m_liPrim.Clear();
	m_Min = 0xfffe;
	m_Max = 0;
	m_pScaler = NULL;
	m_pTransform = NULL;
	m_nTransform = 0;

	m_pIB = NULL;
}

bool CRC_SokolBuffer::IsValid()
{
	sg_resource_state BufferState = sg_query_buffer_state(m_VertexBuffer);
	return (BufferState != SG_RESOURCESTATE_INVALID) || (BufferState != SG_RESOURCESTATE_FAILED);
}

void CRC_SokolBuffer::UpdateData(void* _pData, uint _Size)
{
	if (m_Access == CRC_BUFFER_ACCESS_IMMUTABLE)
		Error_static("CRC_SokolBuffer::UpdateData", "Failed to update immutable buffer.");

	sg_range Range = {};
	Range.ptr = _pData;
	Range.size = _Size;
	sg_update_buffer(m_VertexBuffer, Range);
}
