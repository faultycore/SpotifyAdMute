#pragma once

#include <combaseapi.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

HRESULT InitializeCOM();
HRESULT GetAudioSessionManager(IAudioSessionManager2**);
HRESULT EnumAudioSessions(IAudioSessionManager2* pSessionManager, ISimpleAudioVolume**, DWORD);