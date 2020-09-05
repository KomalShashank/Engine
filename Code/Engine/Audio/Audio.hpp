#pragma once

#include <map>
#include <vector>
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"

#pragma comment(lib, "ThirdParty/fmod/fmodex_vc")



typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;



enum PlaybackMode
{
	INVALID_PLAYBACK_MODE = -1,
	FORWARD_PLAYBACK_MODE,
	LOOPING_PLAYBACK_MODE,
	NUMBER_OF_PLAYBACK_MODES
};



class AudioSystem
{
public:
	AudioSystem();
	~AudioSystem();

	static void InitializeAudioSystem();
	static void UninitializeAudioSystem();

	static AudioSystem* SingletonInstance();

	SoundID CreateOrGetSound(const char* soundFileName);
	AudioChannelHandle PlaySound(SoundID playableSoundID, PlaybackMode soundPlaybackMode, float volumeLevel = 1.0f, float panLevel = 0.0f);
	
	void PauseSound(AudioChannelHandle audioChannel, bool paused);
	void StopSound(AudioChannelHandle audioChannel);
	bool IsSoundPlaying(AudioChannelHandle audioChannel);
	bool IsSoundPaused(AudioChannelHandle audioChannel);
	
	void IncreaseVolume(AudioChannelHandle audioChannel);
	void DecreaseVolume(AudioChannelHandle audioChannel);

	float GetVolumeLevel(AudioChannelHandle audioChannel) const;
	
	void Update();

protected:
	void InitializeFMOD();
	void ValidateResult(FMOD_RESULT result);

protected:
	FMOD::System* m_FMODSystem;
	std::map<const char*, SoundID, CompareCStrings> m_RegisteredSoundIDs;
	std::vector<FMOD::Sound*> m_RegisteredSounds;
};