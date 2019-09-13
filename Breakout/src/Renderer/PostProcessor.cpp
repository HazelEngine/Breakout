#include "PostProcessor.h"

PostProcessor::PostProcessor(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height), Confuse(false), Chaos(false), Shake(false)
{
	// Shader
	m_Shader.reset(Shader::Load("Assets/Shaders/PostProcessVS.cso", "Assets/Shaders/PostProcessPS.cso"));

	// Framebuffers
	m_Framebuffer.reset(Framebuffer::Create(FramebufferType::Texture2D, width, height));
	m_MSFramebuffer.reset(Framebuffer::Create(FramebufferType::Renderbuffer, width, height));

	// Offsets
	float offset = 1.0f / 300.0f;
	std::array<std::array<float, 2>, 9> offsets =
	{{
		{ -offset,  offset },    // top-left
		{    0.0f,  offset },    // top-center
		{  offset,  offset },    // top-right
		{ -offset,    0.0f },    // center-left
		{    0.0f,    0.0f },    // center-center
		{  offset,    0.0f },    // center-right
		{ -offset, -offset },    // bottom-left
		{    0.0f, -offset },    // bottom-center
		{  offset, -offset }     // bottom-right
	}};

	// Edge kernel
	std::array<int, 9> edgeKernel =
	{
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};

	// Blur kernel
	std::array<float, 9> blurKernel =
	{
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};

	// Initialize render data and uniforms
	InitRenderData();
	m_Shader->GetParam("Scene")->SetAsInt(0);
	m_Shader->GetParam("Offsets")->SetAsFloat2Array(offsets.size(), (const float*)offsets.data());
	m_Shader->GetParam("EdgeKernel")->SetAsIntArray(edgeKernel.size(), edgeKernel.data());
	m_Shader->GetParam("BlurKernel")->SetAsFloatArray(blurKernel.size(), blurKernel.data());
}

void PostProcessor::BeginRender()
{
	m_MSFramebuffer->Bind();

	RenderCommand::SetClearColor({ 0.8f, 0.8f, 0.8f, 1.0f });
	RenderCommand::Clear();
}

void PostProcessor::EndRender()
{
	// Now resolve multisampled colorbuffer into intermediate Framebuffer to store to texture
	m_MSFramebuffer->BlitTo(m_Framebuffer.get());
}

void PostProcessor::Render(float time)
{
	m_Shader->Bind();

	// Set uniforms/options
	m_Shader->GetParam("Time")->SetAsFloat(time);
	m_Shader->GetParam("Confuse")->SetAsInt(Confuse);
	m_Shader->GetParam("Chaos")->SetAsInt(Chaos);
	m_Shader->GetParam("Shake")->SetAsInt(Shake);

	// Render textured quad
	m_Framebuffer->GetBuffer(0)->Bind(TextureSlot0);

	Renderer::Submit(m_Shader, m_VertexArray);
}

void PostProcessor::InitRenderData()
{
	// Vertices
	std::array<float, 16> vertices =
	{
		// Positions   // TexCoords
	   -1.0f, -1.0f,   0.0f, 0.0f,
		1.0f,  1.0f,   1.0f, 1.0f,
	   -1.0f,  1.0f,   0.0f, 1.0f,
		1.0f, -1.0f,   1.0f, 0.0f
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