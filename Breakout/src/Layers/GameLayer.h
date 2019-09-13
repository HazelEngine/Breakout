#pragma once

#include <Hazel.h>
using namespace Hazel;

#include "Config.h"
#include "GameState.h"
#include "Entities/PowerUp.h"
#include "Entities/GameLevel.h"
#include "Entities/BallObject.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/ParticleGenerator.h"
#include "Renderer/PostProcessor.h"

// GameLayer holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class GameLayer : public Hazel::Layer
{
public:
	GameLayer(Window* const window);

	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

private:
	void Render();
	void ProcessInput(float dt);
	void DoCollisions();
	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float dt);
	void ActivatePowerUp(PowerUp& powerUp);
	void ResetLevel();
	void ResetPlayer();

private:
	// Game state
	GameState                   m_State;
	std::unique_ptr<GameObject> m_Player;
	std::unique_ptr<BallObject> m_Ball;
	std::vector<PowerUp>        m_PowerUps;
	std::vector<GameLevel>      m_Levels;
	uint32_t                    m_CurrentLevel;
	uint32_t                    m_Lives;

	// Render state
	std::unique_ptr<Texture2D> m_BackgroundTex;
	std::unique_ptr<SpriteRenderer> m_Renderer;
	std::unique_ptr<ParticleGenerator> m_Particles;
	std::unique_ptr<PostProcessor> m_Effects;

	OrthographicCamera m_Camera;
	Window* m_Window;
};