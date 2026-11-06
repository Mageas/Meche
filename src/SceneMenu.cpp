#include "SceneMenu.h"

#include "Assets.h"
#include "ScenePlay.h"

#include "GameEngine.h"
#include "SFML/Graphics/Sprite.hpp"

namespace Meche
{

SceneMenu::SceneMenu(GameEngine* gameEngine) : Scene(gameEngine)
{
	RegisterAction(sf::Keyboard::Scancode::Escape, Action::Name::Quit);
	RegisterAction(sf::Keyboard::Scancode::S, Action::Name::Down);
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

	sf::Sprite sprite(Assets::Get().GetTexture("TexPigIdle"));
	m_GameEngine->GetWindow().draw(sprite);

}

} // namespace Meche