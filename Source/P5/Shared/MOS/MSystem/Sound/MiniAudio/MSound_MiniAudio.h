

#ifndef DInc_MSound_MiniAudio_h
#define DInc_MSound_MiniAudio_h

#include "../MSound.h"
#include "../MSound_Mixer.h"
#include "../SCMixer/MSound_SCMixer.h"

#include "../../Misc/MPerfGraph.h"

class CMixerOutputEffect;
// -------------------------------------------------------------------
class CSoundContext_MiniAudio : public CSoundContext_Mixer, public CSubSystem
{
	friend class CStaticVoice_MiniAudio;
	friend class CSCMiniAudio_Buffer;

	typedef CSoundContext_Mixer CSuper;

	MRTC_DECLARE;

	const static int mcs_MaxDebugSounds = 384;
	char ** m_DebugStrTemp;

	int m_bCreated;
	uint32 m_Recursion;

	uint32 m_PortNumber;

	void *m_pMainThreadID;

	TThinArray<CStaticVoice_MiniAudio *> m_lpStaicVoices;

	ma_device m_Device;

private:

	//void Thread_Submit();
	//void Thread_Read();
	//void Thread_StartSounds();
	//void Thread_AudioRingBuffer();

	void CreateDevice();
	void DestroyDevice();

	CStaticVoice_MiniAudio *GetStaticVoice(int _WaveID);

	void CreateAll();
	void DestroyAll();

public:

	CSoundContext_MiniAudio();
	~CSoundContext_MiniAudio();
	virtual void Create(CStr _Params);
	virtual void KillThreads();

	virtual void Platform_GetInfo(CPlatformInfo &_Info);
	virtual void Platform_Init(uint32 _MaxMixerVoices);
	virtual void Platform_StartStreamingToMixer(uint32 _MixerVoice, CVoice *_pVoice, uint32 _WaveID, fp32 _SampleRate);
	virtual void Platform_StopStreamingToMixer(uint32 _MixerVoice);

	// Precache
	virtual void Wave_Precache(int _WaveID);
	virtual void Wave_PrecacheFlush();
	virtual void Wave_PrecacheBegin( int _Count );
	virtual void Wave_PrecacheEnd(uint16 *_pPreCacheOrder, int _nIds);
	virtual	int Wave_GetMemusage(int _WaveID);

	virtual const char **GetDebugStrings();

public:
	bool UpdateVoiceDelete();

	virtual void Refresh();

	// Overrides from CSubSystem
	virtual aint OnMessage(const CSS_Msg& _Msg);
	virtual void OnRefresh(int _Context);
	virtual void OnBusy(int _Context);
};

extern CSoundContext_MiniAudio *g_MiniAudioSoundContext;

typedef TPtr<CSoundContext_MiniAudio> spCSoundContext_MiniAudio;

#endif // DInc_MSound_MiniAudio_h
