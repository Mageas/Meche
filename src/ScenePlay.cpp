#include "ScenePlay.h"

#include "Assets.h"
#include "GameEngine.h"
#include "SceneMenu.h"

#include <format>
#include <fstream>
#include <limits>
#include <print>

namespace Meche
{

ScenePlay::ScenePlay(GameEngine* gameEngine, std::string levelPath)
	: Scene(gameEngine), m_LevelPath(std::move(levelPath))
{
	RegisterAction(sf::Keyboard::Scancode::Escape, Action::Name::Quit);

	RegisterAction(sf::Keyboard::Scancode::A, Action::Name::Left);
	RegisterAction(sf::Keyboard::Scancode::D, Action::Name::Right);
}

std::expected<void, std::string> ScenePlay::Init()
{
	if (const auto result = LoadLevel(); !result)
		return std::unexpected(result.error());

	SpawnPlayer();

	return {};
}

void ScenePlay::Update()
{
	m_EntityManager.Update();
	UpdatePlayerVelocity();
	UpdateMovement();
	UpdateAnimation();
}

void ScenePlay::DoAction(const Action& action)
{
	InputComponent& input = m_Player->GetComponent<InputComponent>();
	const bool isPressed = action.GetType() == Action::Type::Begin;

	switch (action.GetName())
	{
	case Action::Name::Quit:
		if (isPressed)
		{
			if (const auto result = m_GameEngine->ChangeScene<SceneMenu>("Menu"); !result)
				std::println(stderr, "[PLAY] {}", result.error());
		}
		break;
	case Action::Name::Right:
		if (m_Player->IsAlive())
			input.Right = isPressed;
		break;
	case Action::Name::Left:
		if (m_Player->IsAlive())
			input.Left = isPressed;
		break;
	default:
		break;
	}
}

void ScenePlay::Render()
{
	const sf::View view(Vec2(s_WorldWidth / 2, s_WorldHeight / 2), Vec2(s_WorldWidth, s_WorldHeight));
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
			Vec2 cell;

			file >> name >> cell.X >> cell.Y;

			SpawnTile(name, cell, Entity::Tag::Tile);
		}
		else if (keyword == "Dec")
		{
			std::string name;
			Vec2 cell;

			file >> name >> cell.X >> cell.Y;

			SpawnTile(name, cell, Entity::Tag::Decoration);
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

void ScenePlay::SpawnTile(const std::string& name, const Vec2& cell, Entity::Tag tag)
{
	const Animation& animation = Assets::Get().GetAnimation(name);
	const EntityPtr tile = m_EntityManager.AddEntity(tag);
	tile->AddComponent<TransformComponent>(GridToMidPixel(cell, animation.GetSize()));
	if (tag == Entity::Tag::Tile)
		tile->AddComponent<BoundingBoxComponent>(animation.GetSize());
	tile->AddComponent<AnimationComponent>(animation, true);
}

void ScenePlay::SpawnPlayer()
{
	const EntityPtr player = m_EntityManager.AddEntity(Entity::Tag::Player);
	AnimationComponent& animation =
		player->AddComponent<AnimationComponent>(Assets::Get().GetAnimation("PigIdle"), true);
	animation.Current.GetSprite().setScale(Vec2(-1, 1));
	player->AddComponent<TransformComponent>(GridToMidPixel(m_PlayerConfig.SpawnCell, animation.Current.GetSize()));
	player->AddComponent<BoundingBoxComponent>(m_PlayerConfig.CollisionSize);
	player->AddComponent<InputComponent>();

	m_Player = std::move(player);
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

void ScenePlay::UpdatePlayerVelocity()
{
	if (m_Player->IsAlive())
	{
		TransformComponent& transform = m_Player->GetComponent<TransformComponent>();
		const InputComponent& input = m_Player->GetComponent<InputComponent>();

		float velocityX = 0;

		if (input.Right)
			velocityX += m_PlayerConfig.RunSpeed;
		if (input.Left)
			velocityX -= m_PlayerConfig.RunSpeed;

		transform.Velocity.X = velocityX;
	}
}

void ScenePlay::UpdateMovement()
{
	for (const auto& entity : m_EntityManager.GetEntities())
	{
		TransformComponent& transform = entity->GetComponent<TransformComponent>();

		if (!entity->IsAlive() || !transform.Exists)
			continue;

		transform.Position += transform.Velocity;
	}
}

void ScenePlay::UpdateAnimation()
{
	if (m_Player->IsAlive())
	{
		AnimationComponent& animation = m_Player->GetComponent<AnimationComponent>();
		TransformComponent& transform = m_Player->GetComponent<TransformComponent>();

		if (transform.Velocity.X > 0)
		{
			if (animation.Current.GetName() != "PigRun")
				animation.Current = Assets::Get().GetAnimation("PigRun");
			animation.Current.GetSprite().setScale(Vec2(-1, 1));
		}
		else if (transform.Velocity.X < 0)
		{
			if (animation.Current.GetName() != "PigRun")
				animation.Current = Assets::Get().GetAnimation("PigRun");
			animation.Current.GetSprite().setScale(Vec2(1, 1));
		}
		else
		{
			const auto scale = animation.Current.GetSprite().getScale();
			if (animation.Current.GetName() != "PigIdle")
				animation.Current = Assets::Get().GetAnimation("PigIdle");
			animation.Current.GetSprite().setScale(scale);
		}
	}

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		AnimationComponent& animation = entity->GetComponent<AnimationComponent>();

		if (!entity->IsAlive() || !animation.Exists)
			continue;

		animation.Current.Update();
	}
}

} // namespace Meche
