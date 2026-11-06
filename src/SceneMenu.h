#pragma once

#include "Animation.h"
#include "Scene.h"

#include <array>
#include <cstdint>
#include <string>

namespace Meche
{

class SceneMenu : public Scene
{
public:
	explicit SceneMenu(GameEngine* gameEngine);

	void Update() override;
	void DoAction(const Action& action) override;
	void Render() override;

private:
	void DrawLevelSelector();

	void SelectNextLevel();
	void SelectPreviousLevel();

private:
	std::array<std::string, 2> m_Levels = {"assets/level1.txt", "assets/level2.txt"};
	uint32_t m_SelectedLevel = 0;
};

} // namespace Meche
