#pragma once

namespace Meche
{

class Action
{
public:
	enum class Type
	{
		Begin,
		End,
	};

	enum class Name
	{
		Up,
		Down,
		Left,
		Right,
		Play,
		Quit,
		Shoot,
		Pause,
		ToggleTextures,
		ToggleCollision,
		ToggleGrid,
	};

	Action(Name actionName, Type actionType) : m_Name(actionName), m_Type(actionType) {}

	Name GetName() const { return m_Name; }
	Type GetType() const { return m_Type; }

private:
	Name m_Name;
	Type m_Type;
};

} // namespace Meche