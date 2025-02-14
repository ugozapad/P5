#include "MRTC.h"
#include "MDisplayGL.h"
#include "MRenderGL.h"

class CRegisterRenderGL
{
public:
	CRegisterRenderGL()
	{
		MRTC_REFERENCE(CDisplayContextGL);
		MRTC_REFERENCE(CRenderContextGL);
	}
};

CRegisterRenderGL g_RegisterRenderGL;
