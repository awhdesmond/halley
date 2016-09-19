#pragma once

namespace Halley
{
	class InputKeyboard;
	class InputJoystick;
	class InputMouse;
	class InputTouch;

	class InputAPI
	{
	public:
		virtual ~InputAPI() {}
		
		virtual size_t getNumberOfKeyboards() const = 0;
		virtual std::shared_ptr<InputKeyboard> getKeyboard(int id = 0) const = 0;

		virtual size_t getNumberOfJoysticks() const = 0;
		virtual std::shared_ptr<InputJoystick> getJoystick(int id = 0) const = 0;

		virtual size_t getNumberOfMice() const = 0;
		virtual std::shared_ptr<InputMouse> getMouse(int id = 0) const = 0;

		virtual Vector<std::shared_ptr<InputTouch>> getNewTouchEvents() = 0;
		virtual Vector<std::shared_ptr<InputTouch>> getTouchEvents() = 0;
	};
}