#include "PCH.h"
#include "MInputWin32.h"

// -------------------------------------------------------------------
//  Implement MRTC
// -------------------------------------------------------------------
MRTC_IMPLEMENT_DYNAMIC(CDI_Device, CReferenceCount);
MRTC_IMPLEMENT_DYNAMIC(CDI_Device_Keyboard, CDI_Device);
MRTC_IMPLEMENT_DYNAMIC(CInputContext_DInput, CInputContext);
MRTC_IMPLEMENT_DYNAMIC(CInputContext_Win32, CInputContext);


IMPLEMENT_OPERATOR_NEW(CInputContext_DInput);

// -------------------------------------------------------------------
//  CDI_Device
// -------------------------------------------------------------------
CDI_Device::CDI_Device()
{
}

CDI_Device::~CDI_Device()
{
}

void CDI_Device::Destroy()
{
}

bool CDI_Device::Create(CInputContext_DInput* _pInput, int _PortNumber, int _iDevice)
{
	return false;
}

void CDI_Device::OnRefresh()
{
}

void CDI_Device::OnLogCaps()
{
}

void CDI_Device::OnFeedback(const float _fForce1, const float _fForce2)
{
}

// -------------------------------------------------------------------
//  CDI_Device_Keyboard
// -------------------------------------------------------------------
CDI_Device_Keyboard::CDI_Device_Keyboard()
{
}

bool CDI_Device_Keyboard::Create(CInputContext_DInput* _pInput, int _PortNumber, int _iDevice)
{
	return false;
}

void CDI_Device_Keyboard::OnRefresh()
{
}

// -------------------------------------------------------------------
//  CInputContext_DInput
// -------------------------------------------------------------------
CInputContext_DInput::CInputContext_DInput()
{
}

CInputContext_DInput::~CInputContext_DInput()
{
}

void CInputContext_DInput::Create(const char* _pParams)
{
	LogFile("(CInputContext_DInput::Create)");
	CInputContextCore::Create(_pParams);

	// For DInput
#if 0
	m_lspDevices.SetLen(INPS3_MAXJOYSTICKS + INPS3_MAXKEYBOARDS);

	for (int i = 0; i < INPS3_MAXKEYBOARDS; i++)
	{
		/*
				if ((cellKbSetRepeat (i, 30, 2)) != CELL_KB_OK)
				{
					Error("Create", "Failed to initialize USB Keyboard Library attrib");
				}
				if ((cellKbSetArrangement (i, CELL_KB_ARRANGEMENT_101))!= CELL_KB_OK)
				{
					Error("Create", "Failed to initialize USB Keyboard Library attrib");
				}
		 */
		if ((cellKbSetReadMode(i, CELL_KB_RMODE_INPUTCHAR)) != CELL_KB_OK)
		{
			Error("Create", "Failed to initialize USB Keyboard Library attrib");
		}
		if ((cellKbSetCodeType(i, CELL_KB_CODETYPE_ASCII)) != CELL_KB_OK)
		{
			Error("Create", "Failed to initialize USB Keyboard Library attrib");
		}
	}


	DI_CreateDevices();

	Update();
#endif
}

void CInputContext_DInput::Update()
{
	// For DInput
#if 0
	// Any device changes?
	DI_CreateDevices();

	// Refresh all devices
	M_TRY
	{
		//		MACRO_GetRegisterObject(CSystem, pSys, "SYSTEM");

				for (int iDev = 0; iDev < m_lspDevices.Len(); iDev++)
				{
					CPS3_Device* pDev = m_lspDevices[iDev];
					if (pDev)
					{
						pDev->OnRefresh();
					}
				}
	}
		M_CATCH(
			catch (CCException)
	{
	}
		)
#endif
}

bool CInputContext_DInput::IsGamepadValid(int _iPad)
{
	return false;
}

aint CInputContext_DInput::OnMessage(const CSS_Msg& _Msg)
{
	return CSubSystem::OnMessage(_Msg);
}

// -------------------------------------------------------------------
//  Win32 Input Context WndProc callback
// -------------------------------------------------------------------
LRESULT CALLBACK CInputContext_Win32_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CInputContext_Win32* self = reinterpret_cast<CInputContext_Win32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return self->WndProc(hWnd, uMsg, wParam, lParam);
}

// -------------------------------------------------------------------
//  CInputContext_Win32
// -------------------------------------------------------------------
CInputContext_Win32::CInputContext_Win32() :
	m_iMainWindow(0),
	m_hMainWnd(0),
	m_pWndProc(0)
{
}

CInputContext_Win32::~CInputContext_Win32()
{
}

void CInputContext_Win32::Create(const char* _pParams)
{
	LogFile("(CInputContext_Win32::Create)");
	CInputContextCore::Create(_pParams);

	MACRO_AddSubSystem(this);
}

void CInputContext_Win32::Update()
{
	M_LOCK(m_InputLock);

	POINT pt;
	::GetCursorPos(&pt);
}

bool CInputContext_Win32::IsGamepadValid(int _iPad)
{
	return false;
}

aint CInputContext_Win32::OnMessage(const CSS_Msg& _Msg)
{
	switch (_Msg.m_Msg)
	{
	case CSS_MSG_WIN32_PREDESTROYWINDOW:
	{
		if (m_iMainWindow >= 0 && _Msg.m_Params[1] == m_iMainWindow)
		{
			WindowDetach(m_hMainWnd);
			m_hMainWnd = NULL;
		}
	}
	break;

	case CSS_MSG_WIN32_CREATEDWINDOW:
	{
		if (m_iMainWindow >= 0 && _Msg.m_Params[1] == m_iMainWindow)
		{
			MACRO_GetRegisterObject(CSystem, pSys, "SYSTEM");
			m_hMainWnd = (HWND)pSys->m_spDisplay->Win32_GethWnd(m_iMainWindow);
			WindowAttach(m_hMainWnd);
		}
	}
	break;
	}

	return CSubSystem::OnMessage(_Msg);
}

void CInputContext_Win32::WindowAttach(HWND hWnd)
{
	m_pWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)CInputContext_Win32_WndProc);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
}

void CInputContext_Win32::WindowDetach(HWND hWnd)
{
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)m_pWndProc);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
}

LRESULT CALLBACK CInputContext_Win32::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int scanCode = MapVirtualKey((UINT)wParam, MAPVK_VK_TO_VSC);

	switch (uMsg)
	{
	case WM_KEYDOWN:
		DownKey(scanCode, 0, 0.0f, 0);
		break;

	case WM_KEYUP:
		UpKey(scanCode, 0.0f, 0, 0);
		break;

	default:
		break;
	}

	return CallWindowProc(m_pWndProc, hWnd, uMsg, wParam, lParam);
}
