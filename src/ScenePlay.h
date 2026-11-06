#pragma once

#include "Scene.h"

namespace Meche
{

class ScenePlay : public Scene
{
public:
	explicit ScenePlay(GameEngine* gameEngine);

	void Update() override;
	void DoAction(const Action& action) override;
	void Render() override;
};

} // namespace Meche
