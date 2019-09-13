#pragma once

// Represents the four possible (collision) directions
enum Direction { Up, Right, Down, Left };

// Defines a Collision typedef that represents collision data
// <collision?, what direction?, difference vector center - closest point>
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Calculates which direction a vector is facing (N, E, S, or W)
Direction VectorDirection(glm::vec2 target)
{
	static glm::vec2 compass[] =
	{
		glm::vec2(0.0f,  1.0f),    // Up
		glm::vec2(1.0f,  0.0f),    // Right
		glm::vec2(0.0f, -1.0f),    // Down
		glm::vec2(-1.0f,  0.0f)     // Left
	};

	float max = 0.0f;
	uint32_t bestMatch = -1;

	for (uint32_t i = 0; i < 4; i++)
	{
		float dotProduct = glm::dot(glm::normalize(target), compass[i]);
		if (dotProduct > max)
		{
			max = dotProduct;
			bestMatch = i;
		}
	}

	return static_cast<Direction>(bestMatch);
}

bool CheckCollision(const GameObject& one, const GameObject& two)    // AABB - AABB collision
{
	// Collision x-axis?
	bool xCollision = one.m_Position.x + one.m_Size.x >= two.m_Position.x
		&& two.m_Position.x + two.m_Size.x >= one.m_Position.x;

	// Collision y-axis?
	bool yCollision = one.m_Position.y + one.m_Size.y >= two.m_Position.y
		&& two.m_Position.y + two.m_Size.y >= one.m_Position.y;

	// Collision only if on both axes
	return xCollision && yCollision;
}

Collision CheckCollision(const BallObject& one, const GameObject& two)    // AABB - circle collision
{
	// Get center point circle first
	glm::vec2 center(one.m_Position + one.m_Radius);

	// Calculate AABB info (center, half-extents)
	glm::vec2 aabbHalfExtents(two.m_Size.x * 0.5f, two.m_Size.y * 0.5f);
	glm::vec2 aabbCenter(
		two.m_Position.x + aabbHalfExtents.x,
		two.m_Position.y + aabbHalfExtents.y
	);

	// Get difference vector between both centers
	glm::vec2 difference = center - aabbCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);

	// Add clamped value to aabbCenter and we get the value of box closest to circle
	glm::vec2 closest = aabbCenter + clamped;

	// Retrieve vector between center circle and closest point AABB
	difference = closest - center;

	// And check if length <= radius
	if (glm::length(difference) <= one.m_Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, Up, glm::vec2(0.0f));
}

Collision CheckPlayerCollision(const BallObject& ball, const GameObject& player)    // AABB - circle collision
{
	// Get center point circle first
	glm::vec2 center(ball.m_Position + ball.m_Radius);

	// Calculate AABB info (center, half-extents)
	glm::vec2 aabbHalfExtents(player.m_Size.x * 0.5f, player.m_Size.y * 0.5f);
	glm::vec2 aabbCenter(
		player.m_Position.x + aabbHalfExtents.x,
		player.m_Position.y + aabbHalfExtents.y
	);

	// Get difference vector between both centers
	glm::vec2 difference = center - aabbCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);

	// Add clamped value to aabbCenter and we get the value of box closest to circle
	glm::vec2 closest = aabbCenter + clamped;

	// Retrieve vector between center circle and closest point AABB
	difference = closest - center;

	// And check if length <= radius
	if (glm::length(difference) <= ball.m_Radius * 0.25f)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, Up, glm::vec2(0.0f));
}