#include "ScenePlay.h"

#include "Assets.h"
#include "GameEngine.h"
#include "Physics.h"
#include "SceneMenu.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <algorithm>
#include <cmath>
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
	RegisterAction(sf::Keyboard::Scancode::T, Action::Name::ToggleTextures);
	RegisterAction(sf::Keyboard::Scancode::C, Action::Name::ToggleCollision);
	RegisterAction(sf::Keyboard::Scancode::G, Action::Name::ToggleGrid);
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
	case Action::Name::ToggleTextures:
		if (isPressed)
			m_ShowTextures = !m_ShowTextures;
		break;
	case Action::Name::ToggleCollision:
		if (isPressed)
			m_ShowCollisions = !m_ShowCollisions;
		break;
	case Action::Name::ToggleGrid:
		if (isPressed)
			m_ShowGrid = !m_ShowGrid;
		break;
	default:
		break;
	}
}

void ScenePlay::Render()
{
	ApplyCamera();

	if (m_ShowTextures)
		RenderEntities();
	if (m_ShowCollisions)
		RenderDebugCollisions();
	if (m_ShowGrid)
		RenderDebugGrid();
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
	EntityPtr player = m_EntityManager.AddEntity(Entity::Tag::Player);
	const AnimationComponent& animation =
		player->AddComponent<AnimationComponent>(Assets::Get().GetAnimation("PigIdle"), true);
	player->AddComponent<TransformComponent>(GridToMidPixel(m_PlayerConfig.SpawnCell, animation.Current.GetSize()),
											 Vec2(-1.0f, 1.0f));
	player->AddComponent<BoundingBoxComponent>(m_PlayerConfig.CollisionSize);
	player->AddComponent<InputComponent>();
	player->AddComponent<StateComponent>();

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

		const StateComponent& state = m_Player->GetComponent<StateComponent>();
		if (input.Jump && input.CanJump && state.Current != PlayerState::Air)
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
	StateComponent& playerState = m_Player->GetComponent<StateComponent>();

	bool isOnGround = false;

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
				isOnGround = true;
		}
		else
		{
			const bool isOnRight = playerTransform.Position.X > entityTransform.Position.X;
			playerTransform.Position.X += isOnRight ? overlap.X : -overlap.X;
		}
	}

	if (!isOnGround)
		playerState.Current = PlayerState::Air;
	else if (playerTransform.Velocity.X != 0)
		playerState.Current = PlayerState::Run;
	else
		playerState.Current = PlayerState::Stand;
}

void ScenePlay::UpdateAnimation()
{
	if (m_Player->IsAlive())
	{
		AnimationComponent& animation = m_Player->GetComponent<AnimationComponent>();
		TransformComponent& transform = m_Player->GetComponent<TransformComponent>();

		const StateComponent& state = m_Player->GetComponent<StateComponent>();

		std::string animationName;
		switch (state.Current)
		{
		case PlayerState::Stand:
			animationName = "PigIdle";
			break;
		case PlayerState::Run:
			animationName = "PigRun";
			break;
		case PlayerState::Air:
			animationName = transform.Velocity.Y < 0 ? "PigJump" : "PigFall";
			break;
		}

		if (animation.Current.GetName() != animationName)
			animation.Current = Assets::Get().GetAnimation(animationName);

		if (transform.Velocity.X > 0)
			transform.Scale.X = -1.0f;
		else if (transform.Velocity.X < 0)
			transform.Scale.X = 1.0f;
	}

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		AnimationComponent& animation = entity->GetComponent<AnimationComponent>();

		if (!entity->IsAlive() || !animation.Exists)
			continue;

		animation.Current.Update();
	}
}

void ScenePlay::ApplyCamera()
{
	const Vec2& playerPosition = m_Player->GetComponent<TransformComponent>().Position;
	const float viewCenterX = std::max(playerPosition.X, s_WorldWidth / 2);

	const sf::View view(Vec2(viewCenterX, s_WorldHeight / 2), Vec2(s_WorldWidth, s_WorldHeight));
	m_GameEngine->GetWindow().setView(view);
}

void ScenePlay::RenderEntities()
{
	sf::RenderWindow& window = m_GameEngine->GetWindow();

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		AnimationComponent& animation = entity->GetComponent<AnimationComponent>();
		const TransformComponent& transform = entity->GetComponent<TransformComponent>();

		if (!entity->IsAlive() || !animation.Exists || !transform.Exists)
			continue;

		auto& sprite = animation.Current.GetSprite();
		sprite.setPosition(transform.Position);
		sprite.setScale(transform.Scale);
		window.draw(sprite);
	}
}

void ScenePlay::RenderDebugCollisions()
{
	sf::RenderWindow& window = m_GameEngine->GetWindow();

	sf::RectangleShape rectangle;
	rectangle.setFillColor(sf::Color::Transparent);
	rectangle.setOutlineColor(sf::Color::White);
	rectangle.setOutlineThickness(-1);

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		const TransformComponent& transform = entity->GetComponent<TransformComponent>();
		const BoundingBoxComponent& boundingBox = entity->GetComponent<BoundingBoxComponent>();

		if (!entity->IsAlive() || !transform.Exists || !boundingBox.Exists)
			continue;

		rectangle.setSize(boundingBox.Size);
		rectangle.setPosition(transform.Position - boundingBox.HalfSize);
		window.draw(rectangle);
	}
}

void ScenePlay::RenderDebugGrid()
{
	sf::RenderWindow& window = m_GameEngine->GetWindow();

	const sf::Color lineColor(255, 255, 255, 80);
	const float viewCenterX = window.getView().getCenter().x;
	const float viewLeft = viewCenterX - s_WorldWidth / 2;
	const float viewRight = viewCenterX + s_WorldWidth / 2;

	sf::VertexArray lines(sf::PrimitiveType::Lines);

	for (float x = std::floor(viewLeft / s_CellSize) * s_CellSize; x <= viewRight; x += s_CellSize)
	{
		lines.append({Vec2(x, 0), lineColor});
		lines.append({Vec2(x, s_WorldHeight), lineColor});
	}

	// Cells count from the bottom, and the height is not a multiple of a cell.
	for (float y = s_WorldHeight; y >= 0; y -= s_CellSize)
	{
		lines.append({Vec2(viewLeft, y), lineColor});
		lines.append({Vec2(viewRight, y), lineColor});
	}

	window.draw(lines);
}

} // namespace Meche
