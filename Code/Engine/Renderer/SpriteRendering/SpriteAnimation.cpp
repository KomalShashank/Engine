#include "Engine/Renderer/SpriteRendering/SpriteAnimation.hpp"



SpriteAnimation::SpriteAnimation(SpriteSheet* animatableSpriteSheet, float durationInSeconds,
	SpriteAnimationMode animationMode, int startingSpriteIndex, int endingSpriteIndex) :
m_SpriteSheet(animatableSpriteSheet),
m_DurationInSeconds(durationInSeconds),
m_AnimationMode(animationMode),
m_StartingSpriteIndex(startingSpriteIndex),
m_EndingSpriteIndex(endingSpriteIndex),
m_IsFinished(false),
m_IsPlaying(true),
m_ElapsedTimeInSeconds(0.0f)
{
	m_TotalNumberOfFrames = static_cast<float>(endingSpriteIndex - startingSpriteIndex + 1);
}



void SpriteAnimation::Update(float deltaTimeInSeconds)
{
	if (m_IsPlaying)
	{
		m_ElapsedTimeInSeconds += deltaTimeInSeconds;
		if (m_ElapsedTimeInSeconds >= m_DurationInSeconds)
		{
			if (m_AnimationMode == LOOPING_MODE)
			{
				Reset();
			}
			else if (m_AnimationMode == SINGLE_PLAY_MODE)
			{
				Pause();
				m_IsFinished = true;
			}
		}
	}
}



AABB2 SpriteAnimation::GetCurrentFrameTextureCoordinates() const
{
	float fractionOfFrameFromStart = m_TotalNumberOfFrames * GetFractionOfElapsedTime();
	int currentFrameNumber = m_StartingSpriteIndex + static_cast<int>(floor(fractionOfFrameFromStart));
	AABB2 currentFrameTextureCoordinates = m_SpriteSheet->GetTextureCoordsForSpriteIndex(currentFrameNumber);

	return currentFrameTextureCoordinates;
}



Texture* SpriteAnimation::GetTexture() const
{
	return m_SpriteSheet->GetSpriteSheet();
}



void SpriteAnimation::Pause()
{
	m_IsPlaying = false;
}



void SpriteAnimation::Resume()
{
	m_IsPlaying = true;
}



void SpriteAnimation::Reset()
{
	m_ElapsedTimeInSeconds = 0.0f;
	m_IsPlaying = true;
}



bool SpriteAnimation::IsFinished() const
{
	return m_IsFinished;
}



bool SpriteAnimation::IsPlaying() const
{
	return m_IsPlaying;
}



float SpriteAnimation::GetDurationInSeconds() const
{
	return m_DurationInSeconds;
}



float SpriteAnimation::GetElapsedTimeInSeconds() const
{
	return m_ElapsedTimeInSeconds;
}



float SpriteAnimation::GetRemainingTimeInSeconds() const
{
	return m_DurationInSeconds - m_ElapsedTimeInSeconds;
}



float SpriteAnimation::GetFractionOfElapsedTime() const
{
	return m_ElapsedTimeInSeconds / m_DurationInSeconds;
}



float SpriteAnimation::GetFractionOfRemainingTime() const
{
	return (m_DurationInSeconds - m_ElapsedTimeInSeconds) / m_DurationInSeconds;
}



void SpriteAnimation::SetElapsedTimeInSeconds(float elapsedTimeInSeconds)
{
	m_ElapsedTimeInSeconds = elapsedTimeInSeconds;
}



void SpriteAnimation::SetFractionOfElapsedTime(float fractionOfElapsedTime)
{
	m_ElapsedTimeInSeconds = fractionOfElapsedTime * m_DurationInSeconds;
}