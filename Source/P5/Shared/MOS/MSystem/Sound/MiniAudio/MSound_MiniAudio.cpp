#include "PCH.h"

#include "MMath_Vec128.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "MSound_MiniAudio.h"
#include "../../MSystem.h"

CSoundContext_MiniAudio* g_MiniAudioSoundContext = NULL;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
    // pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
    // frameCount frames.
}

// -------------------------------------------------------------------
//  CSoundContext_MiniAudio
// -------------------------------------------------------------------
MRTC_IMPLEMENT_DYNAMIC(CSoundContext_MiniAudio, CSoundContext);

void CSoundContext_MiniAudio::CreateDevice()
{
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config.sampleRate        = 48000;           // Set to 0 to use the device's native sample rate.
    config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = this;            // Can be accessed from the device object (device.pUserData).

    if (ma_device_init(NULL, &config, &m_Device) != MA_SUCCESS) {
        Error_static("CSoundContext_MiniAudio::CreateDevice", "Failed to create device");
    }

    ConOutL("(CSoundContext_MiniAudio::CreateDevice): MiniAudio is initialized");

    m_bCreated = true;
}

void CSoundContext_MiniAudio::DestroyDevice()
{
    ma_device_uninit(&m_Device);

    // Clear device
    memset(&m_Device, 0, sizeof(m_Device));

    m_bCreated = false;
}

CStaticVoice_MiniAudio* CSoundContext_MiniAudio::GetStaticVoice(int _WaveID)
{
    return nullptr;
}

void CSoundContext_MiniAudio::CreateAll()
{
}

void CSoundContext_MiniAudio::DestroyAll()
{
}

CSoundContext_MiniAudio::CSoundContext_MiniAudio()
{
    g_MiniAudioSoundContext = this;

    m_Recursion = 0;

    //	m_StaticVoiceHash.InitHash(2,(uint32)(&((CStaticVoice_PS3 *)(0x70000000))->m_WaveID) - 0x70000000,2);

    m_DebugStrTemp = NULL;

    m_PortNumber = 0xffffffff;

    m_bCreated = false;
    m_pMainThreadID = MRTC_SystemInfo::OS_GetThreadID();

    MACRO_AddSubSystem(this);
}

CSoundContext_MiniAudio::~CSoundContext_MiniAudio()
{
    KillThreads();

    MACRO_RemoveSubSystem(this);

    //for (int i = 0; i < m_lpStaicVoices.Len(); i++)
    //{
    //    if (m_lpStaicVoices[i])
    //        delete m_lpStaicVoices[i];
    //}
    m_lpStaicVoices.Clear();


    DestroyDevice();

    if (m_DebugStrTemp)
    {
        for (int i = 0; i < mcs_MaxDebugSounds - 1; ++i)
            delete[] m_DebugStrTemp[i];

        delete[] m_DebugStrTemp;
    }

    g_MiniAudioSoundContext = NULL;
}

void CSoundContext_MiniAudio::Create(CStr _Params)
{
}

void CSoundContext_MiniAudio::KillThreads()
{
}

void CSoundContext_MiniAudio::Platform_GetInfo(CPlatformInfo& _Info)
{
    memset(&_Info, 0, sizeof(_Info));

    _Info.m_nProssingThreads = 1;
    _Info.m_nChannels = 8;
    _Info.m_FrameLength = 256;
    _Info.m_SampleRate = 48000;
    _Info.m_Speakers[0].m_Position = CVec3Dfp32(-1.0f, -1.0f, 0.0f); // Front Left
    _Info.m_Speakers[1].m_Position = CVec3Dfp32(-1.0f, 1.0f, 0.0f); // Front Right
    _Info.m_Speakers[2].m_Position = CVec3Dfp32(0.0f, -1.0f, 0.0f); // Surround Left
    _Info.m_Speakers[3].m_Position = CVec3Dfp32(0.0f, 1.0f, 0.0f); // Surround Right
    _Info.m_Speakers[4].m_Position = CVec3Dfp32(-1.0f, 0.0f, 0.0f); // Front Center
    _Info.m_Speakers[5].m_Position = CVec3Dfp32(0.0f, 0.0f, 0.0f); // LFE
    _Info.m_Speakers[5].m_bLFE = true;
    _Info.m_Speakers[6].m_Position = CVec3Dfp32(1.0f, -0.5f, 0.0f); // Back Left
    _Info.m_Speakers[7].m_Position = CVec3Dfp32(1.0f, 0.5f, 0.0f); // Back Right
}

void CSoundContext_MiniAudio::Platform_Init(uint32 _MaxMixerVoices)
{
}

void CSoundContext_MiniAudio::Platform_StartStreamingToMixer(uint32 _MixerVoice, CVoice* _pVoice, uint32 _WaveID, fp32 _SampleRate)
{
}

void CSoundContext_MiniAudio::Platform_StopStreamingToMixer(uint32 _MixerVoice)
{
}

void CSoundContext_MiniAudio::Wave_Precache(int _WaveID)
{
}

void CSoundContext_MiniAudio::Wave_PrecacheFlush()
{
}

void CSoundContext_MiniAudio::Wave_PrecacheBegin(int _Count)
{
}

void CSoundContext_MiniAudio::Wave_PrecacheEnd(uint16* _pPreCacheOrder, int _nIds)
{
}

int CSoundContext_MiniAudio::Wave_GetMemusage(int _WaveID)
{
    return 0;
}

const char** CSoundContext_MiniAudio::GetDebugStrings()
{
    const char** pSuperStrings = CSuper::GetDebugStrings();
    return (const char**)pSuperStrings;
}

bool CSoundContext_MiniAudio::UpdateVoiceDelete()
{
    return false;
}

// -------------------------------------------------------------------
void CSoundContext_MiniAudio::Refresh()
{
    DLock(m_InterfaceLock);

    if (!m_bCreated)
        return;

    if (m_Recursion) return;
    m_Recursion++;

    M_TRY
    {
        {
            DUnlock(m_InterfaceLock);
            UpdateVoiceDelete();
        }

        CSuper::Refresh();
    }
        M_CATCH(
            catch (CCException)
    {
        m_Recursion--;
        throw;
    }
        )
        m_Recursion--;


}

// -------------------------------------------------------------------
aint CSoundContext_MiniAudio::OnMessage(const CSS_Msg& _Msg)
{
    return 0;
}

void CSoundContext_MiniAudio::OnRefresh(int _Context)
{
    Refresh();
}

void CSoundContext_MiniAudio::OnBusy(int _Context)
{
    Refresh();
}
