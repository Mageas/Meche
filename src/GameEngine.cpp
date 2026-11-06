#include "GameEngine.h"

#include "Assets.h"
#include "SceneMenu.h"

#include <SFML/System/Clock.hpp>
#include <imgui-SFML.h>

namespace Meche
{

GameEngine::~GameEngine()
{
	if (m_IsImGuiInit)
		ImGui::SFML::Shutdown(m_Window);

	Assets::Get().Clear();
}

std::expected<void, std::string> GameEngine::Init()
{
	if (auto result = Assets::Get().LoadFromFile("assets/assets.txt"); !result)
		return std::unexpected(result.error());

	m_Window.create(sf::VideoMode({1280, 720}), "Meche");
	m_Window.setFramerateLimit(60);
	m_Window.setKeyRepeatEnabled(false);

	if (!ImGui::SFML::Init(m_Window))
		return std::unexpected("Unable to initialize ImGui");

	m_IsImGuiInit = true;

	ChangeScene<SceneMenu>("Menu");

	return {};
}

void GameEngine::Run()
{
	sf::Clock deltaClock;

	while (m_Window.isOpen())
	{
		const ScenePtr& currentScene = GetCurrentScene();

		while (const auto event = m_Window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(m_Window, *event);

			if (event->is<sf::Event::Closed>())
				Quit();

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				DispatchKeyAction(currentScene, keyPressed->scancode, Action::Type::Begin);

			if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
				DispatchKeyAction(currentScene, keyReleased->scancode, Action::Type::End);
		}

		ImGui::SFML::Update(m_Window, deltaClock.restart());
		currentScene->Update();

		m_Window.clear();
		currentScene->Render();
		ImGui::SFML::Render(m_Window);
		m_Window.display();
	}
}

void GameEngine::DispatchKeyAction(const ScenePtr& scene, sf::Keyboard::Scancode scancode, Action::Type type)
{
	const ScancodeActionMap& actionMap = scene->GetActionMap();

	const auto entry = actionMap.find(scancode);
	if (entry == actionMap.end())
		return;

	scene->DoAction(Action(entry->second, type));
}

} // namespace Meche
