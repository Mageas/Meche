#pragma once

#include "Scene.h"

namespace Meche
{

class SceneMenu : public Scene
{
public:
	explicit SceneMenu(GameEngine* gameEngine);

	void Update() override;
	void DoAction(const Action& action) override;
	void Render() override;
};

} // namespace Meche
