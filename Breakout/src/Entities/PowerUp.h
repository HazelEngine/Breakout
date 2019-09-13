#pragma once

#include "Config.h"
#include "GameObject.h"

class PowerUp : public GameObject
{
public:
	// Ctor
	PowerUp(
		const std::string& type,
		float duration,
		glm::vec2 position,
		glm::vec3 color,
		const std::shared_ptr<Texture2D>& texture
	) : GameObject(position, POWERUP_SIZE, texture, color, POWERUP_VELOCITY),
		Type(type),
		Duration(duration),
		Activated() {}

public:
	// State
	std::string Type;
	float       Duration;
	bool        Activated;
};