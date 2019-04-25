#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class InspectorData{
	friend class Inspector;
	friend class InspectorControl;
	// Helper Types
	public:
	struct data{
		enum{
			INS_INT,
			INS_STRING,
			INS_V2F
		}type;
		
		union {
			int i;
			std::string s;
			sf::Vector2f v2f;
			
			//U(){}
			//U(const std::string& str) : s(str){}
			//U& operator=(const std::string str) {new(&s) std::string(str); return *this;}
		};
		
		data() : s(){}
		~data(){}
		data(const data& oth) : type(oth.type){
			switch(type){
				case INS_INT: i = oth.i; break;
				case INS_STRING: s = oth.s; break;
				case INS_V2F: v2f = oth.v2f; break;
			}
		}
		data& operator=(const data& oth){
			type = oth.type;
			switch(type){
				case INS_INT: i = oth.i; break;
				case INS_STRING: s = oth.s; break;
				case INS_V2F: v2f = oth.v2f; break;
			}
			return *this;
		}
		
		//Specialized initializations
		data(int i) : i(i), type(INS_INT){}
		data(std::string s) : s(s), type(INS_STRING){}
		data(sf::Vector2f v) : v2f(v), type(INS_V2F){}
		
		///data() : type(INS_STRING), s(){s = new std::string("");}
		///data(const data& d) : type(d.type){
		///	switch(type){
		///		case INS_INT: i = d.i; break;
		///		case INS_STRING:{
		///			s = new std::string(*(d.s));
		///		}
		///	}
		///}
		///
		///~data(){
		///	using std::string;
		///	switch(type){
		///		case INS_STRING: delete s; return;
		///		default: return;
		///	}
		///}
		
	};
	// Start member definition
	private:
		std::unordered_map<std::string, data> _data;
		template<class T, typename dMember, typename dType>
		T& opAccess(const std::string& key);
	public:
		InspectorData();
		bool insert(const std::string s, data& d);
		template <class T>
		T& operator[](const std::string& key);
};

class InspectorControl : public sf::Drawable{
	friend class Inspector;
	private:
	sf::Text _tag;
	sf::Text _dataText;
	InspectorData::data* _data;
	
	InspectorControl(const std::string tag, InspectorData::data& data);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class Inspector : public sf::Drawable{
	private:
		InspectorData* _data;
		sf::Vector2u _size;
		sf::Color _color;
		std::vector<InspectorControl> _elements;
	public:
		Inspector(const sf::RenderWindow& window, const sf::Color col = sf::Color(25, 94, 85));
		~Inspector(){};
		void generate(InspectorData& iData);
		void clear();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
