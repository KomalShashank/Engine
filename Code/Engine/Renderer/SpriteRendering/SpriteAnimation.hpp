#pragma once

#include "Engine/Renderer/SpriteRendering/SpriteSheet.hpp"



enum SpriteAnimationMode
{
	INVALID_ANIMATION_MODE = -1,
	SINGLE_PLAY_MODE,
	LOOPING_MODE,
	PINGPONG_MODE,
	NUMBER_OF_ANIMATION_MODES
};



class SpriteAnimation
{
public:
	SpriteAnimation(SpriteSheet* animatableSpriteSheet, float durationInSeconds,
		SpriteAnimationMode animationMode, int startingSpriteIndex, int endingSpriteIndex);

	void Update(float deltaTimeInSeconds);

	AABB2 GetCurrentFrameTextureCoordinates() const;
	Texture* GetTexture() const;

	void Pause();
	void Resume();
	void Reset();

	bool IsFinished() const;
	bool IsPlaying() const;

	float GetDurationInSeconds() const;
	float GetElapsedTimeInSeconds() const;
	float GetRemainingTimeInSeconds() const;
	float GetFractionOfElapsedTime() const;
	float GetFractionOfRemainingTime() const;

	void SetElapsedTimeInSeconds(float elapsedTimeInSeconds);
	void SetFractionOfElapsedTime(float fractionOfElapsedTime);

public:
	SpriteSheet* m_SpriteSheet;
	SpriteAnimationMode m_AnimationMode;
	
	float m_DurationInSeconds;
	int m_StartingSpriteIndex;
	int m_EndingSpriteIndex;

	bool m_IsFinished;
	bool m_IsPlaying;

	float m_ElapsedTimeInSeconds;
	float m_TotalNumberOfFrames;
};