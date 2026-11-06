#pragma once

#include "Vec2.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cstdint>
#include <optional>
#include <string>

namespace Meche
{

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& name, const sf::Texture& texture, uint32_t frameCount, uint32_t speed);

	const std::string& GetName() const { return m_Name; }
	const Vec2& GetSize() const { return m_FrameSize; }
	const sf::Sprite& GetSprite() const { return m_Sprite.value(); }
	sf::Sprite& GetSprite() { return m_Sprite.value(); }

	bool HasEnded() const { return m_GameFrame >= m_FrameCount * m_Speed; }

	void Update();

private:
	std::string m_Name;
	std::optional<sf::Sprite> m_Sprite;
	uint32_t m_FrameCount = 1;
	uint32_t m_GameFrame = 0;
	uint32_t m_Speed = 1;
	Vec2 m_FrameSize;
};

} // namespace Meche