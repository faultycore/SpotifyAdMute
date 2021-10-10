#pragma once

#include <Windows.h>
#include <audiopolicy.h>

template<class T> 
void safe_release(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

HRESULT initialize_com();

HRESULT audio_session_manager(IAudioSessionManager2**);

HRESULT enum_audio_sessions(ISimpleAudioVolume**);

HRESULT change_mute_status(ISimpleAudioVolume*, bool);
