#pragma once

#include "Scene.h"

#include <string>

namespace Meche
{

class ScenePlay : public Scene
{
public:
	explicit ScenePlay(GameEngine* gameEngine, std::string levelPath);

	void Update() override;
	void DoAction(const Action& action) override;
	void Render() override;

private:
	std::string m_LevelPath;
};

} // namespace Meche
