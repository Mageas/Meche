#pragma once

#include "Scene.h"

#include <expected>
#include <string>

namespace Meche
{

class ScenePlay : public Scene
{
public:
	struct BombConfig
	{
		std::string AnimationName;
		Vec2 CollisionSize;
		Vec2 ThrowSpeed;
		uint32_t FuseFrames = 0;
		Vec2 BlastSize;
	};

	struct PlayerConfig
	{
		Vec2 SpawnCell;
		Vec2 CollisionSize;
		float RunSpeed = 0.0f;
		float JumpSpeed = 0.0f;
		float MaxSpeed = 0.0f;
		float Gravity = 0.0f;
	};

	explicit ScenePlay(GameEngine* gameEngine, std::string levelPath);

	[[nodiscard]] std::expected<void, std::string> Init() override;

	void Update() override;
	void DoAction(const Action& action) override;
	void Render() override;

	[[nodiscard]] std::expected<void, std::string> LoadLevel();

	Vec2 GridToMidPixel(const Vec2& position, const Vec2& size) const;

private:
	static constexpr float s_CellSize = 32.0f;
	static constexpr float s_WorldHeight = 360.0f;
	static constexpr float s_WorldWidth = 640.0f;

	std::string m_LevelPath;

	BombConfig m_BombConfig;
	PlayerConfig m_PlayerConfig;
};

} // namespace Meche
