#include "SpriteRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer()
{
	// Shader
	m_Shader.reset(Shader::Load("Assets/Shaders/SpriteVS.cso", "Assets/Shaders/SpritePS.cso"));

	// Vertices
	std::array<float, 16> vertices =
	{
		// Positions  // TexCoords
		0.0f, 1.0f,   0.0f, 1.0f,
		1.0f, 0.0f,   1.0f, 0.0f,
		0.0f, 0.0f,   0.0f, 0.0f,
		1.0f, 1.0f,   1.0f, 1.0f
	};

	// Indices
	std::array<uint32_t, 6> indices = { 0, 1, 2, 0, 3, 1 };

	// VertexArray
	m_VertexArray.reset(VertexArray::Create());

	// VertexBuffer
	auto vBuffer = std::shared_ptr<VertexBuffer>();
	vBuffer.reset(VertexBuffer::Create(vertices.data(), vertices.size()));
	vBuffer->SetLayout({
		{ ShaderDataType::Float4, "Vertex" }
	});
	m_VertexArray->AddVertexBuffer(vBuffer);

	// IndexBuffer
	auto iBuffer = std::shared_ptr<IndexBuffer>();
	iBuffer.reset(IndexBuffer::Create(indices.data(), indices.size()));
	m_VertexArray->SetIndexBuffer(iBuffer);
}

void SpriteRenderer::DrawSprite(
	const Texture2D const* texture,
	glm::vec2 position,
	glm::vec2 size,
	float rotation,
	glm::vec3 color
)
{
	m_Shader->Bind();

	// Prepare transformations
	glm::mat4 model = glm::mat4(1.0f);

	// First translate (transformations are: scale happens first, then
	// rotation and then final translation happens; reversed order)
	model = glm::translate(model, glm::vec3(position, 0.0f));

	// Move origin of rotation to center of quad
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));

	// Then rotate
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

	// Move origin back
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	// Last scale
	model = glm::scale(model, glm::vec3(size, 1.0f));
	
	m_Shader->GetParam("Model")->SetAsMat4(model);
	m_Shader->GetParam("SpriteColor")->SetAsFloat3(color.x, color.y, color.z);

	// Render textured quad
	texture->Bind(TextureSlot0);

	Renderer::Submit(m_Shader, m_VertexArray);
}