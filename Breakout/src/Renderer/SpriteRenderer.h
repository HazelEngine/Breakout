#pragma once

#include <glm/glm.hpp>

#include <Hazel.h>
using namespace Hazel;

class SpriteRenderer
{
public:
	// Ctor
	SpriteRenderer();

	// Renders a defined quad textured with given sprite
	void DrawSprite(
		const Texture2D const* texture,
		glm::vec2 position,
		glm::vec2 size = glm::vec2(10, 10),
		float rotation = 0.0f,
		glm::vec3 color = glm::vec3(1.0f)
	);

private:
	// Render state
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexArray> m_VertexArray;
};