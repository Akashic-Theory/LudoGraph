#include "global.h"

Global::Global() : 
	_font(sf::Font()),
	_insp(Inspector())
{
	
}

Global& Global::get(){
	static Global instance;
	return instance;
}
