#include "SceneMenu.h"

#include "GameEngine.h"
#include "ScenePlay.h"

namespace Meche
{

SceneMenu::SceneMenu(GameEngine* gameEngine) : Scene(gameEngine)
{
	RegisterAction(sf::Keyboard::Scancode::Escape, Action::Name::Quit);
}

void SceneMenu::Update() {}

void SceneMenu::DoAction(const Action& action)
{
	if (action.GetType() == Action::Type::Begin)
	{
		switch (action.GetName())
		{
		case Action::Name::Quit:
			m_GameEngine->Quit();
			break;
		case Action::Name::Down:
			m_GameEngine->ChangeScene<ScenePlay>("Game");
			break;
		default:
			break;
		}
	}
}

void SceneMenu::Render()
{
	m_GameEngine->GetWindow().clear(sf::Color(10, 20, 60));
}

} // namespace Meche