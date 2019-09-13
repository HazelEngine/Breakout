#pragma once

#include <Hazel.h>
using namespace Hazel;

#include "Renderer/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
	// Ctors
	GameObject();
	GameObject(
		glm::vec2 pos,
		glm::vec2 size,
		const std::shared_ptr<Texture2D>& sprite,
		glm::vec3 color = glm::vec3(1.0f),
		glm::vec2 velocity = glm::vec2(0.0f)
	);

	// Draw sprite
	virtual void Draw(SpriteRenderer* const renderer);

public:
	// Object state
	glm::vec2 m_Position, m_Size, m_Velocity;
	glm::vec3 m_Color;
	float     m_Rotation;
	bool      m_IsSolid;
	bool      m_Destroyed;

	// Render state
	std::shared_ptr<Texture2D> m_Sprite;
};