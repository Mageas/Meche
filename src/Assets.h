#pragma once

#include "Animation.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <expected>
#include <filesystem>
#include <map>
#include <string>

namespace Meche
{

class Assets
{
public:
	static Assets& Get()
	{
		static Assets instance;
		return instance;
	}

	Assets(const Assets& other) = delete;
	Assets& operator=(const Assets& other) = delete;

	void Clear()
	{
		m_Animations.clear();
		m_Textures.clear();
		m_Fonts.clear();
	}

	const sf::Texture& GetTexture(const std::string& name) const { return m_Textures.at(name); }
	const Animation& GetAnimation(const std::string& name) const { return m_Animations.at(name); }
	const sf::Font& GetFont(const std::string& name) const { return m_Fonts.at(name); }

	[[nodiscard]] std::expected<void, std::string> LoadFromFile(const std::filesystem::path& filePath);

private:
	Assets() = default;

private:
	std::map<std::string, sf::Texture> m_Textures;
	std::map<std::string, Animation> m_Animations;
	std::map<std::string, sf::Font> m_Fonts;
};

} // namespace Meche