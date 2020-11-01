#include <Windows.h>
#include <Psapi.h>
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
HRESULT EnumAudioSessions(ISimpleAudioVolume** pAudioVolume, DWORD* processID)
{
	HRESULT hr = S_OK;

	int cbSessionCount = 0;
	LPWSTR pswSession = NULL;
	DWORD sessionID;

	IAudioSessionManager2* pSessionManager = NULL;
	IAudioSessionEnumerator* pSessionList = NULL;
	IAudioSessionControl* pSessionControl = NULL;
	IAudioSessionControl2* pSessionControl2 = NULL;
	ISimpleAudioVolume* pSimpleAudioVol = NULL;

	if (SUCCEEDED(hr = GetAudioSessionManager(&pSessionManager)))
	{
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

							HANDLE handle = OpenProcess(
								PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
								FALSE,
								sessionID
							);

							if (handle)
							{
								wchar_t processName[50];
								if (!GetModuleBaseName(handle, 0, processName, 50))
								{
									ErrorExit("Failed to retrieve module name");
								}

								std::wstring name(processName);
								if (name == L"Spotify.exe")
								{
									pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleAudioVol);
									*pAudioVolume = pSimpleAudioVol;
						
									*processID = sessionID;
								}
							}
						}
					}

					if (FAILED(hr))
					{
						SafeRelease(&pSessionManager);
						SafeRelease(&pSessionControl);
						SafeRelease(&pSessionControl2);
						SafeRelease(&pSessionList);
						return hr;
					}
				}
			}
		}
	}

	CoTaskMemFree(pswSession);
	SafeRelease(&pSessionManager);
	SafeRelease(&pSessionControl);
	SafeRelease(&pSessionControl2);
	SafeRelease(&pSessionList);
	return hr;
}

HRESULT ChangeMuteStatus(ISimpleAudioVolume* pAudioVolume, bool shouldMute)
{	
	if (pAudioVolume)
		pAudioVolume->SetMute(shouldMute, &GUID_NULL);
	else
		return E_FAIL;
	
	return S_OK;
}

