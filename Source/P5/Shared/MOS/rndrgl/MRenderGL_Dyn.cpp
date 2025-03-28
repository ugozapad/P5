
#include "MRTC.h"

class CRegisterRenderGL
{
public:
	CRegisterRenderGL()
	{
		MRTC_REFERENCE(CDisplayContextGL);
		MRTC_REFERENCE(CRenderContextGL);
	}
};

CRegisterRenderGL g_RenderGLDyn;
