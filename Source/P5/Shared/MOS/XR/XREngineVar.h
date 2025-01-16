#ifndef __XREngineVar_H__
#define __XREngineVar_H__

enum
{
	 XR_ENGINE_MODE_LIGHTMAP		= 0
	,XR_ENGINE_MODE_UNIFIED		= 1

	,XR_ENGINE_MODE = 0
	,XR_ENGINE_FLARES
	,XR_ENGINE_DYNLIGHT
	,XR_ENGINE_FASTLIGHT
	,XR_ENGINE_WALLMARKS
	,XR_ENGINE_LODOFFSET
	,XR_ENGINE_LODSCALE
	,XR_ENGINE_VBSKIP
	,XR_ENGINE_ALLOWDEPTHFOG
	,XR_ENGINE_ALLOWVERTEXFOG
	,XR_ENGINE_ALLOWNHF
	,XR_ENGINE_LIGHTSCALE
	,XR_ENGINE_SURFOPTIONS
	,XR_ENGINE_SURFCAPS
	,XR_ENGINE_STENCILSHADOWS
	,XR_ENGINE_SETENGINECLIENT
	,XR_ENGINE_SHADOWDECALS
	,XR_ENGINE_NORENDERTOTEXTURE
	,XR_ENGINE_FOGCULLOFFSET
	,XR_ENGINE_CLEARVIEWPORT

	,XR_ENGINE_UNIFIED_ADDLIGHTOCCLUSION
	,XR_ENGINE_UNIFIED_AMBIENCE

	,XR_ENGINE_PBNIGHTVISIONLIGHT

	// Interfaces
	,XR_ENGINE_TCSHADOWDECALS
	,XR_ENGINE_TCSCREEN
	,XR_ENGINE_MINAVAILABLEVBHEAP
};

#endif // __XREngineVar_H__
