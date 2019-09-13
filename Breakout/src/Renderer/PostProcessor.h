#pragma once

#include <Hazel.h>

#include "SpriteRenderer.h"

// PostProcessor host all post-processing effects for the Breakout
// game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or
// Shake boolean.
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor
{
public:
	// Ctor
	PostProcessor(uint32_t width, uint32_t height);

	// Prepares the post-processor's framebuffer operations before rendering the game
	void BeginRender();

	// Should be called after rendering the game, so it stores all the rendered data into a texture object
	void EndRender();

	// Renders the PostProcessors texture quad (as a screen-encompassing large sprite)
	void Render(float time);

private:
	// Initialize quad for rendering post-processing texture
	void InitRenderData();

public:
	// Options
	bool Confuse, Chaos, Shake;

private:
	// State
	uint32_t m_Width, m_Height;
	std::shared_ptr<Shader> m_Shader;

	// Render state
	std::unique_ptr<Framebuffer> m_MSFramebuffer;    // Multisampled Framebuffer. 
	std::unique_ptr<Framebuffer> m_Framebuffer;    // Regular framebuffer, used for blitting MS Framebuffer to texture
	std::shared_ptr<VertexArray> m_VertexArray;
};