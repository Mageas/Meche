#include "Assets.h"

#include <format>
#include <fstream>
#include <limits>

namespace Meche
{

std::expected<void, std::string> Assets::LoadFromFile(const std::filesystem::path& filePath)
{
	std::ifstream file(filePath);
	std::string keyword;

	if (!file.is_open())
		return std::unexpected(std::format("Unable to open '{}'", filePath.string()));

	while (file >> keyword)
	{
		if (keyword.starts_with('#'))
		{
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		if (keyword == "Texture")
		{
			std::string name;
			std::string path;
			file >> name >> path;

			if (!m_Textures[name].loadFromFile(path))
				return std::unexpected(std::format("Texture '{}' cannot be loaded from '{}'", name, path));
		}
		else if (keyword == "Animation")
		{
			std::string name;
			std::string textureName;
			uint32_t frameCount = 0;
			uint32_t speed = 0;
			file >> name >> textureName >> frameCount >> speed;

			if (frameCount == 0 || speed == 0)
				return std::unexpected(std::format("Animation '{}' needs a frame count and a speed above 0", name));

			if (!m_Textures.contains(textureName))
				return std::unexpected(std::format("Animation '{}' uses the unknown texture '{}'", name, textureName));

			m_Animations[name] = Animation(name, GetTexture(textureName), frameCount, speed);
		}
		else if (keyword == "Font")
		{
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else
		{
			return std::unexpected(std::format("Unknown keyword: {}", keyword));
		}
	}

	return {};
}

} // namespace Meche