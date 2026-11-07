#include "ScenePlay.h"

#include "Assets.h"
#include "GameEngine.h"
#include "SceneMenu.h"

#include <SFML/Graphics/Text.hpp>
#include <fstream>
#include <print>

namespace Meche
{

ScenePlay::ScenePlay(GameEngine* gameEngine, std::string levelPath)
	: Scene(gameEngine), m_LevelPath(std::move(levelPath))
{
	RegisterAction(sf::Keyboard::Scancode::Escape, Action::Name::Quit);
}

std::expected<void, std::string> ScenePlay::Init() { return LoadLevel(); }

void ScenePlay::Update() { m_EntityManager.Update(); }

void ScenePlay::DoAction(const Action& action)
{
	if (action.GetType() == Action::Type::Begin)
	{
		switch (action.GetName())
		{
		case Action::Name::Quit:
			if (const auto result = m_GameEngine->ChangeScene<SceneMenu>("Menu"); !result)
				std::println(stderr, "[PLAY] {}", result.error());
			break;
		default:
			break;
		}
	}
}

void ScenePlay::Render()
{
	sf::View view(Vec2(s_WorldWidth / 2, s_WorldHeight / 2), Vec2(s_WorldWidth, s_WorldHeight));
	m_GameEngine->GetWindow().setView(view);

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		AnimationComponent& animation = entity->GetComponent<AnimationComponent>();
		TransformComponent& transform = entity->GetComponent<TransformComponent>();

		if (!entity->IsAlive() || !animation.Exists || !transform.Exists)
			continue;

		auto& sprite = animation.Current.GetSprite();
		sprite.setPosition(transform.Position);
		m_GameEngine->GetWindow().draw(sprite);
	}
}

std::expected<void, std::string> ScenePlay::LoadLevel()
{
	std::ifstream file(m_LevelPath);
	std::string keyword;

	if (!file.is_open())
		return std::unexpected(std::format("Unable to open '{}'", m_LevelPath));

	while (file >> keyword)
	{
		if (keyword.starts_with('#'))
		{
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		if (keyword == "Tile")
		{
			std::string name;
			uint32_t positionX = 0;
			uint32_t positionY = 0;

			file >> name >> positionX >> positionY;

			const Animation& animation = Assets::Get().GetAnimation(name);
			const EntityPtr tile = m_EntityManager.AddEntity(Entity::Tag::Tile);
			tile->AddComponent<TransformComponent>(GridToMidPixel(Vec2(positionX, positionY), animation.GetSize()));
			tile->AddComponent<BoundingBoxComponent>(animation.GetSize());
			tile->AddComponent<AnimationComponent>(animation, true);
		}
		else if (keyword == "Dec")
		{
			std::string name;
			uint32_t positionX = 0;
			uint32_t positionY = 0;

			file >> name >> positionX >> positionY;

			const Animation& animation = Assets::Get().GetAnimation(name);
			const EntityPtr tile = m_EntityManager.AddEntity(Entity::Tag::Decoration);
			tile->AddComponent<TransformComponent>(GridToMidPixel(Vec2(positionX, positionY), animation.GetSize()));
			tile->AddComponent<AnimationComponent>(animation, true);
		}
		else if (keyword == "Bomb")
		{
			file >> m_BombConfig.AnimationName >> m_BombConfig.CollisionSize.X >> m_BombConfig.CollisionSize.Y >>
				m_BombConfig.ThrowSpeed.X >> m_BombConfig.ThrowSpeed.Y >> m_BombConfig.FuseFrames >>
				m_BombConfig.BlastSize.X >> m_BombConfig.BlastSize.Y;
		}
		else if (keyword == "Player")
		{
			file >> m_PlayerConfig.SpawnCell.X >> m_PlayerConfig.SpawnCell.Y >> m_PlayerConfig.CollisionSize.X >>
				m_PlayerConfig.CollisionSize.Y >> m_PlayerConfig.RunSpeed >> m_PlayerConfig.JumpSpeed >>
				m_PlayerConfig.MaxSpeed >> m_PlayerConfig.Gravity;
		}
		else
		{
			return std::unexpected(std::format("Unknown keyword '{}' in '{}'", keyword, m_LevelPath));
		}
	}

	return {};
}

Vec2 ScenePlay::GridToMidPixel(const Vec2& position, const Vec2& size) const
{
	const float cellLeft = position.X * s_CellSize;
	const float cellBottom = s_WorldHeight - (position.Y * s_CellSize);

	return {
		cellLeft + (size.X / 2),
		cellBottom - (size.Y / 2),
	};
}

} // namespace Meche