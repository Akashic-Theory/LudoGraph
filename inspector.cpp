#include <iostream>

#include "inspector.h"
#include "global.h"

InspectorData::InspectorData():
	_data()
{
	
}

bool InspectorData::changeAccess(const std::string key, const bool b){
	if(_data.find(key) == _data.end()){
		return false;
	}
	std::get<1>(_data[key]) = b;
	return true;
}

bool InspectorData::writable(const std::string key){
	if(_data.find(key) == _data.end()){
		return false;
	}
	return std::get<1>(_data[key]);
}

bool InspectorData::insert(const std::string key, data& d, const bool b){
	if(_data.find(key) == _data.end()){
		_data[key] = std::tuple<data, bool>(d, b);
		return true;
	}
	return false;
}

bool InspectorData::exists(const std::string& key){
	if(_data.find(key) == _data.end()){
		return false;
	}
	return true;
}

//##############################################################################

InspectorControl::InspectorControl(const std::string tag, InspectorData::data* data, const unsigned int fontSize, const bool readonly, const sf::Vector2f& offset):
	_tag(),
	_dataText(),
	_data(data)
{
	_tag.setString(tag);
	
	_tag.setFont(Global::get()._font);
	_dataText.setFont(Global::get()._font);
	
	_tag.setCharacterSize(fontSize);
	_dataText.setCharacterSize(fontSize);
	
	if(readonly){
		_dataText.setOutlineThickness(2);
	}
	
	_tag.setPosition(offset);
	_dataText.setPosition(offset + sf::Vector2f(0, _tag.getLocalBounds().width));
	
	updateText();
}

void InspectorControl::updateText(){
	//std::cout << "Text Update" << std::endl;
	std::stringstream ss = std::stringstream();
	std::visit(overloaded{
		[&](bool arg){
			if(arg){
				ss << "TRUE";
			}else{
				ss << "FALSE";
			}
		},
		[&](int arg){
			ss << arg;
		},
		[&](float arg){
			ss << arg;
		},
		[&](std::string arg){
			ss << arg;
		},
		[&](sf::Vector2f arg){
			ss << '(' << (int)arg.x << ',' << (int)arg.y << ')';
		},
	}, *_data);
	
		_dataText.setString(ss.str());
	sf::FloatRect textBounds = _tag.getLocalBounds();
	_dataText.setPosition(_tag.getPosition());
	_dataText.move(textBounds.width + 25, 0);
}

bool InspectorControl::within(float x, float y){
	// ISSUE - WRONG HITBOXES. ONLY DRAWING IS MOVED, NOT OBJECT
	if(_dataText.getGlobalBounds().contains(x, y)){
		return true;
	}
	return false;
}

void InspectorControl::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_tag, states);
	target.draw(_dataText, states);
	//std::cout << _dataText.getString().toAnsiString() << std::endl;
	
	//Hitbox drawing
	/*
	sf::RectangleShape shape = sf::RectangleShape();
	
	sf::FloatRect bound = _dataText.getLocalBounds();
	sf::Vector2f size = sf::Vector2f(bound.width, bound.height);
	
	shape.setPosition(_dataText.getPosition());
	shape.setSize(sf::Vector2f(size));
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::White);
	shape.setOutlineThickness(1);
	
	target.draw(shape, states);
	*/
}
//##############################################################################

Inspector::Inspector(const sf::Color col):
	_data(),
	_view(),
	_shape(),
	_selected(),
	_activeElement(nullptr),
	_ss()
{
	_view.reset(sf::FloatRect(0, 0, 250, 750));
	_view.setViewport(sf::FloatRect(0.0f, 0.0f, 0.2f, 1.f));
	
	sf::Color fill = col;
	fill.a = 127;
	_shape.setSize(sf::Vector2f(245, 750));
	_shape.setFillColor(fill);
	_shape.setOutlineColor(col);
	_shape.setOutlineThickness(5);
	
}

Inspector::~Inspector(){
	clear();
}

sf::View& Inspector::getView(){
	return _view;
}

void Inspector::generate(InspectorData& iData){
	//std::cout << "generate:" << std::endl;
	_elements.clear();
	_data = &iData;
	sf::Vector2f offset = sf::Vector2f();
	for(auto& [name, data] : iData._data){
		InspectorControl* control = new InspectorControl(name, &(std::get<0>(data)), 14, !std::get<1>(data), offset);
		_elements.push_back(control);
		offset.y += control->_tag.getLocalBounds().height + 10;
	}
}

void Inspector::clear(){
	//std::cout << "clear:" << std::endl;
	for(auto el : _elements){
		delete el;
	}
	_elements.clear();
	_data = nullptr;
}

bool Inspector::select(const sf::Vector2f& pos, const std::function<void()>& updates){
	return select(pos.x, pos.y, updates);
}

bool Inspector::select(float x, float y, const std::function<void()>& updates){
	//std::cout << "Select:" << std::endl;
	if(_activeElement){
		updateData(); //guarantees _ss has been reset
		_selected = "";
		_activeElement = nullptr;
	}
	for(auto& el : _elements){
		if(el->within(x, y) && _data->writable(el->_tag.getString())){
			_selected = el->_tag.getString();
			_activeElement = el;
			
			if(el->_data->index() != InspectorData::INS_BOOL){
				el->_dataText.setString("___");
			}
			
			std::cout << "CLICK" << _elements.size() << std::endl;
			
			//check for bool type
			if(_activeElement->_data->index() == InspectorData::INS_BOOL){
				//std::cout << "BOOL" << _elements.size() << std::endl;
				updateData();
				updates();
			}
			
			return true;
		}
	}
	sf::FloatRect hitbox = sf::FloatRect(sf::Vector2f(), _view.getSize());
	//std::cout 	<< "Viewport: " << std::endl
	//			<< "  X:" << rec.left	<< std::endl
	//			<< "  Y:" << rec.top	<< std::endl
	//			<< "  W:" << rec.width	<< std::endl
	//			<< "  H:" << rec.height	<< std::endl
	//			<< " x- " << x << std::endl
	//			<< " y- " << y << std::endl;
	if(hitbox.contains(x, y)){
		return true;
	}
	return false;
}

void Inspector::updateData(){
	//Guarantee _ss is reset and in a valid state
	//std::cout << "updateData:" << std::endl;
	if(_data == nullptr){
		std::cout << "NULLLLLL!!!!!!!!!!:" << std::endl;
	}else if(_data->_data.find(_selected) != _data->_data.end()){
		//if(std::get<0>(_data->_data[_selected]).index() == 1)
			//std::cout << _ss.str() << " <--- " << std::get<std::string>(_data->_data[_selected]) << std::endl;
		InspectorData::data& handle = std::get<0>(_data->_data[_selected]);
		switch(handle.index()){
			case InspectorData::INS_BOOL:{
				bool& b = std::get<bool>(handle);
				b = !b;
			}break;
			case InspectorData::INS_INT:{
				if(!_ss.str().empty()){
					int i;
					_ss >> i;
					std::get<int>(handle) = i;
				}
		std::cout << "Int Set" << std::endl;
			}break;
			case InspectorData::INS_FLOAT:{
				if(!_ss.str().empty()){
					float f;
					_ss >> f;
					std::get<float>(handle) = f;
				}
			}break;
			case InspectorData::INS_STRING:{
				if(!_ss.str().empty()){
					std::string s;
					_ss >> s;
					std::get<std::string>(handle) = s;
				}
		std::cout << "String Set" << std::endl;
			}break;
			case InspectorData::INS_V2F:{
				if(!_ss.str().empty()){
					float x, y;
					char sep = ',';
					std::cout << _ss.str() << std::endl;
					_ss >> x >> sep >> y;
					//debug
					std::cout << _ss.str() << std::endl;
					std::cout << "x: " << x << " y: " << y << std::endl;
					if(!_ss.fail()){
						std::cout << "VecF set" << std::endl;
						std::get<sf::Vector2f>(handle) = sf::Vector2f(x, y);
					}
				}
			}break;
			
			//DO NOTHING
			default: {}
		}
	}
	_ss = std::stringstream();
	_selected = "";
	_activeElement = nullptr;
	updateView();
	
}

void Inspector::updateView(){
	//std::cout << "updateView:" << std::endl;
	for(auto el : _elements){
		el->updateText();
	}
}

void Inspector::write(std::string str){
	//std::cout << "write:" << std::endl;
	if(_activeElement){
		_ss << str;
		//std::cout << _activeElement->_dataText.getString().toAnsiString() << std::endl;
		_activeElement->_dataText.setString(_ss.str());
		//std::cout << _activeElement->_dataText.getString().toAnsiString() << std::endl;
	}
}

void Inspector::delLastChar(){
	//std::cout << "del:" << std::endl;
	if(_activeElement){
		std::string s = _ss.str();
		s.pop_back();
		_ss = std::stringstream();
		_ss << s;
		_activeElement->_dataText.setString(s);
	}
}

void Inspector::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	sf::View prev = target.getView();
	target.setView(_view);
	
	//std::cout << _elements.size();
	target.draw(_shape);
	for(auto el : _elements){
		el->draw(target, states);
	}
	
	target.setView(prev);
}
