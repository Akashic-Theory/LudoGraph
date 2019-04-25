#pragma once

#include <SFML/Graphics.hpp>

class Global{
	private:
		static Global* _instance;
		
		Global();
		Global(Global const& copy);
		Global& operator=(Global const& copy);
		
	public:
		sf::Font _font;
		
		static Global& get();
};
