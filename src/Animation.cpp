#include "Animation.h"

namespace Meche
{

Animation::Animation(const std::string& name, const sf::Texture& texture, uint32_t frameCount, uint32_t speed)
	: m_Name(name), m_Sprite(texture), m_FrameCount(frameCount), m_Speed(speed)
{
	const auto size = texture.getSize();
	m_FrameSize = Vec2(size.x / m_FrameCount, size.y);

	const int sizeX = static_cast<int>(m_FrameSize.X);
	const int sizeY = static_cast<int>(m_FrameSize.Y);

	m_Sprite->setOrigin(m_FrameSize / 2);
	m_Sprite->setTextureRect(sf::IntRect({0, 0}, {sizeX, sizeY}));
}

void Animation::Update()
{
	m_GameFrame++;

	const uint32_t imageNumber = (m_GameFrame / m_Speed) % m_FrameCount;

	const int positionX = static_cast<int>(m_FrameSize.X) * static_cast<int>(imageNumber);
	const int sizeX = static_cast<int>(m_FrameSize.X);
	const int sizeY = static_cast<int>(m_FrameSize.Y);
	m_Sprite->setTextureRect(sf::IntRect({positionX, 0}, {sizeX, sizeY}));
}

} // namespace Meche