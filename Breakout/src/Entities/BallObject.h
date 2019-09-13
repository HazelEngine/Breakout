#pragma once

#include "GameObject.h"

// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class BallObject : public GameObject
{
public:
	// Ctors
	BallObject();
	BallObject(
		glm::vec2 pos,
		float radius,
		glm::vec2 velocity,
		const std::shared_ptr<Texture2D>& sprite
	);

	// Moves the ball, keeping it constrained within the window
	// bounds (except bottom edge). Returns new position.
	glm::vec2 Move(float dt, uint32_t windowWidth);

	// Resets the ball to original state with given position an
	// velocity
	void Reset(glm::vec2 pos, glm::vec2 velocity);

public:
	// Ball state
	float m_Radius;
	bool  m_Stuck;
	bool  Sticky, PassThrough;
};