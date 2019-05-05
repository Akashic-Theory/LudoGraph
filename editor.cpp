#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "segment.h"
#include "inspector.h"
#include "global.h"



int main(int argc, char* argv[])
{
	using namespace std;
	
	sf::VideoMode resolution = sf::VideoMode(1280, 720);
	// Handle commandline arguments
	std::cout << argv[0] << std::endl;
	if(argc > 1){
		int count = 0;
		while(count < argc){
			string str = string(argv[count]);
			if(!str.compare("-r")){
				count++;
				if(count < argc){
					cout << "custom res" << endl;
					str = string(argv[count]);
					if(!str.compare("1080p")){
						cout << "Resolution set to 1080p" << endl;
						resolution = sf::VideoMode(1920, 1080);
					}
				}
			}
			count++;
		}
	}
	
	Global::get()._font.loadFromFile("res/SpaceMono.ttf");
	
	sf::Clock clock = sf::Clock();
	
	sf::RenderWindow window(resolution, "SFML works!");
	//window.setFramerateLimit(5);
	sf::View mainView = sf::View();
	
	Inspector& insp = Global::get()._insp;
	Segment s = Segment();
	sf::Vector2i mPos = sf::Mouse::getPosition(window);
	SegmentController* mov = nullptr;
	SegmentController* sel = nullptr;
	std::set<SegTarget*> multi = std::set<SegTarget*>();
	bool drag = false;
	bool editing = false;
	
	TargetGroupHierarchy hier = TargetGroupHierarchy();
	//vector<SegTarget*> targets = vector<SegTarget*>();
	
	mainView.reset(sf::FloatRect(0, 0, 720, 720));
	mainView.setViewport(sf::FloatRect(0.2f, 0.05f, 0.6f, 0.95f));
	
	window.setView(mainView);
	
	while (window.isOpen()){
		//Messy event handling ----- Refactor later
		sf::Event event;
		while (window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:{
					// Window close button pressed
					window.close();
				}break;
				case sf::Event::MouseButtonPressed:{
					// Mouse pressed
					if(event.mouseButton.button == sf::Mouse::Button::Left){
						//Reset selected objet
						mov = nullptr;
						if(editing){
							insp.updateData();
							s.updateData();
							hier.preDraw();
						}
						if(insp.select(window.mapPixelToCoords(mPos, insp.getView()), [&](){
								s.updateData();
								hier.preDraw();
								cout << "LAMBDA" << endl;
							}) 
							|| hier.select(window.mapPixelToCoords(mPos, hier.getView()), sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl), [&](InspectorData* dat){
								insp.generate(*dat);
							})){
							editing = true;
						}else{
							editing = false;
							if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)){
								if(sel){
									for(auto t : multi){
										t->mark(false);
									}
									sel->mark(false);
									sel = nullptr;
								}
								multi.clear();
							}
						}
						mPos = sf::Mouse::getPosition(window);
						if(!editing){
							//Main View Click
							sf::Vector2f mPosView = window.mapPixelToCoords(mPos, mainView);
							for(auto t : hier.getActive()){
								if(t->getGlobalBounds().contains(mPosView)){
									mov = t;
								}
							}
							if(mov == nullptr && s.getGlobalBounds().contains(mPosView)){
								mov = &s;
							}
							if(mov){
								mov->mark(true);
								insp.generate(mov->getData());
							}else{
								insp.clear();
							}
							sel = mov;
							if(sel && sel != &s){
								multi.insert((SegTarget*)sel);
								cout << "Inserted! cur: " << multi.size() << endl;
							}
						}
					}else if(event.mouseButton.button == sf::Mouse::Button::Middle){
						drag = true;
					}else if(event.mouseButton.button == sf::Mouse::Button::Right){
						mPos = sf::Mouse::getPosition(window);
						hier.regroup(window.mapPixelToCoords(mPos, hier.getView()), multi);
					}
					
				}break;
				case sf::Event::MouseButtonReleased:{
					if(event.mouseButton.button == sf::Mouse::Button::Middle){
						drag = false;
					}
					if(mov){
						mov->snap();
					}
					mov = nullptr;
				}break;
				case sf::Event::MouseMoved:{
					sf::Vector2f last = window.mapPixelToCoords(mPos, mainView);
					mPos = sf::Mouse::getPosition(window);
					sf::Vector2f curPos = window.mapPixelToCoords(mPos, mainView);
					sf::Vector2f diff = curPos - last;
					if(drag){
						mainView.move(diff.x * -1, diff.y * -1);
					}else if(mov){
						mov->move(diff.x, diff.y);
						insp.updateView();
					}
				}break;
				case sf::Event::MouseWheelScrolled:{
					sf::Vector2f point = window.mapPixelToCoords(mPos, hier.getView());
					point.y = 0;
					if(sf::FloatRect(0, 0, 250, 5).contains(point)){
						hier.scroll((float)event.mouseWheelScroll.delta * 10);
					}else{
						mainView.zoom((float)event.mouseWheelScroll.delta / 5 + 1);
					}
				}break;
				case sf::Event::KeyPressed:{
					// Unconditional
					//switch(event.key.code){
					//	case sf::Keyboard::Key::R:{
					//		s.updateData();
					//	}break;
					//	default:{}
					//}
					// Only if  not currently in editor
					if(!editing){
						switch(event.key.code){
							case sf::Keyboard::Key::D:{
								for(auto target : multi){
									hier.remove(target);
								}
								multi.clear();
								insp.clear();
								sel = nullptr;
								mov = nullptr;
								/*for(auto it = targets.begin(); it != targets.end();){
									if(*it == sel){
										delete *it;
										targets.erase(it);
										it = targets.end();
										sel = nullptr;
										mov = nullptr;
									}else{
										++it;
									}
								}*/
								///std::cout << targets.size() << std::endl;
							}break;
							case sf::Keyboard::Key::A:{
								//if(sel == &s){
								//	hier.newTarget(window.mapPixelToCoords(mPos, mainView), s.getSnap());
								//}
								if(sel){
									SegTarget& seg = hier.newTarget(window.mapPixelToCoords(mPos, mainView), sel->getData().access<sf::Vector2f>("Snap Amt"));
									if(sel != &s){
										std::string build = sel->getData().access<std::string>("Name");
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
										sel->mark(false);
										sel = &seg;
										sel->mark(true);
									}
								}
							}break;
							case sf::Keyboard::Key::S:{
								s.snap();
								for(auto target : hier.getAllTargets()){
									target->snap();
								}
							}break;
							case sf::Keyboard::Key::G:{
								hier.newGroup();
							}
							default:{}
						}
					}else{
						switch(event.key.code){
							case sf::Keyboard::Key::D:{
								cout << "Deleting Selected Groupd" << endl;
								hier.deleteSelectedGroups();
								insp.clear();
							}break;
							case sf::Keyboard::Key::Enter:{
								insp.updateData();
								hier.preDraw();
								s.updateData();
							}break;
							case sf::Keyboard::Key::Backspace:{
								insp.delLastChar();
							}
							default:{}
						}
					}
				}break;
				case sf::Event::TextEntered:{
					if(editing && event.text.unicode > 31 && event.text.unicode < 127){
						insp.write(sf::String(event.text.unicode));
					}
				}break;
				default:{
					
				}
			}
		}
		
		//cout << (bool)mov << (bool)sel << drag << editing << endl;
		//Draw Cycle
		window.clear();
		
		window.setView(mainView);
		window.draw(s);
		{
			auto pos = s.getCenter();
			for(auto& t : hier.getActive()){
				t->updateSource(pos);
			}
		}
		
		/*for(auto t : targets){
			t->updateSource(s.getCenter());
			window.draw(*t);
		}*/
		
		window.draw(insp);
		window.draw(hier);
		
		window.display();
	}

	return 0;
}
