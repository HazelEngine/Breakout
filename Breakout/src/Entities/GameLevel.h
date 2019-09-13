#pragma once

#include <Hazel.h>

#include "Entities/GameObject.h"
#include "Renderer/SpriteRenderer.h"

typedef std::vector<std::vector<uint32_t>> VecOfU32Vecs;

class GameLevel
{
public:
	// Ctor
	GameLevel() {};

	// Loads level from file
	void Load(const std::string& file, uint32_t levelWidth, uint32_t levelHeight);

	// Render level
	void Draw(SpriteRenderer* const renderer);

	// Check if the level is completed (all non-solid tiles are destroyed)
	bool IsCompleted();

private:
	void Init(VecOfU32Vecs tileData, uint32_t levelWidth, uint32_t levelHeight);

public:
	// Level state
	std::vector<GameObject> m_Bricks;
};