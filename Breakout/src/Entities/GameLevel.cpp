#include "GameLevel.h"

#include <fstream>

void GameLevel::Load(const std::string& file, uint32_t levelWidth, uint32_t levelHeight)
{
	// Clear old data
	m_Bricks.clear();

	// Load from file
	std::ifstream fstream(file);
	if (fstream)
	{
		VecOfU32Vecs tileData;
		uint32_t tileCode;

		std::string line;
		while (std::getline(fstream, line))    // Read each line from level file
		{
			std::istringstream sstream(line);
			std::vector<uint32_t> row;

			while (sstream >> tileCode)    // Read each word separated by spaces
				row.push_back(tileCode);

			tileData.push_back(row);
		}

		if (tileData.size() > 0)
			Init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer* const renderer)
{
	for (auto& tile : m_Bricks)
	{
		if (!tile.m_Destroyed)
			tile.Draw(renderer);
	}
}

bool GameLevel::IsCompleted()
{
	for (auto& tile : m_Bricks)
	{
		if (!tile.m_IsSolid && !tile.m_Destroyed)
			return false;
	}

	return true;
}

void GameLevel::Init(VecOfU32Vecs tileData, uint32_t levelWidth, uint32_t levelHeight)
{
	std::shared_ptr<Texture2D> texBlock;
	std::shared_ptr<Texture2D> texBlockSolid;

	// Calculate dimensions
	uint32_t height = tileData.size();
	// NOTE: we can index vector at [0] since this function is only called if height > 0
	uint32_t width = tileData[0].size();
	float unitWidth = levelWidth / static_cast<float>(width);
	float unitHeight = levelHeight / static_cast<float>(height);

	// Initialize level tiles based on tileData
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			// Check block type from level data
			if (tileData[y][x] == 1)    // Solid
			{
				// Load the solid block texture
				if (!texBlockSolid)
					texBlockSolid.reset(Texture2D::Load("Assets/Textures/Block_Solid.png"));

				glm::vec2 pos = glm::vec2(unitWidth * x, unitHeight * y);
				glm::vec2 size = glm::vec2(unitWidth, unitHeight);

				GameObject tile(pos, size, texBlockSolid, glm::vec3(0.8f, 0.8f, 0.7f));
				tile.m_IsSolid = true;

				m_Bricks.push_back(tile);
			}
			else if (tileData[y][x] > 1)    // Non-solid; now determine its color based on level data
			{
				// Load the block texture
				if (!texBlock)
					texBlock.reset(Texture2D::Load("Assets/Textures/Block.png"));

				glm::vec3 color = glm::vec3(1.0f);
				glm::vec2 pos = glm::vec2(unitWidth * x, unitHeight * y);
				glm::vec2 size = glm::vec2(unitWidth, unitHeight);

				if (tileData[y][x] == 2)          // Blue
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)     // Vivid green
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)     // Yellow
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)     // Orange
					color = glm::vec3(1.0f, 0.5f, 0.0f);
				else if (tileData[y][x] == 6)     // White
					color = glm::vec3(1.0f, 1.0f, 1.0f);
				else if (tileData[y][x] == 7)     // Green
					color = glm::vec3(0.039f, 0.48f, 0.011f);
				else if (tileData[y][x] == 8)     // Red
					color = glm::vec3(0.86f, 0.25f, 0.16f);
				else if (tileData[y][x] == 9)     // Purple
					color = glm::vec3(0.63f, 0.086f, 1.0f);
				else if (tileData[y][x] == 10)    // Dark green
					color = glm::vec3(0.21f, 0.37f, 0.015f);
				else if (tileData[y][x] == 11)    // Brown Gray
					color = glm::vec3(0.74f, 0.72f, 0.68f);
				else if (tileData[y][x] == 12)    // Brown
					color = glm::vec3(0.23f, 0.14f, 0.0f);

				GameObject tile(pos, size, texBlock, color);

				m_Bricks.push_back(tile);
			}
		}
	}
}
