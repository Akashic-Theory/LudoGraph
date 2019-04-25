#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "construct.h"
#include "inspector.h"

class SegmentController: public Movable{
	protected:
		sf::Sprite _sprite;
		sf::Color _tint;
		InspectorData _data;
		SegmentController(const sf::Color col);
	public:
		virtual ~SegmentController() {}
		virtual sf::FloatRect getGlobalBounds() const;
		virtual void move(const float x, const float y);
		virtual void setPos(const float x, const float y);
		virtual void mark(const bool t);
		virtual InspectorData& getData();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
};

class SegTarget: public sf::Drawable, public SegmentController{
	private:
		static sf::Texture _defSeg;
		sf::VertexArray _line;
	public:
		SegTarget();
		void updateSource(const sf::Vector2f& v);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class Segment : public sf::Drawable, public SegmentController{
	private:
		static sf::Texture _defSeg;
	public:
		Segment();
		
		sf::Vector2f getOrigin() const;
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
