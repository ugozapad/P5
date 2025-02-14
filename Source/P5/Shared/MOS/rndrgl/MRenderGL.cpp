#include "PCH.h"
#include "MRenderGL.h"

MRTC_IMPLEMENT_DYNAMIC(CRenderContextGL, CRC_Core);

CRenderContextGL::CRenderContextGL()
{
}

CRenderContextGL::~CRenderContextGL()
{
}

CDisplayContext* CRenderContextGL::GetDC()
{
	return nullptr;
}

void CRenderContextGL::Flip_SetInterval(int _nFrames)
{
}

void CRenderContextGL::Texture_PrecacheFlush()
{
}

void CRenderContextGL::Texture_PrecacheBegin(int _Count)
{
}

void CRenderContextGL::Texture_PrecacheEnd()
{
}

int CRenderContextGL::Texture_GetBackBufferTextureID()
{
	return 0;
}

int CRenderContextGL::Texture_GetZBufferTextureID()
{
	return 0;
}

int CRenderContextGL::Texture_GetFrontBufferTextureID()
{
	return 0;
}

int CRenderContextGL::Geometry_GetVBSize(int _VBID)
{
	return 0;
}

void CRenderContextGL::Internal_RenderPolygon(int _nV, const CVec3Dfp32* _pV, const CVec3Dfp32* _pN, const CVec4Dfp32* _pCol, const CVec4Dfp32* _pSpec, const CVec4Dfp32* _pTV0, const CVec4Dfp32* _pTV1, const CVec4Dfp32* _pTV2, const CVec4Dfp32* _pTV3, int _Color)
{
}

void CRenderContextGL::Attrib_Set(CRC_Attributes* _pAttrib)
{
}
