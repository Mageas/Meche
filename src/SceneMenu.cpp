#include "SceneMenu.h"

#include "Assets.h"
#include "GameEngine.h"
#include "ScenePlay.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <filesystem>
#include <print>

namespace Meche
{

namespace
{

void DrawCenteredText(sf::RenderWindow& window, const std::string& string, unsigned int characterSize, float y,
					  sf::Color color)
{
	sf::Text text(Assets::Get().GetFont("Arial"), string, characterSize);
	text.setFillColor(color);
	text.setOrigin(text.getLocalBounds().getCenter());
	text.setPosition({window.getView().getCenter().x, y});
	window.draw(text);
}

} // namespace

SceneMenu::SceneMenu(GameEngine* gameEngine) : Scene(gameEngine)
{
	RegisterAction(sf::Keyboard::Scancode::Escape, Action::Name::Quit);
	RegisterAction(sf::Keyboard::Scancode::W, Action::Name::Up);
	RegisterAction(sf::Keyboard::Scancode::S, Action::Name::Down);
	RegisterAction(sf::Keyboard::Scancode::D, Action::Name::Play);

	EntityPtr entity = m_EntityManager.AddEntity(Entity::Tag::Player);
	entity->AddComponent<AnimationComponent>(Assets::Get().GetAnimation("PigRun"), true);
}

void SceneMenu::Update()
{
	m_EntityManager.Update();

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		if (entity->IsAlive() && entity->HasComponent<AnimationComponent>())
			entity->GetComponent<AnimationComponent>().Current.Update();
	}

	if (m_ErrorFramesLeft > 0)
		m_ErrorFramesLeft--;
}

void SceneMenu::DoAction(const Action& action)
{
	if (action.GetType() == Action::Type::Begin)
	{
		switch (action.GetName())
		{
		case Action::Name::Quit:
			m_GameEngine->Quit();
			break;
		case Action::Name::Up:
			SelectPreviousLevel();
			break;
		case Action::Name::Down:
			SelectNextLevel();
			break;
		case Action::Name::Play:
			if (const auto result = m_GameEngine->ChangeScene<ScenePlay>("Game", m_Levels[m_SelectedLevel]); !result)
			{
				m_ErrorMessage = result.error();
				m_ErrorFramesLeft = s_ErrorDisplayFrames;
				std::println(stderr, "[MENU] {}", m_ErrorMessage);
			}
			break;
		default:
			break;
		}
	}
}

void SceneMenu::Render()
{
	m_GameEngine->GetWindow().setView(m_GameEngine->GetWindow().getDefaultView());

	m_GameEngine->GetWindow().clear(sf::Color(10, 20, 60));

	for (const auto& entity : m_EntityManager.GetEntities())
	{
		if (entity->IsAlive() && entity->HasComponent<AnimationComponent>())
		{
			auto& sprite = entity->GetComponent<AnimationComponent>().Current.GetSprite();

			sprite.setPosition(Vec2(640, 250));
			sprite.setScale(Vec2(6, 6));
			m_GameEngine->GetWindow().draw(sprite);
		}
	}

	if (m_ErrorFramesLeft > 0)
		DrawCenteredText(m_GameEngine->GetWindow(), m_ErrorMessage, 24, 375.0f, sf::Color(230, 90, 90));

	DrawLevelSelector();
}

void SceneMenu::DrawLevelSelector()
{
	constexpr sf::Color titleColor(255, 222, 173);
	constexpr sf::Color selectedColor(255, 255, 255);
	constexpr sf::Color idleColor(140, 150, 190);
	constexpr sf::Color highlightColor(60, 80, 150);

	constexpr float firstRowY = 430.0f;
	constexpr float rowHeight = 56.0f;
	constexpr sf::Vector2f highlightSize(420.0f, 48.0f);

	sf::RenderWindow& window = m_GameEngine->GetWindow();

	DrawCenteredText(window, "MECHE", 84, 90.0f, titleColor);

	sf::RectangleShape highlight(highlightSize);
	highlight.setOrigin(highlightSize / 2.0f);
	highlight.setFillColor(highlightColor);
	highlight.setPosition(
		{window.getView().getCenter().x, firstRowY + rowHeight * static_cast<float>(m_SelectedLevel)});
	window.draw(highlight);

	for (uint32_t i = 0; i < m_Levels.size(); i++)
	{
		const std::string name = std::filesystem::path(m_Levels[i]).stem().string();
		const bool isSelected = i == m_SelectedLevel;
		const float y = firstRowY + rowHeight * static_cast<float>(i);

		DrawCenteredText(window, isSelected ? "> " + name + " <" : name, 32, y, isSelected ? selectedColor : idleColor);
	}

	DrawCenteredText(window, "W / S : choisir     D : jouer     Echap : quitter", 22, 670.0f, idleColor);
}

void SceneMenu::SelectNextLevel() { m_SelectedLevel = (m_SelectedLevel + 1) % m_Levels.size(); }

void SceneMenu::SelectPreviousLevel() { m_SelectedLevel = (m_SelectedLevel + m_Levels.size() - 1) % m_Levels.size(); }

} // namespace Meche