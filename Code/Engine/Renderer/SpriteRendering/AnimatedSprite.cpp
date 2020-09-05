#include "Engine/Renderer/SpriteRendering/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteDatabase.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteRenderer.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



AnimationSequence::AnimationSequence() :
m_ID("DefaultNullAnimationSequence"),
m_AnimationMode(INVALID_ANIMATION_MODE),
m_Duration(0.0f)
{

}



void AnimationSequence::AddFrame(const char* spriteResourceID, float frameDuration)
{
	AnimationFrame newFrame;
	newFrame.m_SpriteResource = SpriteDatabase::SingletonInstance()->GetConstantSpriteResource(spriteResourceID);
	newFrame.m_StartTime = m_Duration;

	m_Frames.push_back(newFrame);

	m_Duration += frameDuration;
}



const SpriteResource* AnimationSequence::GetAnimationFrame(float currentTime) const
{
	const SpriteResource* spriteResource = m_Frames[0].m_SpriteResource;
	for (size_t frameIndex = 0; frameIndex < m_Frames.size(); ++frameIndex)
	{
		if (currentTime < m_Frames[frameIndex].m_StartTime)
		{
			spriteResource = m_Frames[frameIndex - 1].m_SpriteResource;
			break;
		}

		if (frameIndex == m_Frames.size() - 1)
		{
			if (currentTime >= m_Frames[frameIndex].m_StartTime)
			{
				spriteResource = m_Frames[frameIndex].m_SpriteResource;
				break;
			}
		}
	}

	return spriteResource;
}



void AnimationSequence::UpdateCurrentTime(float& currentTime, float deltaTimeInSeconds) const
{
	switch (m_AnimationMode)
	{
	case SINGLE_PLAY_MODE:
	{
		currentTime += deltaTimeInSeconds;
		currentTime = ClampFloat(currentTime, 0.0f, m_Duration);
	}
		break;

	case LOOPING_MODE:
	{
		currentTime += deltaTimeInSeconds;
		currentTime = WrapAroundCircularRange(currentTime, 0.0f, m_Duration);
	}
		break;

	case PINGPONG_MODE:
	{
		currentTime += deltaTimeInSeconds;
		currentTime = OscillateWithinRange(currentTime, 0.0f, m_Duration);
	}
		break;

	default:
		break;
	}
}



AnimatedSprite::AnimatedSprite(const char* animationSequenceID) :
Sprite(),
m_CurrentTime(0.0f)
{
	m_Sequence = SpriteDatabase::SingletonInstance()->GetConstantAnimationSequence(animationSequenceID);
	SetCurrentFrame(0.0f);
}



void AnimatedSprite::Update(float deltaTimeInSeconds)
{
	m_Sequence->UpdateCurrentTime(m_CurrentTime, deltaTimeInSeconds);
	SetCurrentFrame(m_CurrentTime);
}



void AnimatedSprite::EnableSprite()
{
	if (!m_IsEnabled)
	{
		m_IsEnabled = true;
		SpriteRenderer::SingletonInstance()->RegisterSprite(this);
		SpriteRenderer::SingletonInstance()->RegisterAnimatedSprite(this);
	}
}



void AnimatedSprite::DisableSprite()
{
	if (m_IsEnabled)
	{
		m_IsEnabled = false;
		SpriteRenderer::SingletonInstance()->UnregisterSprite(this);
		SpriteRenderer::SingletonInstance()->UnregisterAnimatedSprite(this);
	}
}



void AnimatedSprite::SetCurrentFrame(float currentTime)
{
	m_SpriteResource = m_Sequence->GetAnimationFrame(currentTime);
	m_Material = m_SpriteResource->m_Material;
	m_Material->SetDiffuseTexture(m_SpriteResource->m_Texture);
}