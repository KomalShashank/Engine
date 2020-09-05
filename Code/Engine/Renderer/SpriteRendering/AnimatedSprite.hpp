#pragma once

#include "Engine/Renderer/SpriteRendering/Sprite.hpp"



enum SpriteAnimationMode
{
	INVALID_ANIMATION_MODE = -1,
	SINGLE_PLAY_MODE,
	LOOPING_MODE,
	PINGPONG_MODE,
	NUMBER_OF_ANIMATION_MODES
};



struct AnimationFrame
{
	const SpriteResource* m_SpriteResource;
	float m_StartTime;
};



class AnimationSequence
{
public:
	AnimationSequence();
	AnimationSequence(const AnimationSequence&) = delete;

	void AddFrame(const char* spriteResourceID, float frameDuration);
	const SpriteResource* GetAnimationFrame(float currentTime) const;

	void UpdateCurrentTime(float& currentTime, float deltaTimeInSeconds) const;

public:
	const char* m_ID;

	std::vector<AnimationFrame> m_Frames;
	SpriteAnimationMode m_AnimationMode;
	float m_Duration;
};



class AnimatedSprite : public Sprite
{
public:
	AnimatedSprite(const char* animationSequenceID);

	void Update(float deltaTimeInSeconds);

	void EnableSprite() override;
	void DisableSprite() override;

private:
	void SetCurrentFrame(float currentTime);

private:
	const AnimationSequence* m_Sequence;
	float m_CurrentTime;
};