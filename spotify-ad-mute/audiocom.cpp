#include <Windows.h>
#include <Psapi.h>
#include <mmdeviceapi.h>
#include "AudioCom.hpp"
#include "Utility.hpp"

// Initializes COM library 
// Return value can be checked with SUCCEEDED() or FAILED() macro
HRESULT initialize_com()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	return hr;
}

// Writes an object of IAudioSessionManager2* type to pSessionManager arg
// Return value can be checked with SUCCEEDED() or FAILED() macro
HRESULT audio_session_manager(IAudioSessionManager2** pSessionManager)
{
	// Get GUIDs of COM class and interface
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	IAudioSessionManager2* pAudioManager = NULL;
	IMMDeviceEnumerator* pDeviceEnumerator = NULL;
	IMMDevice* pDevice = NULL;

	HRESULT hr = S_OK;

	if (SUCCEEDED(hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&pDeviceEnumerator))))  // Need device enumerator
	{
		if (SUCCEEDED(hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice)))  // Get default audio device from enumerator
		{
			if (SUCCEEDED(hr = pDevice->Activate(__uuidof(IAudioSessionManager2), 0, NULL, reinterpret_cast<void**>(&pAudioManager))))  // Activate audio manager of default audio device
			{
				*pSessionManager = pAudioManager;
				safe_release(&pDeviceEnumerator);
				safe_release(&pDevice);

				return hr;
			}
		}
	}

	safe_release(&pDeviceEnumerator);
	safe_release(&pDevice);
	safe_release(&pAudioManager);
	return hr;
}

// Takes a pointer to IAudioSessionManager2 object and a process ID, loops through the audio sessions of IAudioSessionManager2
// and compares the process ID to sessID arg and writes a pointer to an associated ISimpleAudioVolume object to pSimpleAudio Volume if they match
// Return value can be checked with SUCCEEDED() or FAILED() macro
HRESULT enum_audio_sessions(ISimpleAudioVolume** pAudioVolume)
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

	if (SUCCEEDED(hr = audio_session_manager(&pSessionManager)))
	{
		if (SUCCEEDED(hr = pSessionManager->GetSessionEnumerator(&pSessionList)))
		{
			if (SUCCEEDED(hr = pSessionList->GetCount(&cbSessionCount)))
			{
				for (int i = 0; i < cbSessionCount; ++i)
				{
					CoTaskMemFree(pswSession);
					safe_release(&pSessionControl);
					safe_release(&pSessionControl2);

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
									error_exit("Failed to retrieve module name");
								}

								std::wstring name(processName);
								if (name == L"Spotify.exe")
								{
									pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleAudioVol);
									*pAudioVolume = pSimpleAudioVol;
						
									CloseHandle(handle);
									CoTaskMemFree(pswSession);
									safe_release(&pSessionManager);
									safe_release(&pSessionControl);
									safe_release(&pSessionControl2);
									safe_release(&pSessionList);
									return hr;
								}
							}
						}
					}

					if (FAILED(hr))
					{
						
						safe_release(&pSimpleAudioVol);
						safe_release(&pSessionManager);
						safe_release(&pSessionControl);
						safe_release(&pSessionControl2);
						safe_release(&pSessionList);
						return hr;
					}
				}
			}
		}
	}

	CoTaskMemFree(pswSession);
	safe_release(&pSimpleAudioVol);
	safe_release(&pSessionManager);
	safe_release(&pSessionControl);
	safe_release(&pSessionControl2);
	safe_release(&pSessionList);
	return hr;
}

HRESULT change_mute_status(ISimpleAudioVolume* pAudioVolume, bool shouldMute)
{	
	if (pAudioVolume)
		pAudioVolume->SetMute(shouldMute, &GUID_NULL);
	else
		return E_FAIL;
	
	return S_OK;
}

