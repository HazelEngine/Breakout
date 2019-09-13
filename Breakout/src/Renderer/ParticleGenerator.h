#pragma once

#include <Hazel.h>
using namespace Hazel;

#include "Entities/GameObject.h"

// Represents a single particle and its state
struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float     Scale;
	float     Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Scale(15.0f), Life(0.0f) {}
};

// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
public:
	// Ctor
	ParticleGenerator(
		const std::shared_ptr<Texture2D>& texture,
		uint32_t amount
	);

	// Update all particles
	void Update(
		float dt,
		const GameObject* const object,
		uint32_t newParticles,
		glm::vec2 offset = glm::vec2(0.0f)
	);

	// Render all particles
	void Draw();

private:
	// Returns the first particle index that's currently unused
	// e.g. Life <= 0.0f or 0 if no particle is currently inactive
	uint32_t FirstUnusedParticle();

	// Respawns particle
	void RespawnParticle(
		Particle& particle,
		const GameObject* const object,
		glm::vec2 offset
	);

private:
	// State
	std::vector<Particle> m_Particles;
	uint32_t m_Amount;

	// Render state
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Texture2D> m_Texture;
};