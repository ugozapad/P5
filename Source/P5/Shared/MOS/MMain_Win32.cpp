
#include "../Platform/Platform.h"

#define MOSMain_ShowError(Err) printf( "%s\n", (char*)Err)


/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| Win32_Main
|__________________________________________________________________________________________________
\*************************************************************************************************/
int Win32_Main(void* this_inst, void* prev_inst, char* cmdline, int cmdshow, const char* _pAppClassName, const char* _pSystemClassName /*= NULL*/)
{
	MRTC_ObjectManager* pObjMgr = MRTC_GetObjectManager();

	pObjMgr->SetDllLoading(false);

	// -------------------------------------------------------------------
	// Create exception-log
	M_TRY
	{
		spCReferenceCount spObj = pObjMgr->CreateObject("CCExceptionLog", "SYSTEM.EXCEPTIONLOG");
		if (spObj != NULL)
		{
			MACRO_GetRegisterObject(CCExceptionLog, pLog, "SYSTEM.EXCEPTIONLOG");
			if (!pLog) Error_static("Win32_Main", "Unable to create exception-log object.");
		}
	}
	M_CATCH(
	catch(CCException _Ex)
	{
		MOSMain_ShowError(_Ex.GetExceptionInfo().GetString());
		exit(0);
	}
	);

	// -------------------------------------------------------------------
	// Create logfile object
	M_TRY
	{
		spCReferenceCount spObj = pObjMgr->CreateObject("CLogFile", "SYSTEM.LOG");
		if (spObj != NULL)
		{
			ILogFile* pLog = TDynamicCast<ILogFile>((CReferenceCount*)spObj);
			if (!pLog) Error_static("Win32_Main", "Unable to create logfile object.");

			pLog->Create("SBZ");
		}
	}
	M_CATCH(
	catch(CCException _Ex)
	{
		MOSMain_ShowError(_Ex.GetExceptionInfo().GetString());
		exit(0);
	}
	);

	M_TRY
	{
		// -------------------------------------------------------------------
		M_TRY
		{
			// -------------------------------------------------------------------
			//  Create CSystem
			spCReferenceCount spObj;
			
			spObj = (CReferenceCount*)pObjMgr->CreateObject("CSystemWin32");

			if (!spObj) Error_static("Win32_Main", "Unable to instance CSystemWin32 object.");

			TPtr<CSystem> spSys = safe_cast<CSystem>((CReferenceCount*)spObj);
			if (!spSys) Error_static("Win32_Main", "CSystemWin32 object was not a class of CSystem.");
			//spObj = NULL;

			MRTC_GOM()->RegisterObject((CReferenceCount*)spSys, "SYSTEM");		
			M_TRY
			{
				spSys->Create(NULL, NULL, NULL, NULL, _pAppClassName);
				spSys->DoModal();
			}
			M_CATCH(
			catch(CCException)
			{
				spSys->Destroy();
				MRTC_GOM()->UnregisterObject((CReferenceCount*)spSys, "SYSTEM");
				throw;
			}
			);
			spSys->Destroy();
			MRTC_GOM()->UnregisterObject((CReferenceCount*)spSys, "SYSTEM");
		}
		M_CATCH(
		catch(CCException _Ex)
		{
			MACRO_GetRegisterObject(CCExceptionLog, pLog, "SYSTEM.EXCEPTIONLOG");
			if (pLog) 
			{
				pLog->DisplayFatal();
			}
			else
				MOSMain_ShowError(_Ex.GetExceptionInfo().GetString());
		}
		);

//		MRTC_GetObjectManager()->UnregisterAll();
	}
	M_CATCH(
	catch(CCException _Ex)
	{
		MOSMain_ShowError(_Ex.GetExceptionInfo().GetString());
	}
	);

	pObjMgr->UnregisterObject(NULL, "SYSTEM.LOG");
	pObjMgr->UnregisterObject(NULL, "SYSTEM.EXCEPTIONLOG");

	return 0;
}