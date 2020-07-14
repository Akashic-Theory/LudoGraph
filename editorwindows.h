#pragma once

#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "segment.h"
#include "global.h"
#include "construct.h"

class SegWindow : public EditorWindow{
	private:
		Segment _s;
		SegmentController* _sel;
		SegmentController* _mov;
		std::set<SegTarget*> _multi;
		bool _drag;
		bool _editing;
		TargetGroupHierarchy _hier;
		sf::RenderWindow& _window;
		sf::Vector2i _last;
	public:
		SegWindow(sf::RenderWindow& win);
		void processEvent(sf::Event& e);
		void preDraw();
		void clear();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
