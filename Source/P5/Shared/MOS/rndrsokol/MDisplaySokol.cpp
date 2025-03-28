#include "PCH.h"
#include "MDisplaySokol.h"
#include "MRenderSokol.h"

#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB       0x2093
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define ERROR_INVALID_VERSION_ARB         0x2095
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);

#pragma comment(lib, "opengl32.lib")

// Dynamic Initializer
class CRegisterRenderSokol
{
public:
	CRegisterRenderSokol()
	{
		MRTC_REFERENCE(CDisplayContextSokol);
		MRTC_REFERENCE(CRenderContextSokol);
	}
};

CRegisterRenderSokol g_RegisterRenderSokol;

MRTC_IMPLEMENT_DYNAMIC(CDisplayContextSokol, CDisplayContext);

CDisplayContextSokol::CDisplayContextSokol()
{
	m_pRenderContext = NULL;

	m_Size.x = 1024;
	m_Size.y = 768;

	m_MaxSize = m_Size;

	m_Image.Create(m_Size.x, m_Size.y, IMAGE_FORMAT_BGRA8, IMAGE_MEM_IMAGE);

	m_bLog = false;
	m_bVSync = false;
	m_bAntialias = false;
	m_bAddedToConsole = false;
	m_bPendingResetMode = false;
	m_BackBufferFormat = IMAGE_FORMAT_BGRA8;
}

CDisplayContextSokol::~CDisplayContextSokol()
{
	if (m_bAddedToConsole)
	{
		MACRO_RemoveSubSystem(this);
		RemoveFromConsole();
	}
}

void CDisplayContextSokol::Create()
{
	CDisplayContext::Create();

	AddToConsole();
	m_bAddedToConsole = true;

	//m_pRenderContext = (CRenderContextSokol*)MRTC_GOM()->CreateObject("CRenderContextSokol");
	//if (!m_pRenderContext)
	//	Error("CDisplayContextSokol::Create", "Failed to create render-context.");
	//
	//m_pRenderContext->Create(this, NULL);

	MACRO_AddSubSystem(this);
}

void CDisplayContextSokol::InitRC()
{
	// Destroy previous context
	if (m_pRenderContext)
	{
		delete m_pRenderContext;
		m_pRenderContext = NULL;
	}

	m_pRenderContext = (CRenderContextSokol*)MRTC_GOM()->CreateObject("CRenderContextSokol");
	if (!m_pRenderContext)
		Error("CDisplayContextSokol::Create", "Failed to create render-context.");

	m_pRenderContext->Create(this, NULL);
}

void CDisplayContextSokol::InitGLRC()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	m_hDC = GetDC(m_Window.GethWnd());
	int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, pixelFormat, &pfd);

	HGLRC tempContext = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, tempContext);

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	M_ASSERT(wglCreateContextAttribsARB, "Cannot load ARB context creation function.");

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	m_hGLRC = wglCreateContextAttribsARB(m_hDC, 0, attribs);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);
	wglMakeCurrent(m_hDC, m_hGLRC);
}

void CDisplayContextSokol::SetMode(int nr)
{
	M_TRACEALWAYS("CDisplayContextSokol::SetMode: %i\n", nr);

	
}

void CDisplayContextSokol::ModeList_Init()
{
}

int CDisplayContextSokol::SpawnWindow(int _Flags)
{
	CRct Rect;
	Rect.p0 = CPnt(0, 0);
	Rect.p1 = m_Size;
	m_Window.Create(Rect);

	InitGLRC();
	InitRC();

	return 0;
}

void CDisplayContextSokol::DeleteWindow(int _iWnd)
{
}

void CDisplayContextSokol::SelectWindow(int _iWnd)
{
}

void CDisplayContextSokol::SetWindowPosition(int _iWnd, CRct _Rct)
{
}

void CDisplayContextSokol::SetPalette(spCImagePalette _spPal)
{
}

CImage* CDisplayContextSokol::GetFrameBuffer()
{
	return &m_Image;
}

void CDisplayContextSokol::ClearFrameBuffer(int _Buffers, int _Color)
{
	//M_TRACEALWAYS("CDisplayContextSokol::ClearFrameBuffer: %i %i\n", _Buffers, _Color);

	if (m_pRenderContext)
		m_pRenderContext->RenderTarget_Clear(CRct(0, 0, m_Size.x, m_Size.y), 0, CPixel32(155, 155, 155, 255), 1.0f, 0);
}

int CDisplayContextSokol::GetMode()
{
	return 0;
}

int CDisplayContextSokol::PageFlip()
{
	SwapBuffers(m_hDC);
	return 0;
}

// --------------------------------
//  CConsoleClient virtuals.
// --------------------------------
void CDisplayContextSokol::Con_SetDataPath(CStr _DataPath)
{
	DebugBreak();
}

void CDisplayContextSokol::Con_r_antialias(int _Antialias)
{
	bool Antialias = _Antialias > 1;
	if (m_bAntialias != Antialias)
	{
		m_bAntialias = Antialias;
		MACRO_GetSystemEnvironment(pEnv);
		pEnv->SetValuei("R_ANTIALIAS", _Antialias);
		m_bPendingResetMode = true;
	}
}

void CDisplayContextSokol::Con_r_backbufferformat(int _BackbufferFormat)
{
	if (m_BackBufferFormat != _BackbufferFormat)
	{
		m_BackBufferFormat = _BackbufferFormat;
		MACRO_GetSystemEnvironment(pEnv);
		pEnv->SetValuei("R_BACKBUFFERFORMAT", _BackbufferFormat);
		m_bPendingResetMode = true;
	}
}


void CDisplayContextSokol::Parser_Modes()
{
	for (int i = 0; i < m_lspModes.Len(); i++)
		ConOut(CStrF("%d : %s, %d hz", i, m_lspModes[i]->m_Format.IDString().Str(), m_lspModes[i]->m_RefreshRate));
}

void CDisplayContextSokol::Parser_VSync(int _VSync)
{
	m_bVSync = _VSync != 0;
}

void CDisplayContextSokol::Parser_VMode(int width, int height)
{
}

void CDisplayContextSokol::Register(CScriptRegisterContext& _RegContext)
{
	CDisplayContext::Register(_RegContext);

	_RegContext.RegFunction("vwinsize", this, &CDisplayContextSokol::Parser_VMode);
	_RegContext.RegFunction("r_vsync", this, &CDisplayContextSokol::Parser_VSync);
	_RegContext.RegFunction("r_vmodes", this, &CDisplayContextSokol::Parser_Modes);
	_RegContext.RegFunction("r_antialias", this, &CDisplayContextSokol::Con_r_antialias);
	_RegContext.RegFunction("r_backbufferformat", this, &CDisplayContextSokol::Con_r_backbufferformat);
}

void CDisplayContextSokol::OnRefresh(int _Context)
{
	CDisplayContext::OnRefresh(_Context);

	Win32_ProcessMessages();
}

void CDisplayContextSokol::OnBusy(int _Context)
{
	CDisplayContext::OnBusy(_Context);
}

CRenderContext* CDisplayContextSokol::GetRenderContext(CRCLock* _pLock)
{
	return m_pRenderContext;
}

int CDisplayContextSokol::Win32_CreateFromWindow(void* _hWnd, int _Flags /*= 0*/)
{
	return 0;
}

int CDisplayContextSokol::Win32_CreateWindow(int _WS, void* _pWndParent, int _Flags /*= 0*/)
{
	return 0;
}

void CDisplayContextSokol::Win32_ProcessMessages()
{
	m_Window.ProcessMessages();
}

void* CDisplayContextSokol::Win32_GethWnd(int _iWnd /*= 0*/)
{
	return nullptr;
}

CDisplayContext* gf_CreateDisplayContextSokolStatic()
{
	return (CDisplayContext*)MRTC_GetObjectManager()->CreateObject("CDisplayContextSokol");
}
