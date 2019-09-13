#pragma once

#include <glm/glm.hpp>

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(80.0f, 15.0f);

// Initial velocity of the player paddle
const float PLAYER_VELOCITY = 500.0f;

// Margin of the player paddle from the bottom border
const float PLAYER_MARGIN = 20.0f;

// Initial velocity of the ball
const glm::vec2 BALL_VELOCITY(100.0f, -350.0f);

// Radius of the ball object
const float BALL_RADIUS = 12.5f;

// Size of the power up
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);

// Velocity of the power up
const glm::vec2 POWERUP_VELOCITY(0.0f, 150.0f);