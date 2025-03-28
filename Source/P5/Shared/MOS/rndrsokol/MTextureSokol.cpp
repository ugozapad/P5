#include "PCH.h"
#include "MDisplaySokol.h"
#include "MRenderSokol.h"

class CTextureSokol
{
public:
	uint16 m_Width;
	uint16 m_Height;

	void Build(uint16 _Width, uint16 _Height)
	{
		m_Width = _Width;
		m_Height = _Height;
	}

	bool NeedBuild(uint16 _Width, uint16 _Height) const
	{
		return ((m_Width != _Width) || (m_Height != _Height));
	}

	void Clear()
	{
		m_Width = 0;
		m_Height = 0;
	}
};

TThinArray<CTextureSokol> lTextures;

void CRenderContextSokol::GL_InitTextureBuildBuffers(int _MaxTextureSize, int _DebugResponsibleTextureID)
{
}

void CRenderContextSokol::GL_InitTextures()
{
	int nTxt = m_pTC->GetIDCapacity();
	lTextures.SetLen(nTxt);
	TAP_RCD<CTextureSokol> plTxt(lTextures);
	for (int i = 0; i < nTxt; i++)
		plTxt[i].Clear();
}

void CRenderContextSokol::GL_DeleteTextures()
{
	int nTxt = m_pTC->GetIDCapacity();
	for (int i = 0; i < nTxt; i++)
		GL_UnloadTexture(i);
}

// -------------------------------------------------------------------
static int GLGetMipMapLevels(int _w, int _h)
{
	//	if (!IsPow2(_w)) Error_static("CTextureContext::GetMipMapLevels", "Not 2^x width.");
	//	if (!IsPow2(_h)) Error_static("CTextureContext::GetMipMapLevels", "Not 2^x height.");
	int l2w = Log2(_w);
	int l2h = Log2(_h);
	return Max(l2w, l2h) + 1;
};

// -------------------------------------------------------------------
static bool IsPow2(const CImage& _Img)
{
	int w = Log2(_Img.GetWidth());
	int h = Log2(_Img.GetHeight());

	return ((_Img.GetWidth() == (1 << w)) && (_Img.GetHeight() == (1 << h)));
}

static int NextPow2(int _Value)
{
	int i = 1;
	while (i < _Value)
		i <<= 1;

	return i;
}

bool CRenderContextSokol::LoadTexture(int _GLImageTarget, int _TextureID, CTextureContainer* _pTC, int _iLocal, uint8 _TextureVersion, const CTC_TextureProperties& _Properties, int _iFirstMip, int _Width, int _Height, int _nMip)
{
	//for (int iMip = 0; iMip < _nMip; iMip++)
	//{
	//	CImage* pTexture = _pTC->GetTexture(_iLocal, _iFirstMip + iMip, _TextureVersion);
	//	int MemModel = pTexture->GetMemModel();

	//	if (MemModel & IMAGE_MEM_COMPRESSED)
	//	{
	//		int w = pTexture->GetWidth();
	//		int h = pTexture->GetHeight();
	//		if (!IsPow2(w) || !IsPow2(h))
	//		{
	//			_iFirstMip++;
	//			iMip--;
	//			_nMip--;
	//			_pTC->ReleaseTexture(_iLocal, _iFirstMip + iMip, _TextureVersion);
	//			continue;
	//		}
	//		if (MemModel & IMAGE_MEM_COMPRESSTYPE_S3TC)
	//		{
	//			uint8* pLocked = (uint8*)pTexture->LockCompressed();
	//			CImage_CompressHeader_S3TC* pHeader = (CImage_CompressHeader_S3TC*)pLocked;
	//			uint8* pData = pLocked + pHeader->getOffsetData();

	//			int w = pTexture->GetWidth();
	//			int h = pTexture->GetHeight();

	//			int CompressType = pHeader->getCompressType();
	//			int GLFmt = -1;
	//			switch (CompressType)
	//			{
	//			case IMAGE_COMPRESSTYPE_S3TC_DXT1: GLFmt = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
	//			case IMAGE_COMPRESSTYPE_S3TC_DXT3: GLFmt = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
	//			case IMAGE_COMPRESSTYPE_S3TC_DXT5: GLFmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
	//			default: Error("CRenderContextPS3::LoadTexture", CStrF("Unsupported S3TC compression format: %d", pHeader->getCompressType())); break;
	//			}
	//			glCompressedTexImage2D(_GLImageTarget, iMip, GLFmt, w, h, 0, pHeader->getSizeData(), pData);
	//			GLErr(CStrF("CRenderContextPS3::LoadTexture (glnTexImage2D S3TC %dx%d, %d)", w, h, iMip));

	//			pTexture->Unlock();
	//		}
	//		else
	//			Error("CRenderContextPS3::LoadTexture", "Unsupported compression.");
	//	}
	//	else
	//	{
	//		uint8* pData = (uint8*)pTexture->Lock();
	//		int Fmt = pTexture->GetFormat();
	//		int InternalFormat = -1;
	//		int GLFmt = -1;
	//		int Size = -1;
	//		switch (Fmt)
	//		{
	//		case IMAGE_FORMAT_BGR5: InternalFormat = GL_RGB5; GLFmt = GL_BGR; Size = GL_UNSIGNED_SHORT; break;
	//		case IMAGE_FORMAT_BGR8: InternalFormat = GL_RGB8; GLFmt = GL_BGR; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_BGRA8: InternalFormat = GL_ARGB_SCE; GLFmt = GL_BGRA; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_BGRX8: InternalFormat = GL_ARGB_SCE; GLFmt = GL_BGRA; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_RGBA8: InternalFormat = GL_RGBA8; GLFmt = GL_RGBA; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_A8: InternalFormat = GL_ALPHA8; GLFmt = GL_ALPHA; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_I8A8: InternalFormat = GL_LUMINANCE8_ALPHA8; GLFmt = GL_LUMINANCE_ALPHA; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_I8: InternalFormat = GL_LUMINANCE8; GLFmt = GL_LUMINANCE; Size = GL_UNSIGNED_BYTE; break;
	//		case IMAGE_FORMAT_RGB16: InternalFormat = GL_RGB16; GLFmt = GL_BGR; Size = GL_UNSIGNED_SHORT; break;
	//		case IMAGE_FORMAT_RGBA16: InternalFormat = GL_RGBA16; GLFmt = GL_BGRA; Size = GL_UNSIGNED_SHORT; break;
	//		case IMAGE_FORMAT_RGBA16_F: InternalFormat = GL_RGBA16F_ARB; GLFmt = GL_BGRA; Size = GL_UNSIGNED_SHORT; break;
	//		case IMAGE_FORMAT_RGB32_F: InternalFormat = GL_RGB32F_ARB; GLFmt = GL_BGR; Size = GL_FLOAT; break;
	//		case IMAGE_FORMAT_RGBA32_F: InternalFormat = GL_RGBA32F_ARB; GLFmt = GL_BGRA; Size = GL_FLOAT; break;
	//		case IMAGE_FORMAT_BGRA4: InternalFormat = GL_RGBA4; GLFmt = GL_RGBA; Size = GL_UNSIGNED_BYTE; break;
	//		default: Error("CRenderContextPS3::LoadTexture", CStrF("Unsupported texture format: %d", Fmt)); break;
	//		}
	//		int w = pTexture->GetWidth();
	//		int h = pTexture->GetHeight();
	//		glTexImage2D(_GLImageTarget, iMip, InternalFormat, w, h, 0, GLFmt, Size, pData);
	//		GLErr(CStrF("CRenderContextPS3::LoadTexture (glnTexImage2D %dx%d, %d)", w, h, iMip));

	//		pTexture->Unlock();
	//	}
	//	_pTC->ReleaseTexture(_iLocal, _iFirstMip + iMip, _TextureVersion);
	//}

	//lTextures[_TextureID].Build(_Width, _Height);
	return true;
}

bool CRenderContextSokol::LoadTextureBuildInto(int _GLImageTarget, int _TextureID, CTextureContainer* _pTC, int _iLocal, const CImage& _TxtDesc, const CTC_TextureProperties& _Properties, int _iFirstMip, int _Width, int _Height, int _nMip)
{
	return false;
}

bool CRenderContextSokol::LoadTextureRender(int _GLImageTarget, int _TextureID, CTextureContainer* _pTC, int _iLocal, const CImage& _TxtDesc, const CTC_TextureProperties& _Properties, int _iFirstMip, int _Width, int _Height, int _nMip)
{
	return false;
}

void CRenderContextSokol::GL_BuildTexture2D(int _RCID, int _TextureID, int _GLObjectTarget, int _GLImageTarget, const CTC_TextureProperties& _Properties)
{
}

void CRenderContextSokol::GL_SetTextureParameters(int _RCID, int _TextureID, int _GLObjectTarget, const CTC_TextureProperties& _Properties)
{
}

void CRenderContextSokol::GL_SetTextureParameters(int _TextureID)
{
}

void CRenderContextSokol::GL_UpdateAllTextureParameters()
{
}

void CRenderContextSokol::GL_BuildTexture(int _RCID, int _TextureID, const CTC_TextureProperties& _Properties)
{
}

void CRenderContextSokol::GL_UnloadTexture(int _RCID)
{
}

void CRenderContextSokol::GL_SetTexture(int _TextureID, int _iTexChannel, bool _bPrecache)
{
}
