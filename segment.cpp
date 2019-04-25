#include <iostream>

#include "segment.h"

static bool init_Seg = false;
sf::Texture Segment::_defSeg = sf::Texture();

static bool init_Target = false;
sf::Texture SegTarget::_defSeg = sf::Texture();

SegmentController::SegmentController(const sf::Color col):
	_sprite(sf::Sprite()),
	_tint(col)
	//_data()
{
	InspectorData::data name = InspectorData::data("Test");
	InspectorData::data pos = InspectorData::data(_sprite.getPosition());
	_data.insert("Pos", pos);
	_data.insert("Name", name);
	//std::cout << "inserted " << d.s << std::endl;
}

sf::FloatRect SegmentController::getGlobalBounds() const{
	return _sprite.getGlobalBounds();
}

void SegmentController::move(const float x, const float y){
	_sprite.move(x, y);
}

void SegmentController::setPos(const float x, const float y){
	_sprite.setPosition(x, y);
}

void SegmentController::mark(const bool t){
	if(t){
		_sprite.setColor(_tint);
	}else{
		_sprite.setColor(sf::Color::White);
	}
}

InspectorData& SegmentController::getData(){
	return _data;
}

//##############################################################################

SegTarget::SegTarget():
	SegmentController(sf::Color::Magenta),
	_line(sf::VertexArray(sf::Lines, 2))
{
	std::cout << "New Sprite" << std::endl;
	if(!init_Target){
		if(!_defSeg.loadFromFile("res/target.png")){
			sf::err() << "Failed to load default texture for Segments" << std::endl;
			exit(1);
		}
		std::cout << "Loaded fallback texture for Sprites!" << std::endl;
		init_Target = true;
	}
	
	_sprite.setTexture(_defSeg);

}

void SegTarget::updateSource(const sf::Vector2f& v){
	auto rec = _sprite.getLocalBounds();
	_line[0].position = _sprite.getPosition() + sf::Vector2f(rec.width / 2, rec.height / 2);
	_line[1].position = v;
}

void SegTarget::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_sprite);
	target.draw(_line);
}

//##############################################################################

Segment::Segment():
	SegmentController(sf::Color::Magenta)
{
	std::cout << "New Sprite" << std::endl;
	if(!init_Seg){
		if(!_defSeg.loadFromFile("res/defTile.png")){
			sf::err() << "Failed to load default texture for Segments" << std::endl;
			exit(1);
		}
		std::cout << "Loaded fallback texture for Sprites!" << std::endl;
		init_Seg = true;
	}
	_sprite.setTexture(_defSeg);
}

sf::Vector2f Segment::getOrigin() const{
	auto rec = _sprite.getLocalBounds();
	return _sprite.getPosition() + sf::Vector2f(rec.width / 2, rec.height / 2);
}

void Segment::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_sprite);
}