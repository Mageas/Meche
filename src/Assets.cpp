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
			std::string name, path;
			file >> name >> path;

			if (!m_Textures[name].loadFromFile(path))
				return std::unexpected(std::format("Unable to load texture '{}' from '{}'", name, path));
		}
		else if (keyword == "Animation")
		{
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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