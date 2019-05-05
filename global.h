#pragma once

#include <SFML/Graphics.hpp>

#include "inspector.h"

class Global{
	private:
		static Global* _instance;
		
		Global();
		Global(Global const& copy);
		Global& operator=(Global const& copy);
		
	public:
		sf::Font _font;
		Inspector _insp;
		
		static Global& get();
};
