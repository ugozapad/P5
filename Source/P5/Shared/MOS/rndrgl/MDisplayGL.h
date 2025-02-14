/*
------------------------------------------------------------------------------------------------
Name:		MDispGL.cpp/h
Purpose:	Display context
Creation:	9703??

Contents:
class				CDisplayContextGL				9703??  -				CDisplayContext for OpenGL

------------------------------------------------------------------------------------------------
*/
#ifndef _INC_MOS_DispGL
#define _INC_MOS_DispGL

#include "../../MSystem/MSystem.h"

class CDisplayContextGL : public CDisplayContext
{
	MRTC_DECLARE;
public:
	CDisplayContextGL();
	~CDisplayContextGL();

	void Create() override;

	CPnt GetScreenSize() override;
	CPnt GetMaxWindowSize() override;
	void SetMode(int nr) override;
	void ModeList_Init() override;
	int SpawnWindow(int _Flags = 0) override;
	void DeleteWindow(int _iWnd) override;
	void SelectWindow(int _iWnd) override;
	void SetWindowPosition(int _iWnd, CRct _Rct) override;
	void SetPalette(spCImagePalette _spPal) override;
	CImage* GetFrameBuffer() override;
	void ClearFrameBuffer(int _Buffers = 49, int _Color = 0) override;
	CRenderContext* GetRenderContext(CRCLock* _pLock) override;
	int Win32_CreateFromWindow(void* _hWnd, int _Flags = 0) override;
	int Win32_CreateWindow(int _WS, void* _pWndParent, int _Flags = 0) override;
	void Win32_ProcessMessages() override;
	void* Win32_GethWnd(int _iWnd = 0) override;

	void Register(NScript::CRegisterContext& _RegContext) override;

private:

	CPnt m_ScreenSize;
	void* m_hWnd;
};

#endif // _INC_MOS_DispGL
