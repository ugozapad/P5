#ifndef _INC_MINPUT_INWIN32
#define _INC_MINPUT_INWIN32

/*
// +-----------------------------------------------------------------+
// | CInputContext_DInput					                         |
// +-----------------------------------------------------------------+
// | Creator:          Magnus Högdahl                                |
// | Created:          2001-02-05                                    |
// | Last modified:    2001-__-__                                    |
// |                                                                 |
// | Description:                                                    |
// |                                                                 |
// |                                                                 |
// +-----------------------------------------------------------------+
// | Copyright Starbreeze Studios AB 2001							 |
// +-----------------------------------------------------------------+
*/

#include "../MInputCore.h"

#include <dinput.h>

class CInputContext_DInput;
// -------------------------------------------------------------------
//  CDI_Device
// -------------------------------------------------------------------
class CDI_Device : public CReferenceCount
{
	MRTC_DECLARE;
public:
	CInputContext_DInput* m_pInput;

	CDI_Device();
	~CDI_Device();
	virtual void Destroy();
	virtual bool Create(CInputContext_DInput* _pInput, int _PortNumber, int _iDevice = 0);

	virtual void OnRefresh();

	virtual void OnLogCaps();

	virtual void OnFeedback(const float _fForce1, const float _fForce2);

};

typedef TPtr<CDI_Device> spCDI_Device;

// -------------------------------------------------------------------
//  CDI_Device_Keyboard
// -------------------------------------------------------------------
class CDI_Device_Keyboard : public CDI_Device
{
	MRTC_DECLARE;

	int m_KBDNr;
	int m_LastModifier;

public:
	CDI_Device_Keyboard();
	virtual bool Create(CInputContext_DInput* _pInput, int _PortNumber, int _iDevice = 0);
	virtual void OnRefresh();
};

typedef TPtr<CDI_Device_Keyboard> spCDI_Device_Keyboard;

// -------------------------------------------------------------------
//  CInputContext_DInput
// -------------------------------------------------------------------
class SYSTEMDLLEXPORT CInputContext_DInput : public CInputContextCore
{
	MRTC_DECLARE;

private:


	// -------------------------------------------------------------------
public:


	DECLARE_OPERATOR_NEW


	CInputContext_DInput();
	~CInputContext_DInput();

	// -------------------------------------------------------------------
	// Overrides from CInputContext
	virtual void Create(const char* _pParams);
	virtual void Update();
	virtual bool IsGamepadValid(int _iPad);

	// -------------------------------------------------------------------
	// Overrides from CSubSystem
	virtual aint OnMessage(const CSS_Msg& _Msg);
};

typedef TPtr<CInputContext_DInput> spCInputContext_DInput;

// -------------------------------------------------------------------
//  CInputContext_Win32
// -------------------------------------------------------------------
class SYSTEMDLLEXPORT CInputContext_Win32 : public CInputContextCore
{
	MRTC_DECLARE;

private:
	int m_iMainWindow;
	HWND m_hMainWnd;
	WNDPROC m_pWndProc;

	// -------------------------------------------------------------------
public:


	DECLARE_OPERATOR_NEW


	CInputContext_Win32();
	~CInputContext_Win32();

	// -------------------------------------------------------------------
	// Overrides from CInputContext
	virtual void Create(const char* _pParams);
	virtual void Update();
	virtual bool IsGamepadValid(int _iPad);

	// -------------------------------------------------------------------
	// Overrides from CSubSystem
	virtual aint OnMessage(const CSS_Msg& _Msg);

	void WindowAttach(HWND hWnd);
	void WindowDetach(HWND hWnd);

	// Window Procedure
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

typedef TPtr<CInputContext_Win32> spCInputContext_Win32;


#endif // _INC_MINPUT_INWIN32
