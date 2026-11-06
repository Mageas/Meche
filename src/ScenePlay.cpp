#include "ScenePlay.h"

#include "GameEngine.h"
#include "SceneMenu.h"

namespace Meche
{

ScenePlay::ScenePlay(GameEngine* gameEngine) : Scene(gameEngine)
{
	RegisterAction(sf::Keyboard::Scancode::Escape, Action::Name::Quit);
}

void ScenePlay::Update() {}

void ScenePlay::DoAction(const Action& action)
{
	if (action.GetType() == Action::Type::Begin)
	{
		switch (action.GetName())
		{
		case Action::Name::Quit:
			m_GameEngine->ChangeScene<SceneMenu>("Menu");
			break;
		default:
			break;
		}
	}
}

void ScenePlay::Render() {}

} // namespace Meche