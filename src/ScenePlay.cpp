#include "ScenePlay.h"

#include "Assets.h"
#include "GameEngine.h"
#include "Physics.h"
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

	RegisterAction(sf::Keyboard::Scancode::W, Action::Name::Up);
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
	UpdateCollisions();
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
	case Action::Name::Up:
		if (m_Player->IsAlive())
		{
			if (isPressed)
			{
				input.Jump = true;
			}
			else
			{
				input.Jump = false;
				input.CanJump = true;
			}
		}
		break;
	case Action::Name::Left:
		if (m_Player->IsAlive())
			input.Left = isPressed;
		break;
	case Action::Name::Right:
		if (m_Player->IsAlive())
			input.Right = isPressed;
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
		InputComponent& input = m_Player->GetComponent<InputComponent>();

		Vec2 velocity;

		if (input.Right)
			velocity.X += m_PlayerConfig.RunSpeed;
		if (input.Left)
			velocity.X -= m_PlayerConfig.RunSpeed;

		if (input.Jump && input.CanJump && input.IsOnGround)
		{
			velocity.Y -= m_PlayerConfig.JumpSpeed;
			input.CanJump = false;
		}

		if (transform.Velocity.Y < 0 && !input.Jump)
			transform.Velocity.Y = 0;

		velocity.Y += m_PlayerConfig.Gravity;

		transform.Velocity.X = velocity.X;
		transform.Velocity.Y =
			std::clamp(transform.Velocity.Y + velocity.Y, -m_PlayerConfig.MaxSpeed, m_PlayerConfig.MaxSpeed);
	}
}

void ScenePlay::UpdateMovement()
{
	for (const auto& entity : m_EntityManager.GetEntities())
	{
		TransformComponent& transform = entity->GetComponent<TransformComponent>();

		if (!entity->IsAlive() || !transform.Exists)
			continue;

		transform.PreviousPosition = transform.Position;
		transform.Position += transform.Velocity;
	}

	if (m_Player->IsAlive())
	{
		TransformComponent& transform = m_Player->GetComponent<TransformComponent>();
		if (transform.Position.Y > s_WorldHeight)
		{
			const AnimationComponent& animation = m_Player->GetComponent<AnimationComponent>();
			transform.Position = GridToMidPixel(m_PlayerConfig.SpawnCell, animation.Current.GetSize());
			transform.PreviousPosition = transform.Position;
			transform.Velocity = Vec2();
		}

		const BoundingBoxComponent& boundingBox = m_Player->GetComponent<BoundingBoxComponent>();
		if (transform.Position.X - boundingBox.HalfSize.X < 0)
			transform.Position.X = boundingBox.HalfSize.X;
	}
}

void ScenePlay::UpdateCollisions()
{
	if (!m_Player->IsAlive())
		return;

	TransformComponent& playerTransform = m_Player->GetComponent<TransformComponent>();
	InputComponent& playerInput = m_Player->GetComponent<InputComponent>();

	playerInput.IsOnGround = false;

	for (const auto& entity : m_EntityManager.GetEntitiesByTag(Entity::Tag::Tile))
	{
		const BoundingBoxComponent& entityBoundingBox = entity->GetComponent<BoundingBoxComponent>();
		const TransformComponent& entityTransform = entity->GetComponent<TransformComponent>();

		if (!entity->IsAlive() || !entityBoundingBox.Exists || !entityTransform.Exists)
			continue;

		const Vec2 overlap = Physics::GetOverlap(*m_Player, *entity);
		if (!(overlap.X > 0 && overlap.Y > 0))
			continue;

		const Vec2 previousOverlap = Physics::GetPreviousOverlap(*m_Player, *entity);
		const bool resolveY = previousOverlap.X > 0 || (previousOverlap.Y <= 0 && overlap.Y < overlap.X);

		if (resolveY)
		{
			const bool isAbove = playerTransform.Position.Y < entityTransform.Position.Y;
			playerTransform.Position.Y += isAbove ? -overlap.Y : overlap.Y;
			playerTransform.Velocity.Y = 0;

			if (isAbove)
				playerInput.IsOnGround = true;
		}
		else
		{
			const bool isOnRight = playerTransform.Position.X > entityTransform.Position.X;
			playerTransform.Position.X += isOnRight ? overlap.X : -overlap.X;
		}
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
