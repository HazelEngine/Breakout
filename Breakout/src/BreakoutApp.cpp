#include <Hazel.h>

#include "GameState.h"
#include "Layers/GameLayer.h"

class Breakout : public Hazel::Application
{
public:
	Breakout()
	{
		PushLayer(new GameLayer(&GetWindow()));
	}

	~Breakout() {}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Breakout();
}