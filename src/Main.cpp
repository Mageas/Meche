#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics.hpp>

#include <print>

int main()
{
	sf::RenderWindow window(sf::VideoMode({1280, 720}), "Meche");
	window.setFramerateLimit(60);

	if (!ImGui::SFML::Init(window))
	{
		std::println(stderr, "Unable to Init the window");
		return 1;
	}

	sf::Clock deltaClock;
	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		window.clear();

		ImGui::Begin("Debug");
		ImGui::Text("Test");
		ImGui::End();

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown(window);
}