#include "GameEngine.h"

#include <string>
#include <expected>
#include <print>

int main()
{
	Meche::GameEngine game;

	if (const std::expected<void, std::string> result = game.Init(); !result)
	{
		std::println(stderr, "[ENGINE] {}", result.error());
		return 1;
	}

	game.Run();
}