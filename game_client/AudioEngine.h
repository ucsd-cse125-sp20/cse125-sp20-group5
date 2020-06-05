#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "Core.h"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

struct Implementation {
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;
	typedef map<string, FMOD::Studio::EventInstance*> EventMap;
	typedef map<string, FMOD::Studio::Bank*> BankMap;

	BankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};

class CAudioEngine {
public:
	static void Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result); 

	void LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const string& strEventName);
	void LoadSound(const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnLoadSound(const string& strSoundName);
	void Set3dListenerAndOrientation(const glm::vec3&vPosition, const glm::vec3&vLook, const glm::vec3& vUp);
	
	void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumedB);

	int PlaySounds(const string& strSoundName, const glm::vec3&vPosition = glm::vec3(0), float fVolumedB = 0.0f);
	void StopSounds(const string& strSoundName); // added by me
	bool IsPlaying(const string& strSoundName); // added by me
	void PlayEvent(const string &strEventName);
	void StopChannel(int nChannelId);
	void StopEvent(const string &strEventName, bool bImmediate = false);
	//void GetEventParameter(const string &strEventName, const string &strEventParameter, float* parameter);
	//void SetEventParameter(const string &strEventName, const string &strParameterName, float fValue);
	void StopAllChannels();
	bool IsPlaying(int nChannelId) const;
	bool IsEventPlaying(const string &strEventName) const;

	FMOD_VECTOR VectorToFmod(const glm::vec3& vPosition);
	float dbToVolume(float db);
	float VolumeTodB(float volume);
	


	FMOD::System *m_pSystem;
	map<string, int> fileToChannelMap;

	CAudioEngine();

};

#endif