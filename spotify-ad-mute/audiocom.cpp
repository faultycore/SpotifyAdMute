#include "audiocom.hpp"
#include "utility.hpp"

// initializes COM library 
// return value can be checked with SUCCEEDED() or FAILED() macro
HRESULT InitializeCOM()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	return hr;
}

// writes an object of IAudioSessionManager2* type to pSessionManager arg
// return value can be checked with SUCCEEDED() or FAILED() macro
HRESULT GetAudioSessionManager(IAudioSessionManager2** pSessionManager)
{
	// get GUIDs of COM class and interface
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	IAudioSessionManager2* pAudioManager = NULL;
	IMMDeviceEnumerator* pDeviceEnumerator = NULL;
	IMMDevice* pDevice = NULL;

	HRESULT hr = S_OK;

	if (SUCCEEDED(hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&pDeviceEnumerator))))  // need device enumerator
	{
		if (SUCCEEDED(hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice)))  // get default audio device from enumerator
		{
			if (SUCCEEDED(hr = pDevice->Activate(__uuidof(IAudioSessionManager2), 0, NULL, reinterpret_cast<void**>(&pAudioManager))))  // activate audio manager of default audio device
			{
				*pSessionManager = pAudioManager;
				SafeRelease(&pDeviceEnumerator);
				SafeRelease(&pDevice);

				return hr;
			}
		}
	}

	SafeRelease(&pDeviceEnumerator);
	SafeRelease(&pDevice);
	SafeRelease(&pAudioManager);
	return hr;
}

// takes a pointer to IAudioSessionManager2 object and a process ID, loops through the audio sessions of IAudioSessionManager2
// and compares the process ID to sessID arg and writes a pointer to an associated ISimpleAudioVolume object to pSimpleAudio Volume if they match
// return value can be checked with SUCCEEDED() or FAILED() macro
HRESULT EnumAudioSessions(IAudioSessionManager2* pSessionManager, ISimpleAudioVolume** pSimpleAudioVolume, DWORD sessID)
{
	if (!pSessionManager)
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	int cbSessionCount = 0;
	LPWSTR pswSession = NULL;
	DWORD sessionID;

	IAudioSessionEnumerator* pSessionList = NULL;
	IAudioSessionControl* pSessionControl = NULL;
	IAudioSessionControl2* pSessionControl2 = NULL;
	ISimpleAudioVolume* pSimpleAudioVol = NULL;

	if (SUCCEEDED(hr = pSessionManager->GetSessionEnumerator(&pSessionList)))
	{
		if (SUCCEEDED(hr = pSessionList->GetCount(&cbSessionCount)))
		{
			for (int i = 0; i < cbSessionCount; ++i)
			{
				CoTaskMemFree(pswSession);
				SafeRelease(&pSessionControl);
				SafeRelease(&pSessionControl2);

				if (SUCCEEDED(hr = pSessionList->GetSession(i, &pSessionControl)))
				{
					if (SUCCEEDED(hr = pSessionControl->GetDisplayName(&pswSession)))
					{
						pSessionControl->QueryInterface(&pSessionControl2);
						pSessionControl2->GetProcessId(&sessionID);

						if (sessionID == sessID)
						{
							pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleAudioVol);
							*pSimpleAudioVolume = pSimpleAudioVol;
						}
					}
				}

				if (FAILED(hr))
				{
					SafeRelease(&pSessionControl);
					SafeRelease(&pSessionControl2);
					SafeRelease(&pSessionList);
					return hr;
				}
			}
		}
	}

	CoTaskMemFree(pswSession);
	SafeRelease(&pSessionControl);
	SafeRelease(&pSessionControl2);
	SafeRelease(&pSessionList);
	return hr;
}

HRESULT ChangeMuteStatus(std::vector<DWORD> processIDs, bool bShouldMute)
{
	HRESULT hr = S_OK;
	IAudioSessionManager2* pSessionManager = NULL;   // pointer to IAudioSessionManager2 COM interface
	ISimpleAudioVolume* pAudioVolume = NULL;         // pointer to ISimpleAudioVolume COM interface

	for (int i = 0; i < processIDs.size(); i++)		 // loop through all processes associated with Spotify, get their audio manager COM object and their mute audio
	{
		SafeRelease(&pSessionManager);
		SafeRelease(&pAudioVolume);

		if (SUCCEEDED(hr = GetAudioSessionManager(&pSessionManager)))
		{
			if (SUCCEEDED(hr = EnumAudioSessions(pSessionManager, &pAudioVolume, processIDs[i])))
			{
				if (pAudioVolume)
					pAudioVolume->SetMute(bShouldMute, &GUID_NULL);
			}
		}

		if (FAILED(hr))
		{
			SafeRelease(&pSessionManager);
			SafeRelease(&pAudioVolume);
			return hr;
		}
	}

	SafeRelease(&pSessionManager);
	SafeRelease(&pAudioVolume);
	return hr;
}