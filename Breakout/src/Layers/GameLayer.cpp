#include "GameLayer.h"

#include <imgui.h>
#include <irrKlang.h>
using namespace irrklang;

#include <Platform/OpenGL/OpenGLHelper.h>

#include "Collision.h"

// DeltaTime variables
// TODO: Remove!
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// TODO: Put in header file
float shakeTime = 0.0f;

// TODO: Change to appropriate location!
ISoundEngine* SoundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_MULTI_THREADED | ESEO_LOAD_PLUGINS | ESEO_USE_3D_BUFFERS);

bool ShouldSpawn(uint32_t chance)
{
	uint32_t random = rand() % chance;
	return random == 0;
}

bool IsOtherPowerUpActive(const std::vector<PowerUp>& powerUps, const std::string& type)
{
	// Check if another PowerUp of the same type is still active
	// in which case we don't disable its effect (yet)
	for (const auto& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}

	return false;
}

GameLayer::GameLayer(Window* const window)
	: Layer("Game"), m_State(Menu),
	  m_CurrentLevel(0), m_Lives(3), m_Window(window),
	  m_Camera(0.0f, window->GetWidth(), window->GetHeight(), 0.0f)
{
	OpenGLHelper::EnableBlend();

	// Load custom font for ImGui rendering
	auto io = ImGui::GetIO();
	ImFontConfig config; config.OversampleH = 5;
	io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/RubikMonoOne.ttf", 13.0f, &config);

	// Set up player
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/Paddle.png"));

		glm::vec2 pos = glm::vec2(
			window->GetWidth() * 0.5f - PLAYER_SIZE.x * 0.5f,
			window->GetHeight() - PLAYER_SIZE.y - PLAYER_MARGIN
		);

		m_Player.reset(new GameObject(pos, PLAYER_SIZE, tex));
	}

	// Set up ball
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/Ball.png"));

		glm::vec2 pos = m_Player->m_Position + glm::vec2(
			PLAYER_SIZE.x * 0.5f - BALL_RADIUS,
			PLAYER_SIZE.y * 0.75f - BALL_RADIUS * 2.0f
		);

		m_Ball.reset(new BallObject(pos, BALL_RADIUS, BALL_VELOCITY, tex));
	}

	// Load levels
	{
		GameLevel level;

		level.Load("Assets/Levels/One.lvl", window->GetWidth(), window->GetHeight() * 0.5f);
		m_Levels.push_back(level);

		level.Load("Assets/Levels/Two.lvl", window->GetWidth(), window->GetHeight() * 0.5f);
		m_Levels.push_back(level);

		level.Load("Assets/Levels/Three.lvl", window->GetWidth(), window->GetHeight() * 0.5f);
		m_Levels.push_back(level);

		level.Load("Assets/Levels/Four.lvl", window->GetWidth(), window->GetHeight() * 0.5f);
		m_Levels.push_back(level);

		level.Load("Assets/Levels/Joquer.lvl", window->GetWidth(), window->GetHeight() * 0.75f);
		m_Levels.push_back(level);

		level.Load("Assets/Levels/Mouse.lvl", window->GetWidth(), window->GetHeight() * 0.75f);
		m_Levels.push_back(level);
	}

	// Load background texture
	m_BackgroundTex.reset(Texture2D::Load("Assets/Textures/Background.jpg"));

	// Renderer
	m_Renderer.reset(new SpriteRenderer());

	// Particles
	auto texParticle = std::shared_ptr<Texture2D>();
	texParticle.reset(Texture2D::Load("Assets/Textures/Particle.png"));
	m_Particles.reset(new ParticleGenerator(texParticle, 500));

	// Effects
	m_Effects.reset(new PostProcessor(window->GetWidth(), window->GetHeight()));

	// Audio
	SoundEngine->play2D("Assets/Audio/Breakout.mp3", true);
}

void GameLayer::OnUpdate()
{
	// Calculate delta time
	// TODO: Replace!
	float currentFrame = OpenGLHelper::GetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Manage user input
	ProcessInput(deltaTime);

	// Update particles
	m_Particles->Update(deltaTime, m_Ball.get(), 2, glm::vec2(m_Ball->m_Radius * 0.5f));

	// Update power ups
	UpdatePowerUps(deltaTime);

	if (m_State == GameState::Active)
	{
		// Compute ball movement
		m_Ball->Move(deltaTime, m_Window->GetWidth());

		// Check for collisions
		DoCollisions();

		// Reduce shake time
		if (shakeTime > 0.0f)
		{
			shakeTime -= deltaTime;
			if (shakeTime <= 0.0f)
				m_Effects->Shake = false;
		}

		// Check loss condition
		if (m_Ball->m_Position.y >= m_Window->GetHeight())    // Did ball reach bottom edge?
		{
			--m_Lives;
			ResetPlayer();

			// Did the player lose all his lives? : Game over
			if (m_Lives == 0)
			{
				ResetLevel();
				m_State = GameState::Menu;
			}
		}

		// Check win condition
		if (m_Levels[m_CurrentLevel].IsCompleted())
		{
			ResetLevel();
			ResetPlayer();
			m_Effects->Chaos = true;
			m_State = GameState::Win;
		}
	}

	// Render even if the game state isn't 'active'
	Render();
}

void GameLayer::OnImGuiRender()
{
	ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;

	if (m_State == GameState::Menu)
	{
		ImGui::SetNextWindowPos(
			{ windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f },
			ImGuiCond_Always,
			{ 0.5f, 0.5f }
		);
		ImGui::Begin(
			"MenuTitle", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs
		);
			ImGui::SetWindowFontScale(2.0f);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Pressione ENTER para jogar");
		ImGui::End();

		ImGui::SetNextWindowPos(
			{ windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f + 25.0f },
			ImGuiCond_Always,
			{ 0.5f, 0.5f }
		);
		ImGui::Begin(
			"MenuSubtitle", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs
		);
			ImGui::SetWindowFontScale(1.5f);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Pressione L para selecionar o nivel");
		ImGui::End();
	}
	if (m_State == GameState::Active)
	{
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowPos({ windowPos.x + 10.0f, windowPos.y + 10.0f });
		ImGui::Begin(
			"Lives",
			nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs
		);
			std::string lives = "Vidas: " + std::to_string(m_Lives);
			ImGui::TextColored({ 0.69f, 0.95f, 0.25f, 1.0f }, lives.c_str());
		ImGui::End();
	}
	if (m_State == GameState::Win)
	{
		ImGui::SetNextWindowPos(
			{ windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f - 25.0f },
			ImGuiCond_Always,
			{ 0.5f, 0.5f }
		);
		ImGui::Begin(
			"WinTitle", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs
		);
			ImGui::SetWindowFontScale(2.0f);
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Voce GANHOU!!!");
		ImGui::End();

		ImGui::SetNextWindowPos(
			{ windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f },
			ImGuiCond_Always,
			{ 0.5f, 0.5f }
		);
		ImGui::Begin(
			"WinSubtitle", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs
		);
			ImGui::SetWindowFontScale(1.8f);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Pressione ENTER para jogar novamente ou ESC para sair");
		ImGui::End();
	}
}

void GameLayer::OnEvent(Event& event)
{
	if (event.GetEventType() == EventType::KeyPressed)
	{
		// Get pressed key code
		auto key = static_cast<KeyPressedEvent*>(&event)->GetKeyCode();

		// When in 'menu' or 'active' state
		if (m_State == GameState::Menu|| m_State == GameState::Active)
		{
			// If 'L' was pressed, change the current game level
			if (key == HZ_KEY_L)
			{
				m_CurrentLevel++;

				// If reached end of list, go back to start
				if (m_CurrentLevel >= m_Levels.size())
					m_CurrentLevel = 0;

				// Reset state
				ResetLevel();
				ResetPlayer();
			}
		}

		// If 'ENTER' was pressed (in 'menu' state), go to 'active' state
		if (m_State == GameState::Menu && key == HZ_KEY_ENTER)
			m_State = GameState::Active;

		// If 'ENTER' was pressed (in 'win' state), disable chaos effect
		if (m_State == GameState::Win && key == HZ_KEY_ENTER)
		{
			m_Effects->Chaos = false;
			m_State = GameState::Menu;
		}
	}
}

void GameLayer::Render()
{
	Renderer::BeginScene(*m_Window, m_Camera);
	
	m_Effects->BeginRender();

	// Draw background
	m_Renderer->DrawSprite(
		m_BackgroundTex.get(),
		glm::vec2(0.0f),
		glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()),
		0.0f
	);

	
	// Draw level
	m_Levels[m_CurrentLevel].Draw(m_Renderer.get());

	// Draw player
	m_Player->Draw(m_Renderer.get());

	// Draw power ups
	for (auto& powerUp : m_PowerUps)
		if (!powerUp.m_Destroyed)
			powerUp.Draw(m_Renderer.get());

	// Draw particles
	m_Particles->Draw();

	// Draw ball
	m_Ball->Draw(m_Renderer.get());
	
	m_Effects->EndRender();
	m_Effects->Render(OpenGLHelper::GetTime());

	Renderer::EndScene();
}

void GameLayer::ProcessInput(float dt)
{
	// Compute the player velocity
	float velocity = PLAYER_VELOCITY * dt;

	// Only when in 'active' state
	if (m_State == GameState::Active)
	{
		// To-left movement
		if (Input::IsKeyPressed(HZ_KEY_A))
		{
			// Avoid move outside window bounds
			if (m_Player->m_Position.x >= 0)
			{
				m_Player->m_Position.x -= velocity;

				// If the ball is stuck in the paddle, move it too
				if (m_Ball->m_Stuck)
					m_Ball->m_Position.x -= velocity;
			}
		}

		// To-right movement
		if (Input::IsKeyPressed(HZ_KEY_D))
		{
			// Avoid move outside window bounds
			if (m_Player->m_Position.x <= m_Window->GetWidth() - m_Player->m_Size.x)
			{
				m_Player->m_Position.x += velocity;

				// If the ball is stuck in the paddle, move it too
				if (m_Ball->m_Stuck)
					m_Ball->m_Position.x += velocity;
			}
		}

		// If 'SPACE' was pressed, release ball from the paddle
		if (Input::IsKeyPressed(HZ_KEY_SPACE))
		{
			m_Ball->m_Stuck = false;
		}
	}
}

void GameLayer::DoCollisions()
{
	for (auto& box : m_Levels[m_CurrentLevel].m_Bricks)
	{
		if (!box.m_Destroyed)
		{
			Collision collision = CheckCollision(*m_Ball, box);
			if (std::get<0>(collision))    // If collision is true
			{
				// Destroy block if not solid
				if (!box.m_IsSolid)
				{
					box.m_Destroyed = true;
					SpawnPowerUps(box);
					SoundEngine->play2D("Assets/Audio/Bleep.mp3", false);
				}
				else
				{
					// If block is solid, enable shake effect
					shakeTime = 0.05f;
					m_Effects->Shake = true;
					SoundEngine->play2D("Assets/Audio/Solid.wav", false);
				}

				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);

				// Don't do collision resolution on non-solid bricks if pass-through activated
				if (!(m_Ball->PassThrough && !box.m_IsSolid))
				{
					if (dir == Left || dir == Right)    // Horizontal collision
					{
						m_Ball->m_Velocity.x = -m_Ball->m_Velocity.x;    // Reverse horizontal velocity

						// Relocate
						float penetration = m_Ball->m_Radius - std::abs(diffVector.x);
						if (dir == Left)
							m_Ball->m_Position.x += penetration;    // Move ball to right
						else
							m_Ball->m_Position.x -= penetration;    // Move ball to left
					}
					else    // Vertical collision
					{
						m_Ball->m_Velocity.y = -m_Ball->m_Velocity.y;    // Reverse vertical velocity

						// Relocate
						float penetration = m_Ball->m_Radius - std::abs(diffVector.y);
						if (dir == Up)
							m_Ball->m_Position.y -= penetration;    // Move ball back up
						else
							m_Ball->m_Position.y += penetration;    // Move ball back down
					}
				}
			}
		}
	}

	// Also check collisions on power ups and if so, activate them
	for (auto& powerUp : m_PowerUps)
	{
		if (!powerUp.m_Destroyed)
		{
			// First check if power up passed bottom edge, if so: keep as inactive and destroy
			if (powerUp.m_Position.y >= m_Window->GetHeight())
				powerUp.m_Destroyed = true;

			if (CheckCollision(*m_Player, powerUp))
			{
				// Collided with player, now activate power up
				ActivatePowerUp(powerUp);
				powerUp.m_Destroyed = true;
				powerUp.Activated = true;
				SoundEngine->play2D("Assets/Audio/PowerUp.mp3", false);
			}
		}
	}

	// Also check collisions for player paddle (unless stuck)
	Collision collision = CheckPlayerCollision(*m_Ball, *m_Player);
	if (!m_Ball->m_Stuck && std::get<0>(collision))
	{
		// Check where it hit the paddle, and change velocity based on where it hit the paddle
		float centerBoard = m_Player->m_Position.x + m_Player->m_Size.x * 0.5f;
		float distance = (m_Ball->m_Position.x + m_Ball->m_Radius) - centerBoard;
		float percentage = distance / (m_Player->m_Size.x * 0.5f);

		// Then move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = m_Ball->m_Velocity;

		m_Ball->m_Velocity.x = BALL_VELOCITY.x * percentage * strength;
		
		// Keep speed consistent over both axis (multiply by length of old velocity, so total strength is not changed)
		m_Ball->m_Velocity = glm::normalize(m_Ball->m_Velocity) * glm::length(oldVelocity);
		
		// Fix sticky paddle
		m_Ball->m_Velocity.y = -1.0f * abs(m_Ball->m_Velocity.y);

		// If Sticky power up is activated, also stick ball to paddle once new velocity vectors were calculated
		m_Ball->m_Stuck = m_Ball->Sticky;

		SoundEngine->play2D("Assets/Audio/Bleep.wav", false);
	}
}

void GameLayer::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75))    // 1 in 75 chance
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/PowerUp_Speed.png"));
		m_PowerUps.push_back(
			PowerUp("Speed", 0.0f, block.m_Position, glm::vec3(0.5f, 0.5f, 1.0f), tex)
		);
	}
	if (ShouldSpawn(75))
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/PowerUp_Sticky.png"));
		m_PowerUps.push_back(
			PowerUp("Sticky", 20.0f, block.m_Position, glm::vec3(1.0f, 0.5f, 1.0f), tex)
		);
	}
	if (ShouldSpawn(75))
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/PowerUp_PassThrough.png"));
		m_PowerUps.push_back(
			PowerUp("PassThrough", 10.0f, block.m_Position, glm::vec3(0.5f, 1.0f, 0.5f), tex)
		);
	}
	if (ShouldSpawn(75))
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/PowerUp_Increase.png"));
		m_PowerUps.push_back(
			PowerUp("PadSizeIncrease", 0.0f, block.m_Position, glm::vec3(1.0f, 0.6f, 0.4f), tex)
		);
	}
	if (ShouldSpawn(15))    // Negative power ups should spawn more often
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/PowerUp_Confuse.png"));
		m_PowerUps.push_back(
			PowerUp("Confuse", 15.0f, block.m_Position, glm::vec3(1.0f, 0.3f, 0.3f), tex)
		);
	}
	if (ShouldSpawn(15))
	{
		auto tex = std::shared_ptr<Texture2D>();
		tex.reset(Texture2D::Load("Assets/Textures/PowerUp_Chaos.png"));
		m_PowerUps.push_back(
			PowerUp("Chaos", 15.0f, block.m_Position, glm::vec3(0.9f, 0.25f, 0.25f), tex)
		);
	}
}

void GameLayer::UpdatePowerUps(float dt)
{
	for (auto& powerUp : m_PowerUps)
	{
		powerUp.m_Position += powerUp.m_Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// Remove power up from list (will later be removed)
				powerUp.Activated = false;

				// Deactive effects
				if (powerUp.Type == "Sticky")
				{
					if (!IsOtherPowerUpActive(m_PowerUps, "Sticky"))
					{
						// Only reset if no other power up of type Sticky is active
						m_Ball->Sticky = false;
						m_Player->m_Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "PassThrough")
				{
					if (!IsOtherPowerUpActive(m_PowerUps, "PassThrough"))
					{
						// Only reset if no other power up of type PassThrough is active
						m_Ball->PassThrough = false;
						m_Ball->m_Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "Confuse")
				{
					if (!IsOtherPowerUpActive(m_PowerUps, "Confuse"))
					{
						// Only reset if no other power up of type Confuse is active
						m_Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "Chaos")
				{
					if (!IsOtherPowerUpActive(m_PowerUps, "Chaos"))
					{
						// Only reset if no other power up of type Chaos is active
						m_Effects->Chaos = false;
					}
				}
			}
		}
	}

	// Remove all power ups from vector that are destroyed AND !activated (thus either off the map or finished)
	// Note we use a lambda expression to remove each PowerUp which is destroyed and not activated.
	m_PowerUps.erase(std::remove_if(
			m_PowerUps.begin(), m_PowerUps.end(),
			[](const PowerUp& powerUp) { return powerUp.m_Destroyed && !powerUp.Activated; }
		),
		m_PowerUps.end()
	);
}

void GameLayer::ActivatePowerUp(PowerUp& powerUp)
{
	// Initialize a power up based type of power up
	if (powerUp.Type == "Speed")
	{
		m_Ball->m_Velocity *= 1.2f;
	}
	else if (powerUp.Type == "Sticky")
	{
		m_Ball->Sticky = true;
		m_Player->m_Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "PassThrough")
	{
		m_Ball->PassThrough = true;
		m_Ball->m_Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "PadSizeIncrease")
	{
		m_Player->m_Size.x += 50.0f;
	}
	else if (powerUp.Type == "Confuse")
	{
		if (!m_Effects->Chaos)
			m_Effects->Confuse = true;    // Only activate if chaos wasn't already active
	}
	else if (powerUp.Type == "Chaos")
	{
		if (!m_Effects->Confuse)
			m_Effects->Chaos = true;    // Only activate if confuse wasn't already active
	}
}

void GameLayer::ResetLevel()
{
	m_Lives = 3;

	if (m_CurrentLevel == 0)
		m_Levels[0].Load("Assets/Levels/One.lvl", m_Window->GetWidth(), m_Window->GetHeight() * 0.5f);
	else if (m_CurrentLevel == 1)
		m_Levels[1].Load("Assets/Levels/Two.lvl", m_Window->GetWidth(), m_Window->GetHeight() * 0.5f);
	else if (m_CurrentLevel == 2)
		m_Levels[2].Load("Assets/Levels/Three.lvl", m_Window->GetWidth(), m_Window->GetHeight() * 0.5f);
	else if (m_CurrentLevel == 3)
		m_Levels[3].Load("Assets/Levels/Four.lvl", m_Window->GetWidth(), m_Window->GetHeight() * 0.5f);
	else if (m_CurrentLevel == 4)
		m_Levels[4].Load("Assets/Levels/Joquer.lvl", m_Window->GetWidth(), m_Window->GetHeight() * 0.75f);
	else if (m_CurrentLevel == 5)
		m_Levels[5].Load("Assets/Levels/Mouse.lvl", m_Window->GetWidth(), m_Window->GetHeight() * 0.75f);
}

void GameLayer::ResetPlayer()
{
	// Reset player/ball stats
	m_Player->m_Size = PLAYER_SIZE;
	m_Player->m_Position = glm::vec2(
		m_Window->GetWidth() * 0.5f - PLAYER_SIZE.x * 0.5f,
		m_Window->GetHeight() - PLAYER_SIZE.y - PLAYER_MARGIN
	);
	m_Ball->Reset(m_Player->m_Position + glm::vec2(
		PLAYER_SIZE.x * 0.5f - BALL_RADIUS,
		PLAYER_SIZE.y * 0.75f - BALL_RADIUS * 2.0f
	), BALL_VELOCITY);

	// Also disable all active power ups
	m_Effects->Chaos = m_Effects->Confuse = false;
	m_Ball->PassThrough = m_Ball->Sticky = false;
	m_Player->m_Color = glm::vec3(1.0f);
	m_Ball->m_Color = glm::vec3(1.0f);
}
