#pragma once

#include <SFML/Graphics.hpp>

class Movable{
	private:
	public:
		virtual void setPos(const float x, const float y) = 0;
		virtual void move(const float x, const float y) = 0;
};

class EditorWindow : public sf::Drawable{
	private:
	protected:
		sf::View _view;
		EditorWindow() : _view(sf::View()){
				_view.reset(sf::FloatRect(0, 0, 720, 720));
				_view.setViewport(sf::FloatRect(0.2f, 0.05f, 0.6f, 0.95f));
		}
	public:
		virtual void clear() = 0;
		virtual void processEvent(sf::Event& e) = 0;
};

class Hierarchy{
	private:
	public:
};
