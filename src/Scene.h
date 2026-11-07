#pragma once

#include "Action.h"
#include "EntityManager.h"

#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <expected>
#include <map>
#include <string>

namespace Meche
{

using ScancodeActionMap = std::map<sf::Keyboard::Scancode, Action::Name>;

class GameEngine;

class Scene
{
public:
	virtual ~Scene() = default;

	[[nodiscard]] virtual std::expected<void, std::string> Init() { return {}; }

	virtual void Update() = 0;
	virtual void DoAction(const Action& action) = 0;
	virtual void Render() = 0;

	void RegisterAction(sf::Keyboard::Scancode scancode, Action::Name name) { m_ActionMap[scancode] = name; }

	const ScancodeActionMap& GetActionMap() const { return m_ActionMap; }

protected:
	explicit Scene(GameEngine* gameEngine) : m_GameEngine(gameEngine) {}

protected:
	GameEngine* m_GameEngine;
	EntityManager m_EntityManager;
	uint32_t m_FrameCount = 0;
	ScancodeActionMap m_ActionMap;
	bool m_IsPaused = false;
};

} // namespace Meche
