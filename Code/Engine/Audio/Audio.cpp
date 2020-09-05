#include "Engine/Audio/Audio.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



AudioSystem* g_AudioSystem = nullptr;



AudioSystem::AudioSystem() : m_FMODSystem(nullptr)
{
	InitializeFMOD();
}



AudioSystem::~AudioSystem()
{
	for (auto currentSoundID = m_RegisteredSoundIDs.begin(); currentSoundID != m_RegisteredSoundIDs.end();)
	{
		currentSoundID = m_RegisteredSoundIDs.erase(currentSoundID);
	}

	for (FMOD::Sound* currentSound : m_RegisteredSounds)
	{
		FMOD_RESULT result = currentSound->release();
		ValidateResult(result);
	}
}



void AudioSystem::InitializeAudioSystem()
{
	if (g_AudioSystem == nullptr)
	{
		g_AudioSystem = new AudioSystem();
	}
}



void AudioSystem::UninitializeAudioSystem()
{
	if (g_AudioSystem != nullptr)
	{
		delete g_AudioSystem;
		g_AudioSystem = nullptr;
	}
}



AudioSystem* AudioSystem::SingletonInstance()
{
	ASSERT_OR_DIE(g_AudioSystem != nullptr, "Audio System instance does not exist.");

	return g_AudioSystem;
}



SoundID AudioSystem::CreateOrGetSound(const char* soundFileName)
{
	auto foundSoundID = m_RegisteredSoundIDs.find(soundFileName);
	if (foundSoundID != m_RegisteredSoundIDs.end())
	{
		return foundSoundID->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_FMODSystem->createSound(soundFileName, FMOD_DEFAULT, nullptr, &newSound);
		if (newSound)
		{
			SoundID newSoundID = m_RegisteredSounds.size();
			m_RegisteredSoundIDs[soundFileName] = newSoundID;
			m_RegisteredSounds.push_back(newSound);

			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}



AudioChannelHandle AudioSystem::PlaySound(SoundID playableSoundID, PlaybackMode soundPlaybackMode, float volumeLevel /*= 1.0f*/, float panLevel /*= 0.0f*/)
{
	unsigned int numberOfSounds = m_RegisteredSounds.size();
	if (playableSoundID < 0 || playableSoundID >= numberOfSounds)
	{
		return nullptr;
	}

	FMOD::Sound* currentSound = m_RegisteredSounds[playableSoundID];
	if (!currentSound)
	{
		return nullptr;
	}

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_FMODSystem->playSound(FMOD_CHANNEL_FREE, currentSound, false, &channelAssignedToSound);
	if (channelAssignedToSound)
	{
		channelAssignedToSound->setVolume(volumeLevel);
		channelAssignedToSound->setPan(panLevel);
		if (soundPlaybackMode == LOOPING_PLAYBACK_MODE)
		{
			channelAssignedToSound->setMode(FMOD_LOOP_NORMAL);
		}
	}

	return (AudioChannelHandle)channelAssignedToSound;
}



void AudioSystem::PauseSound(AudioChannelHandle audioChannel, bool paused)
{
	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;
		FMODAudioChannel->setPaused(paused);
	}
}



void AudioSystem::StopSound(AudioChannelHandle audioChannel)
{
	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;
		FMODAudioChannel->stop();
	}
}



bool AudioSystem::IsSoundPlaying(AudioChannelHandle audioChannel)
{
	bool soundIsPlaying = false;

	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;
		FMODAudioChannel->isPlaying(&soundIsPlaying);
	}

	return soundIsPlaying;
}



bool AudioSystem::IsSoundPaused(AudioChannelHandle audioChannel)
{
	bool soundIsPaused = false;

	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;
		FMODAudioChannel->getPaused(&soundIsPaused);
	}

	return soundIsPaused;
}



void AudioSystem::IncreaseVolume(AudioChannelHandle audioChannel)
{
	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;
		
		float volumeLevel;
		FMODAudioChannel->getVolume(&volumeLevel);
		volumeLevel += 0.05f;
		FMODAudioChannel->setVolume(volumeLevel);
	}
}



void AudioSystem::DecreaseVolume(AudioChannelHandle audioChannel)
{
	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;

		float volumeLevel;
		FMODAudioChannel->getVolume(&volumeLevel);
		volumeLevel -= 0.05f;
		FMODAudioChannel->setVolume(volumeLevel);
	}
}



float AudioSystem::GetVolumeLevel(AudioChannelHandle audioChannel) const
{
	float volumeLevel = 0.0f;

	if (audioChannel != nullptr)
	{
		FMOD::Channel* FMODAudioChannel = (FMOD::Channel*)audioChannel;
		FMODAudioChannel->getVolume(&volumeLevel);
	}

	return volumeLevel;
}



void AudioSystem::Update()
{
	FMOD_RESULT result = m_FMODSystem->update();
	ValidateResult(result);
}



void AudioSystem::InitializeFMOD()
{
	const int MAX_AUDIO_DEVICE_NAME_LENGTH = 256;
	FMOD_RESULT result;
	unsigned int FMODVersion;
	int numberOfDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS deviceCapabilities;
	char audioDeviceName[MAX_AUDIO_DEVICE_NAME_LENGTH];

	result = FMOD::System_Create(&m_FMODSystem);
	ValidateResult(result);

	result = m_FMODSystem->getVersion(&FMODVersion);
	ValidateResult(result);

	if (FMODVersion < FMOD_VERSION)
	{
		
	}

	result = m_FMODSystem->getNumDrivers(&numberOfDrivers);
	ValidateResult(result);

	if (numberOfDrivers == 0)
	{
		result = m_FMODSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ValidateResult(result);
	}
	else
	{
		result = m_FMODSystem->getDriverCaps(0, &deviceCapabilities, 0, &speakerMode);
		ValidateResult(result);

		result = m_FMODSystem->setSpeakerMode(speakerMode);
		ValidateResult(result);

		if (deviceCapabilities & FMOD_CAPS_HARDWARE_EMULATED)
		{
			result = m_FMODSystem->setDSPBufferSize(1024, 10);
			ValidateResult(result);
		}

		result = m_FMODSystem->getDriverInfo(0, audioDeviceName, MAX_AUDIO_DEVICE_NAME_LENGTH, 0);
		ValidateResult(result);

		if (strstr(audioDeviceName, "SigmaTel"))
		{
			result = m_FMODSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			ValidateResult(result);
		}
	}

	result = m_FMODSystem->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		result = m_FMODSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ValidateResult(result);

		result = m_FMODSystem->init(100, FMOD_INIT_NORMAL, 0);
		ValidateResult(result);
	}
}



void AudioSystem::ValidateResult(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		__debugbreak();
	}
}