#include "GameEngine.h"

#include <print>

int main()
{
	Meche::GameEngine game;

	if (const auto result = game.Init(); !result)
	{
		std::println(stderr, "[ENGINE] {}", result.error());
		return 1;
	}

	game.Run();
}
