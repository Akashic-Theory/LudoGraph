#include <iostream>
#include <cmath>

#include "segment.h"
#include "global.h"

static bool init_Seg = false;
sf::Texture Segment::_defSeg = sf::Texture();

static bool init_Target = false;
sf::Texture SegTarget::_defSeg = sf::Texture();

SegmentController::SegmentController(const sf::Color col):
	_sprite(sf::Sprite()),
	_def(sf::Color::White),
	_tint(col),
	_snapAmt()
	//_data()
{
	auto rec = _sprite.getLocalBounds();
	_sprite.setOrigin(_sprite.getOrigin() + sf::Vector2f(rec.width / 2, rec.height / 2));
	
	InspectorData::data name = InspectorData::data((std::string)"Test");
	InspectorData::data pos = InspectorData::data(_sprite.getPosition());
	InspectorData::data b = InspectorData::data((bool)true);
	InspectorData::data snapDist = InspectorData::data(sf::Vector2f(64, 64));
	_data.insert("Snap Amt", snapDist);
	_data.insert("Snap", b);
	_data.insert("Pos", pos, false);
	_data.insert("Name", name);
}

sf::FloatRect SegmentController::getGlobalBounds() const{
	return _sprite.getGlobalBounds();
}

sf::Vector2f SegmentController::getSnap(){
	return _data.access<sf::Vector2f>("Snap Amt");
}

void SegmentController::move(const sf::Vector2f& diff){
	move(diff.x, diff.y);
}

void SegmentController::move(const float x, const float y){
	_sprite.move(x, y);
	_data.access<sf::Vector2f>("Pos") = getCenter();
}

void SegmentController::setPos(const sf::Vector2f& target){
	setPos(target.x, target.y);
}
	
void SegmentController::setPos(const float x, const float y){
	auto rec = _sprite.getLocalBounds();
	_sprite.setPosition(x - rec.width / 2, y - rec.height / 2);
	_data.access<sf::Vector2f>("Pos") = getCenter();
}

void SegmentController::snap(){
	if(_data.access<bool>("Snap") == false){
		return;
	}
	auto rec = _sprite.getLocalBounds();
	sf::Vector2f& snap = _data.access<sf::Vector2f>("Snap Amt");
	sf::Vector2f curPos = getCenter();
	sf::Vector2f pos = curPos;
	if(snap.x > 0){
		pos.x = (snap.x * round(curPos.x / snap.x));
	}
	if(snap.y > 0){
		pos.y = (snap.y * round(curPos.y / snap.y));
	}
	setPos(pos);
	
	_snapAmt = snap;
}

void SegmentController::setDefCol(const sf::Color& col){
	_def = col;
}

void SegmentController::setTint(const sf::Color& col){
	_tint = col;
}

void SegmentController::mark(const bool t){
	if(t){
		_sprite.setColor(_tint);
	}else{
		_sprite.setColor(_def);
	}
}

sf::Vector2f SegmentController::getCenter() const{
	auto rec = _sprite.getLocalBounds();
	return _sprite.getPosition() + sf::Vector2f(rec.width / 2, rec.height / 2);
}

InspectorData& SegmentController::getData(){
	return _data;
}

//##############################################################################

SegTarget::SegTarget(const sf::Vector2f& origin, const sf::Vector2f& snapping):
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
	auto rec = _sprite.getLocalBounds();
	_sprite.setPosition(origin - sf::Vector2f(rec.width / 2, rec.height / 2));
	_data.access<sf::Vector2f>("Pos") = getCenter();
	_data.access<sf::Vector2f>("Snap Amt") = snapping;
}

void SegTarget::updateSource(const sf::Vector2f& v){
	auto rec = _sprite.getLocalBounds();
	_line[0].position = getCenter();
	_line[1].position = v;
}

void SegTarget::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_sprite);
	target.draw(_line);
}

//##############################################################################

Segment::Segment(const sf::Vector2f& origin):
	SegmentController(sf::Color::Magenta),
	_grid(0)
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
	_sprite.setPosition(origin);
	_data.access<sf::Vector2f>("Pos") = getCenter();
	
	InspectorData::data gridSize = InspectorData::data((int)_grid);
	_data.insert("Grid Size", gridSize);
}

void Segment::updateData(){
	_grid = _data.access<int>("Grid Size");
}

void Segment::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_sprite);
	
	if(_grid > 0){
		sf::Vector2f pos = _sprite.getPosition();
		sf::Sprite gSprite = sf::Sprite(_sprite);
		sf::Color col = sf::Color::White;
		auto rec = _sprite.getLocalBounds();
		for(int i = -_grid; i < _grid + 1; i++){
			for(int j = -_grid; j < _grid + 1; j++){
				int max = abs(i) < abs(j) ? abs(j) : abs(i);
				col.a = 100 - (int)(max / (float)_grid * 50);
				gSprite.setColor(col);
				gSprite.setPosition(pos + sf::Vector2f(i * rec.width, j * rec.height));
				target.draw(gSprite);
			}
		}
	}	
}

//##############################################################################

TargetGroup::TargetGroup(const std::string& tag, const unsigned int fontSize) :
	_data(),
	_subs(),
	_elems(),
	_tag(),
	_subTags()
{
	_tag.setString(tag);
	_tag.setFont(Global::get()._font);
	_tag.setCharacterSize(fontSize);
	
	InspectorData::data name = InspectorData::data(tag);
	InspectorData::data expand = InspectorData::data(true);
	
	_data.insert("Name", name);
	_data.insert("Expand", expand);
	
}

TargetGroup::~TargetGroup(){

}

TargetGroup* TargetGroup::select(const float x, const float y, SegTarget*& target){
	if(_tag.getGlobalBounds().contains(x, y)){
		return this;
	}
	for(auto& [sgt, tag] : _subTags){
		if(tag.getGlobalBounds().contains(x, y)){
			target = sgt;
			return this;
		}
	}
	return nullptr;
}

float TargetGroup::preDraw(const int tier, float offset){
	_tag.setPosition(10.0 * tier + 5, offset);
	offset += _tag.getLocalBounds().height + 5;
	if(!_data.access<bool>("Expand")){
		_subTags.clear();
		_tag.setString("[+]" + _data.access<std::string>("Name"));
		return offset;
	}else{
		_tag.setString("[-]" + _data.access<std::string>("Name"));
		//offset += (_tag.getLocalBounds().height + 5) * _elems.size();
		
		_subTags.clear();
		sf::Text text = _tag;
		text.move(10, 0);
		for(auto el : _elems){
			text.move(0, text.getLocalBounds().height + 5);
			text.setString(el->getData().access<std::string>("Name"));
			offset += text.getLocalBounds().height + 5;
			_subTags.push_back(std::tuple(el, text));
		}
		for(auto sub : _subs){
			offset = sub->preDraw(tier + 1, offset);
		}
		return offset;
	}
}

void TargetGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(_tag, states);
	
	for(auto& [t, el] : _subTags){
		target.draw(el, states);
	}
	
	//sf::Text text = _tag;
	//text.move(10, 0);
	//for(auto el : _elems){
	//	text.move(0, text.getLocalBounds().height + 5);
	//	text.setString(el->getData().access<std::string>("Name"));
	//	target.draw(text);
	//}
}

//##############################################################################

TargetGroupHierarchy::TargetGroupHierarchy(const sf::Color& col) :
	_managed(),
	_groups(),
	_roots(),
	_activeTargets(),
	_activeGroups(),
	_shape(),
	_view(),
	_dump("Ungrouped")
{
	_dump._data.changeAccess("Name", false);
	
	_view.reset(sf::FloatRect(0, 0, 250, 750));
	_view.setViewport(sf::FloatRect(0.8f, 0.0f, 0.2f, 1.f));
	
	sf::Color fill = col;
	fill.a = 127;
	_shape.move(5, 0);
	_shape.setSize(sf::Vector2f(245, 750));
	_shape.setFillColor(fill);
	_shape.setOutlineColor(col);
	_shape.setOutlineThickness(5);
	
	rebuildActive();
	
	preDraw();
}

TargetGroupHierarchy::~TargetGroupHierarchy(){
	for(auto& [target, group] : _managed){
		delete target;
	}
	for(auto target : _dump._elems){
		delete target;
	}
	for(auto group : _groups){
		delete group;
	}
}

bool TargetGroupHierarchy::select(const sf::Vector2f& pos, const bool additive, const std::function<void(InspectorData* dat)>& processData){
	return select(pos.x, pos.y, additive, processData);
}

bool TargetGroupHierarchy::select(const float x, float y, const bool additive, const std::function<void(InspectorData* dat)>& processData){
	//y += _scroll;
	sf::FloatRect hitbox = sf::FloatRect(sf::Vector2f(0, _scroll), _view.getSize());
	//std::cout 	<< "Viewport: " << std::endl
	//			<< "  X:" << hitbox.left	<< std::endl
	//			<< "  Y:" << hitbox.top		<< std::endl
	//			<< "  W:" << hitbox.width	<< std::endl
	//			<< "  H:" << hitbox.height	<< std::endl
	//			<< " x- " << x << std::endl
	//			<< " y- " << y << std::endl;
	if(hitbox.contains(x,y)){
		std::cout << "HIERARCHY CLICK" << std::endl;
		if(!additive){
			_activeGroups.clear();
		}
		SegTarget* target = nullptr;
		TargetGroup* sel = _dump.select(x, y, target);
		if(!sel){
			for(auto group : _roots){
				sel = group->select(x, y, target);
				if(sel){
					break;
				}
			}			
		}
		if(sel){
			_activeGroups.insert(sel);
			if(target){
				processData(&(target->getData()));
			}else{
				processData(&(sel->_data));
			}
		}
		rebuildActive();
		return true;
	}
	return false;
}

void TargetGroupHierarchy::regroup(const sf::Vector2f& pos, std::set<SegTarget*>& targets){
	SegTarget* temp = nullptr;
	TargetGroup* sel = _dump.select(pos.x, pos.y, temp);
	if(!sel){
		for(auto group : _roots){
			sel = group->select(pos.x, pos.y, temp);
			if(sel){
				break;
			}
		}			
	}
	if(sel == &_dump){
		return;
	}
	if(sel){
		if(temp){
			std::cout << "REMOVE!" << std::endl;
			if(_managed.find(temp) != _managed.end()){
				_managed[temp].erase(sel);
				sel->_elems.erase(temp);
				if(_managed[temp].empty()){
					_dump._elems.insert(temp);
				}
			}
		}else{
			for(auto t : targets){
				std::cout << "added!" << std::endl;
				_managed[t].insert(sel);
				sel->_elems.insert(t);
				_dump._elems.erase(t);
			}
		}
		rebuildActive();
		preDraw();
	}
}

SegTarget& TargetGroupHierarchy::newTarget(const sf::Vector2f& origin, const sf::Vector2f& snapping){
	SegTarget* seg = new SegTarget(origin, snapping);
	_managed[seg] = std::set<TargetGroup*>();
	
	_dump._elems.insert(seg);
	rebuildActive();
	preDraw();
	return *seg;
}

void TargetGroupHierarchy::newGroup(const std::string& tag){
	TargetGroup* group = new TargetGroup(tag);
	_groups.insert(group);
	_roots.insert(group);
	preDraw();
}

void TargetGroupHierarchy::deleteSelectedGroups(){
	for(auto group : _activeGroups){
		if(group != &_dump){
			for(auto target : group->_elems){
				_managed[target].erase(group);
				if(_managed[target].empty()){
					_dump._elems.insert(target);
				}
			}
			_groups.erase(group);
			std::cout << "Removed: " << _roots.erase(group) << std::endl;
			delete group;
		}
	}
	_activeGroups.clear();
	rebuildActive();
	preDraw();
}

void TargetGroupHierarchy::remove(SegTarget* target){
	if(_managed.find(target) != _managed.end()){
		for(auto group : _managed[target]){
			group->_elems.erase(target);
		}
		_managed.erase(_managed.find(target));
	}
	_dump._elems.erase(target);
	delete target;
	
	rebuildActive();
	preDraw();
}

const std::set<SegTarget*>& TargetGroupHierarchy::getActive(){
	return _activeTargets;
}

std::vector<SegTarget*> TargetGroupHierarchy::getAllTargets(){
	std::vector<SegTarget*> rv = std::vector<SegTarget*>();
	std::cout << "Start tally:";
	for(auto& t : _managed){
		rv.push_back(t.first);
		std::cout << 1;
	}
	for(auto& t : _dump._elems){
		rv.push_back(t);
		std::cout << 1;
	}
		std::cout << std::endl;
	return rv;
}

sf::View& TargetGroupHierarchy::getView(){
	return _view;
}

void TargetGroupHierarchy::scroll(float factor){
	_scroll -= factor;
	preDraw();
	_view.reset(sf::FloatRect(0, _scroll, 250, 750));
	//_view.move(0, _scroll);
	std::cout << _scroll << std::endl;
}

void TargetGroupHierarchy::rebuildActive(){
	_activeTargets.clear();
	if(_activeGroups.empty()){
		_activeGroups.insert(&_dump);
	}
	std::cout << "Managed: " << _managed.size() << std::endl;
	for(auto it : _managed){
		it.first->updateSource(it.first->getCenter());
	}
	for(auto group : _activeGroups){
		for(auto target : group->_elems){
			_activeTargets.insert(target);
		}
	}
	std::cout << "Active Targets: " << _activeTargets.size() << std::endl;
}

void TargetGroupHierarchy::preDraw(){
	float offset = 0.0;
	//Draw Hierarchy objects
	for(auto root : _roots){
		offset = root->preDraw(0, offset);
	}
	_dump.preDraw(0, offset);
	if(_scroll > offset - 250){
		_scroll = offset - 250;
	}
	if(_scroll < 0){
		_scroll = 0;
	}
}

void TargetGroupHierarchy::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	sf::Transform trans = sf::Transform().translate(0, _scroll);
	sf::View prev = target.getView();
	target.setView(_view);
	target.draw(_shape, trans);
	
	sf::RectangleShape backdrop = sf::RectangleShape();
	backdrop.setFillColor(sf::Color(127, 127, 127, 63));
	backdrop.setSize(sf::Vector2f(_shape.getSize().x, _dump._tag.getLocalBounds().height));
	
	for(auto group : _activeGroups){
		if(_activeGroups.count(group)){
			backdrop.setPosition(group->_tag.getPosition());
			target.draw(backdrop);
		}
	}
	for(auto& group : _groups){
		group->draw(target, states);
	}
	_dump.draw(target, states);
	
	target.setView(prev);
	
	//Draw managed Targets
	for(auto& it : _managed){
		it.first->draw(target, states);
	}
	for(auto t : _dump._elems){
		t->draw(target, states);
	}
}
