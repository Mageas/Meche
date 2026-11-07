#pragma once

#include "Scene.h"

#include <expected>
#include <string>

namespace Meche
{

class ScenePlay : public Scene
{
public:
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
};

} // namespace Meche
