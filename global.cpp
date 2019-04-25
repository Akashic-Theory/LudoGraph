#include "global.h"

Global::Global() : _font(sf::Font()) {}

Global& Global::get(){
	static Global instance;
	return instance;
}
