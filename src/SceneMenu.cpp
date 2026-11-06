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

	m_TestAnimation = Assets::Get().GetAnimation("PigRun");
}

void SceneMenu::Update()
{
	m_TestAnimation.Update();
}

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

	auto& testSprite = m_TestAnimation.GetSprite();
	testSprite.setPosition(Vec2(500, 200));
	testSprite.setScale(Vec2(10, 10));
	m_GameEngine->GetWindow().draw(testSprite);
}

} // namespace Meche