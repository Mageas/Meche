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
	RegisterAction(sf::Keyboard::Scancode::D, Action::Name::Play);

	EntityPtr entity = m_EntityManager.AddEntity(Entity::Tag::Player);
	entity->AddComponent<AnimationComponent>(Assets::Get().GetAnimation("PigRun"), true);
}

void SceneMenu::Update()
{
	m_EntityManager.Update();

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		if (entity->IsAlive() && entity->HasComponent<AnimationComponent>())
			entity->GetComponent<AnimationComponent>().Current.Update();
	}
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
		case Action::Name::Play:
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

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		if (entity->IsAlive() && entity->HasComponent<AnimationComponent>())
		{
			auto& sprite = entity->GetComponent<AnimationComponent>().Current.GetSprite();

			sprite.setPosition(Vec2(500, 200));
			sprite.setScale(Vec2(10, 10));
			m_GameEngine->GetWindow().draw(sprite);
		}
	}
}

} // namespace Meche