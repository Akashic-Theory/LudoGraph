#include "editorwindows.h"

SegWindow::SegWindow(sf::RenderWindow& win) :
	EditorWindow(),
	_s(),
	_sel(nullptr),
	_mov(nullptr),
	_multi(std::set<SegTarget*>()),
	_drag(false),
	_editing(false),
	_hier(),
	_window(win),
	_last()
{
	
}

void SegWindow::processEvent(sf::Event& e){
	Inspector& insp = Global::get()._insp;
	sf::Vector2i mPos = sf::Mouse::getPosition(_window);
	switch(e.type){
		case sf::Event::MouseButtonPressed:{
			switch(e.mouseButton.button){
				case sf::Mouse::Button::Left:{
					_mov = nullptr;
					if(_editing){
						insp.updateData();
						_s.updateData();
						_hier.preDraw();
					}
					if(insp.select(_window.mapPixelToCoords(mPos, insp.getView()), [&](){
							_s.updateData();
							_hier.preDraw();
						}) 
						|| _hier.select(_window.mapPixelToCoords(mPos, _hier.getView()), sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl), [&](InspectorData* dat){
							insp.generate(*dat);
						})){
						_editing = true;
					}else{
						_editing = false;
						if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)){
							if(_sel){
								for(auto t : _multi){
									t->mark(false);
								}
								_sel->mark(false);
								_sel = nullptr;
							}
							_multi.clear();
						}
					}
					if(!_editing){
						//Main View Click
						sf::Vector2f mPosView = _window.mapPixelToCoords(mPos, _view);
						for(auto t : _hier.getActive()){
							if(t->getGlobalBounds().contains(mPosView)){
								_mov = t;
							}
						}
						if(_mov == nullptr && _s.getGlobalBounds().contains(mPosView)){
							_mov = &_s;
						}
						if(_mov){
							_mov->mark(true);
							insp.generate(_mov->getData());
						}else{
							insp.clear();
						}
						_sel = _mov;
						if(_sel && _sel != &_s){
							_multi.insert((SegTarget*)_sel);
						}
					}
				}break;
				case sf::Mouse::Button::Middle:{
					_drag = true;
				}break;
				case sf::Mouse::Button::Right:{
					_hier.regroup(_window.mapPixelToCoords(mPos, _hier.getView()), _multi);
				}break;
				default:{};
			}
		}break;
		case sf::Event::MouseButtonReleased:{
			if(e.mouseButton.button == sf::Mouse::Button::Middle){
				_drag = false;
			}
			if(_mov){
				_mov->snap();
			}
			_mov = nullptr;
		}break;
		case sf::Event::MouseMoved:{
			sf::Vector2f last = _window.mapPixelToCoords(_last, _view);
			sf::Vector2f curPos = _window.mapPixelToCoords(mPos, _view);
			sf::Vector2f diff = curPos - last;
			if(_drag){
				_view.move(diff.x * -1, diff.y * -1);
			}else if(_mov){
				_mov->move(diff.x, diff.y);
				insp.updateView();
			}
			
			_last = mPos;
		}break;
		case sf::Event::MouseWheelScrolled:{
			sf::Vector2f point = _window.mapPixelToCoords(mPos, _hier.getView());
			point.y = 0;
			if(sf::FloatRect(0, 0, 250, 5).contains(point)){
				_hier.scroll((float)e.mouseWheelScroll.delta * 10);
			}else{
				_view.zoom((float)e.mouseWheelScroll.delta / 5 + 1);
			}
		}break;
		case sf::Event::KeyPressed:{
			if(!_editing){
				switch(e.key.code){
					case sf::Keyboard::Key::D:{
						for(auto target : _multi){
							_hier.remove(target);
							_s.remove(target);
						}
						_multi.clear();
						insp.clear();
						_sel = nullptr;
						_mov = nullptr;
					}break;
					case sf::Keyboard::Key::A:{
						//if(_sel == &s){
						//	_hier.newTarget(_window.mapPixelToCoords(mPos, _view), s.getSnap());
						//}
						if(_sel){
							SegTarget& seg = _hier.newTarget(&(_sel->newTarget(_window.mapPixelToCoords(mPos, _view))));
							if(_sel != &_s){
								std::string build = _sel->getData().access<std::string>("Name");
								std::string index;
								try{
									index = build.substr(build.find_last_of(".") + 1);
									int i = std::stoi(index);
									i++;
									build.erase(build.find_last_of(".") + 1);
									build += std::to_string(i);
								}catch(std::invalid_argument ex){
									build += ".1";
								}catch (std::out_of_range ex){
									build += ".1";
								}
								seg.getData().access<std::string>("Name") = build;
								_sel->mark(false);
								_sel = &seg;
								_sel->mark(true);
								_hier.preDraw();
							}
						}
					}break;
					case sf::Keyboard::Key::S:{
						_s.snap();
						for(auto target : _hier.getAllTargets()){
							target->snap();
						}
					}break;
					case sf::Keyboard::Key::G:{
						_hier.newGroup();
					}
					default:{}
				}
			}else{
				switch(e.key.code){
					case sf::Keyboard::Key::D:{
						_hier.deleteSelectedGroups();
						insp.clear();
					}break;
					case sf::Keyboard::Key::Enter:{
						insp.updateData();
						_hier.preDraw();
						_s.updateData();
					}break;
					case sf::Keyboard::Key::Backspace:{
						insp.delLastChar();
					}
					default:{}
				}
			}
		}break;
		case sf::Event::TextEntered:{
			if(_editing && e.text.unicode > 31 && e.text.unicode < 127){
				insp.write(sf::String(e.text.unicode));
			}
		}break;
		default:{};
	}
}

void SegWindow::preDraw(){
	auto pos = _s.getCenter();
	for(auto& t : _hier.getActive()){
		t->updateSource(pos);
	}
}

void SegWindow::clear(){
	_sel = nullptr;
	_mov = nullptr;
	_drag = false;
	_editing = false;
}

void SegWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	const sf::View& prev = target.getView();
	target.setView(_view);
	
	target.draw(_s);
	target.draw(_hier);
	
	target.setView(prev);
}
