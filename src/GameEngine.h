#pragma once

#include "Scene.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <expected>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace Meche
{

using ScenePtr = std::shared_ptr<Scene>;
using ScenesByName = std::map<std::string, ScenePtr>;

class GameEngine
{
public:
	~GameEngine();

	[[nodiscard]] std::expected<void, std::string> Init();
	void Run();

	template <typename T, typename... Args> void ChangeScene(const std::string& name, Args&&... args)
	{
		m_Scenes[name] = std::make_shared<T>(this, std::forward<Args>(args)...);
		m_CurrentScene = name;
	}

	sf::RenderWindow& GetWindow() { return m_Window; }

private:
	const ScenePtr& GetCurrentScene() { return m_Scenes.at(m_CurrentScene); }

	void DispatchKeyAction(const ScenePtr& scene, sf::Keyboard::Scancode scancode, Action::Type type);

private:
	sf::RenderWindow m_Window;
	ScenesByName m_Scenes;
	std::string m_CurrentScene;
	bool m_IsImGuiInit = false;
};

} // namespace Meche
