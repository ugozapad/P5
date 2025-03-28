#ifndef _INC_MDisplaySokol
#define _INC_MDisplaySokol

/*¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯*\
	File:			Display context implementation for Sokol Gfx
\*____________________________________________________________________________________________*/

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| Includes
|__________________________________________________________________________________________________
\*************************************************************************************************/
#include "../../MSystem/Raster/MRender.h"
#include "../../MSystem/Raster/MRCCore.h"
#include <sokol_gfx.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../../Classes/Win32/MWin32.h"

class CRenderContextSokol;

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CDisplayContextSokol
|__________________________________________________________________________________________________
\*************************************************************************************************/
class CDisplayContextSokol : public CDisplayContext
{
protected:
	MRTC_DECLARE;

	CImage m_Image;
	CPnt m_Size;
	CPnt m_MaxSize;

	CRenderContextSokol* m_pRenderContext;

	CWin32_Window m_Window;

	HDC m_hDC = NULL;
	HGLRC m_hGLRC = NULL;

	uint32 m_bLog : 1;
	uint32 m_bVSync : 1;
	uint32 m_bAntialias : 1;
	uint32 m_bAddedToConsole : 1;
	uint32 m_bPendingResetMode : 1;
	uint32 m_BackBufferFormat;

private:
	void InitRC();
	void InitGLRC();

public:
	CDisplayContextSokol();
	~CDisplayContextSokol();

	virtual CPnt GetScreenSize() { return m_Size; }
	virtual CPnt GetMaxWindowSize() { return m_MaxSize; }

	virtual void Create();

	virtual void SetMode(int nr);

	virtual void ModeList_Init();

	virtual int SpawnWindow(int _Flags = 0);
	virtual void DeleteWindow(int _iWnd);
	virtual void SelectWindow(int _iWnd);

	virtual void SetWindowPosition(int _iWnd, CRct _Rct);

	virtual void SetPalette(spCImagePalette _spPal);

	virtual CImage* GetFrameBuffer();

	virtual void ClearFrameBuffer(int _Buffers = (CDC_CLEAR_COLOR | CDC_CLEAR_ZBUFFER | CDC_CLEAR_STENCIL), int _Color = 0);

	int GetMode();

	virtual int PageFlip();

	// Scriptstuff
	virtual void Con_SetDataPath(CStr _DataPath);
	void Con_r_antialias(int _Antialias);
	void Con_r_backbufferformat(int _BackbufferFormat);
	virtual void Parser_Modes();
	virtual void Parser_VSync(int _VSync);
	virtual void Parser_VMode(int width, int height);
	virtual void Register(CScriptRegisterContext& _RegContext);

	// Subsystem
	virtual void OnRefresh(int _Context);
	virtual void OnBusy(int _Context);

	// Inherited via CDisplayContext
	virtual CRenderContext* GetRenderContext(CRCLock* _pLock);

	virtual	int Win32_CreateFromWindow(void* _hWnd, int _Flags = 0);
	virtual int Win32_CreateWindow(int _WS, void* _pWndParent, int _Flags = 0);
	virtual void Win32_ProcessMessages();
	virtual void* Win32_GethWnd(int _iWnd = 0);
};

CDisplayContext* gf_CreateDisplayContextSokolStatic();

#endif // _INC_MDisplaySokol