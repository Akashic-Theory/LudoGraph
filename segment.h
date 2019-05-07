#pragma once

#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "construct.h"
#include "inspector.h"

class SegTarget;

class SegmentController: public Movable{
	protected:
		sf::Sprite _sprite;
		sf::Color _def;
		sf::Color _tint;
		InspectorData _data;
		SegmentController(const sf::Color col = sf::Color());
		
		//TEMPORARY FOR DEBUG
		sf::Vector2f _snapAmt;
	public:
		virtual ~SegmentController() {}
		virtual sf::FloatRect getGlobalBounds() const;
		virtual sf::Vector2f getSnap();
		virtual void move(const sf::Vector2f& diff);
		virtual void move(const float x, const float y);
		virtual void setPos(const sf::Vector2f& target);
		virtual void setPos(const float x, const float y);
		virtual void snap();
		virtual void setDefCol(const sf::Color& col);
		virtual void setTint(const sf::Color& col);
		virtual void mark(const bool t);
		virtual SegTarget& newTarget(const sf::Vector2f& origin = sf::Vector2f());
		virtual sf::Vector2f getCenter() const;
		virtual InspectorData& getData();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
};

class SegTarget: public sf::Drawable, public SegmentController{
	private:
		static sf::Texture _defSeg;
		sf::VertexArray _line;
	public:
		SegTarget(const sf::Vector2f& origin = sf::Vector2f(), const sf::Vector2f& snapping = sf::Vector2f());
		void updateSource(const sf::Vector2f& v);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class Segment : public sf::Drawable, public SegmentController{
	friend class TargetGroupHierarchy;
	private:
		static sf::Texture _defSeg;
		int _grid;
		std::set<SegTarget*> _targets;
	public:
		Segment(const sf::Vector2f& origin = sf::Vector2f());
		~Segment();
		void updateData();
		void remove(SegTarget* target);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class TargetGroup: public sf::Drawable{
	friend class TargetGroupHierarchy;
	private:
		InspectorData _data;
		std::set<TargetGroup*> _subs;
		std::set<SegTarget*> _elems;
		std::string _str;
		sf::Text _tag;
		std::vector<std::tuple<SegTarget*, sf::Text> > _subTags;
		
		TargetGroup(const std::string& tag, const unsigned int fontSize = 16);
		TargetGroup* select(const float x, const float y, SegTarget*& target);
		float preDraw(const int tier, float offset);
	public:
		~TargetGroup();
		//std::vector<SegTarget*>& 
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class TargetGroupHierarchy : public sf::Drawable{
	private:
		std::unordered_map<SegTarget*, std::set<TargetGroup*> > _managed;
		std::set<TargetGroup*> _groups;
		std::set<TargetGroup*> _roots;
		std::set<SegTarget*> _activeTargets;
		std::set<TargetGroup*> _activeGroups;
		sf::RectangleShape _shape;
		sf::View _view;
		TargetGroup _dump;
		float _scroll;
	public:
		TargetGroupHierarchy(const sf::Color& col = sf::Color(127, 127, 127));
		~TargetGroupHierarchy();
		//sf::RectangleShape& 
		bool select(const sf::Vector2f& pos, const bool additive = false, const std::function<void(InspectorData*)>& processData = [](InspectorData*){});
		bool select(const float x, const float y, const bool additive = false, const std::function<void(InspectorData*)>& processData = [](InspectorData*){});
		void regroup(const sf::Vector2f& pos, std::set<SegTarget*>& targets);
		void newGroup(const std::string& tag = "New Group");
		SegTarget& newTarget(SegTarget* target);
		void deleteSelectedGroups();
		void remove(SegTarget* target);
		const std::set<SegTarget*>& getActive();
		std::vector<SegTarget*> getAllTargets();
		sf::View& getView();
		void scroll(float factor);
		void rebuildActive();
		void preDraw();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		
};
