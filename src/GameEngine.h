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

	template <typename T, typename... Args>
	[[nodiscard]] std::expected<void, std::string> ChangeScene(const std::string& name, Args&&... args)
	{
		auto scene = std::make_shared<T>(this, std::forward<Args>(args)...);
		if (const auto result = scene->Init(); !result)
			return std::unexpected(result.error());

		m_Scenes[name] = std::move(scene);
		m_CurrentScene = name;

		return {};
	}

	sf::RenderWindow& GetWindow() { return m_Window; }

	void Quit() { m_Window.close(); }

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
