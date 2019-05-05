#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <sstream>
#include <tuple>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

//Bad Practice - Refactor  later - from  https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class InspectorData{
	friend class Inspector;
	friend class InspectorControl;
	// Helper Types
	public:
	using data = std::variant<
		bool,
		int,
		float,
		std::string,
		sf::Vector2f
	>;
	enum{
		INS_BOOL,
		INS_INT,
		INS_FLOAT,
		INS_STRING,
		INS_V2F
	}type;
	// Start member definition
	private:
		// map  - string key, holds data and bool representing write permission
		std::unordered_map<std::string, std::tuple<data, bool> > _data;
	public:
		InspectorData();
		bool changeAccess(const std::string key, const bool b);
		bool writable(const std::string key);
		bool insert(const std::string s, data& d, const bool b = true);
		bool exists(const std::string& s);
		template <class T>
		T& access(const std::string& key);
};

//GET THIS OUT OF HEADER FILE LATER  VVVVVVVVVVVVVVVVVVVV
template<class T>
T& InspectorData::access(const std::string& key){
	if(_data.find(key) == _data.end()){
		_data[key] = std::tuple<data, bool>(T(), true);
	}
	return std::get<T>(std::get<0>(_data[key]));
}
//GET THIS OUT OF HEADER FILE LATER  ^^^^^^^^^^^^^^^^^^^^^^^^

class InspectorControl : public sf::Drawable{
	friend class Inspector;
	private:
	sf::Text _tag;
	sf::Text _dataText;
	sf::FloatRect _hitbox;
	InspectorData::data* _data;
	
	InspectorControl(const std::string tag, InspectorData::data* data, const unsigned int fontSize = 16, const bool readonly = false, const sf::Vector2f& offset = sf::Vector2f());
	void updateText();
	bool within(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class Inspector : public sf::Drawable{
	private:
		InspectorData* _data;
		sf::View _view;
		sf::RectangleShape _shape;
		std::string _selected;
		InspectorControl* _activeElement;
		std::stringstream _ss;
		std::vector<InspectorControl*> _elements;
	public:
		Inspector(const sf::Color col = sf::Color(25, 94, 85));
		~Inspector();
		sf::View& getView();
		void generate(InspectorData& iData);
		void clear();
		bool select(const sf::Vector2f& pos, const std::function<void()>& updates = [](){});
		bool select(float x, float y, const std::function<void()>& updates = [](){});
		void updateData();
		void updateView();
		void write(std::string str);
		void delLastChar();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
