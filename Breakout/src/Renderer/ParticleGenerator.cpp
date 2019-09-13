#include "ParticleGenerator.h"

#include <Platform/OpenGL/OpenGLHelper.h>
#include <glm/gtc/matrix_transform.hpp>

ParticleGenerator::ParticleGenerator(
	const std::shared_ptr<Texture2D>& texture,
	uint32_t amount
) : m_Texture(texture), m_Amount(amount)
{
	// Shader
	m_Shader.reset(Shader::Load("Assets/Shaders/ParticleVS.cso", "Assets/Shaders/ParticlePS.cso"));

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

	// Create m_Amount default particle instances
	for (uint32_t i = 0; i < m_Amount; ++i)
		m_Particles.push_back(Particle());
}

void ParticleGenerator::Update(
	float dt,
	const GameObject* const object,
	uint32_t newParticles,
	glm::vec2 offset
)
{
	// Add new particles
	for (uint32_t i = 0; i < newParticles; ++i)
	{
		int unused = FirstUnusedParticle();
		RespawnParticle(m_Particles[unused], object, offset);
	}

	// Update all particles
	for (uint32_t i = 0; i < m_Amount; ++i)
	{
		auto& particle = m_Particles[i];
		particle.Life -= dt;    // Reduce life

		if (particle.Life > 0.0f)    // Particle is alive, thus update
		{
			particle.Position -= particle.Velocity * dt;
			particle.Color.a -= 2.5f * dt;
			particle.Scale -= 9.0f * dt;
		}
	}
}

void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	OpenGLHelper::EnableAdditiveBlending();

	for (auto& particle : m_Particles)
	{
		if (particle.Life > 0.0f)
		{
			m_Shader->Bind();

			m_Shader->GetParam("Offset")->SetAsFloat2(
				particle.Position.x,
				particle.Position.y
			);

			m_Shader->GetParam("Scale")->SetAsFloat(
				particle.Scale
			);

			m_Shader->GetParam("Color")->SetAsFloat4(
				particle.Color.r,
				particle.Color.g,
				particle.Color.b,
				particle.Color.a
			);

			m_Texture->Bind(TextureSlot0);
			Renderer::Submit(m_Shader, m_VertexArray);
		}
	}

	OpenGLHelper::DisableAdditiveBlending();
}

// Store the index of the last particle used (for quick access to next dead particle)
uint32_t lastUsedParticle = 0;
uint32_t ParticleGenerator::FirstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (uint32_t i = lastUsedParticle; i < m_Amount; ++i)
	{
		if (m_Particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// Otherwise, do a linear search
	for (uint32_t i = 0; i < lastUsedParticle; ++i)
	{
		if (m_Particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// All particles are taken, override the first one (note that if
	// it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return lastUsedParticle;
}

void ParticleGenerator::RespawnParticle(
	Particle& particle,
	const GameObject* const object,
	glm::vec2 offset
)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float color = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = object->m_Position + random + offset;
	particle.Color = glm::vec4(color, color, color, 1.0f);
	particle.Scale = 15.0f;
	particle.Life = 1.0f;
	particle.Velocity = object->m_Velocity * 0.1f;
}
