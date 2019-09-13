#include "BallObject.h"

BallObject::BallObject()
	: GameObject(), m_Radius(12.5f), m_Stuck(true), Sticky(false), PassThrough(false) {}

BallObject::BallObject(
	glm::vec2 pos,
	float radius,
	glm::vec2 velocity,
	const std::shared_ptr<Texture2D>& sprite
) : GameObject(
		pos,
		glm::vec2(radius * 2, radius * 2),
		sprite,
		glm::vec3(1.0f),
		velocity
	), m_Radius(radius), m_Stuck(true), Sticky(false), PassThrough(false) {}

glm::vec2 BallObject::Move(float dt, uint32_t windowWidth)
{
	// If not stuck to player paddle
	if (!m_Stuck)
	{
		// Rotate the ball
		m_Rotation += 40.0f * dt;

		// Move the ball
		m_Position += m_Velocity * dt;

		// Then check if outside window bounds and if so,
		// reverse velocity and restore at correct position
		if (m_Position.x <= 0.0f)
		{
			m_Velocity.x = -m_Velocity.x;
			m_Position.x = 0.0f;
		}
		else if (m_Position.x + m_Size.x >= windowWidth)
		{
			m_Velocity.x = -m_Velocity.x;
			m_Position.x = windowWidth - m_Size.x;
		}
		if (m_Position.y <= 0.0f)
		{
			m_Velocity.y = -m_Velocity.y;
			m_Position.y = 0.0f;
		}
	}

	return m_Position;
}

void BallObject::Reset(glm::vec2 pos, glm::vec2 velocity)
{
	m_Position = pos;
	m_Rotation = 0.0f;
	m_Velocity = velocity;
	m_Stuck = true;
	Sticky = false;
	PassThrough = false;
}
