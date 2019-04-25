#include <sstream>
#include <iostream>

#include "inspector.h"
#include "global.h"

InspectorData::InspectorData():
	_data()
{
	
}

bool InspectorData::insert(const std::string s, data& d){
	if(_data.find(s) == _data.end()){
		_data[s] = d;
		return true;
	}
	return false;
}

template<class T, typename dMember, typename dType>
T& InspectorData::opAccess(const std::string& key){
	if(_data.find(key) == _data.end()){
		_data[key] = data(T());
		return _data[key].dMember;
	}else{
		if(_data[key].type == dType){
			return _data[key].dMember;
		}else{
			_data[key] = data(T());
			return _data[key].dMember;
		}
	}
}

template<>
sf::Vector2f& InspectorData::operator[](const std::string& key){
	return opAccess<sf::Vector2f, v2f, data::INS_V2F>(key);
	//if(_data.find(key) == _data.end()){
	//	_data[key] = data(sf::Vector2f());
	//	return _data[key].v2f;
	//}else{
	//	if(_data[key].type == data::INS_V2F){
	//		return _data[key].v2f;
	//	}else{
	//		_data[key] = data(sf::Vector2f());
	//		return _data[key].v2f;
	//	}
	//}
}

//##############################################################################

InspectorControl::InspectorControl(const std::string tag, InspectorData::data& data):
	_tag(),
	_dataText(),
	_data(&data)
{
	std::stringstream ss;
	switch(_data->type){
		case InspectorData::data::INS_STRING:{
			ss << _data->s;
		}break;
		case InspectorData::data::INS_INT:{
			ss << _data->i;
		}break;
		case InspectorData::data::INS_V2F:{
			ss << '(' << _data->v2f.x << ',' << _data->v2f.y << ')';
		}break;
		default:{}
	}
	
	std::cout << ss.str() << std::endl;
	
	_dataText.setFont(Global::get()._font);
	_tag.setFont(Global::get()._font);
	
	_dataText.setString(ss.str());
	_tag.setString(tag);
	
	sf::FloatRect textBounds = _tag.getLocalBounds();
	sf::Vector2f offset = sf::Vector2f(textBounds.width + 25, 0);
	_dataText.setPosition(offset);
}

void InspectorControl::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_tag, _tag.getTransform() * states.transform);
	target.draw(_dataText, _dataText.getTransform() * states.transform);
}
//##############################################################################

Inspector::Inspector(const sf::RenderWindow& window, const sf::Color col):
	_data(),
	_size(),
	_color(col)
{
	sf::Vector2u winSize = window.getSize();
	_size.x = winSize.x / 5;
	_size.y = winSize.y;
}

void Inspector::generate(InspectorData& iData){
	_elements.clear();
	for(auto& data : iData._data){
		_elements.push_back(InspectorControl(data.first, data.second));
	}
}

void Inspector::clear(){
	_elements.clear();
}

void Inspector::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	sf::RectangleShape shape = sf::RectangleShape();
	sf::Color fill = _color;
	fill.a = 127;
	
	shape.setSize(sf::Vector2f(_size));
	shape.setFillColor(fill);
	shape.setOutlineColor(_color);
	shape.setOutlineThickness(5);
	target.draw(shape);
	
	//Handle drawing of  elements
	sf::Transform offset = sf::Transform::Identity;
	for(auto control : _elements){
		//control.draw(target, sf::RenderStates(sf::Transform().translate(offset)));
		states.transform = offset;
		control.draw(target, states);
		offset.translate(0, control._tag.getLocalBounds().height + 25);
	}
}
