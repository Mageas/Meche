#include "ScenePlay.h"

#include "Assets.h"
#include "GameEngine.h"
#include "SceneMenu.h"

#include <SFML/Graphics/Text.hpp>

namespace Meche
{

ScenePlay::ScenePlay(GameEngine* gameEngine, std::string levelPath) : Scene(gameEngine), m_LevelPath(std::move(levelPath))
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

void ScenePlay::Render()
{
	sf::Text selector(Assets::Get().GetFont("Arial"), m_LevelPath);
	selector.setPosition(Vec2(100, 100));
	m_GameEngine->GetWindow().draw(selector);
}

} // namespace Meche