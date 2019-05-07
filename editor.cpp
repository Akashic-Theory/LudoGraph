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
#include "editorwindows.h"



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
	SegWindow segWindow = SegWindow(window);
	//window.setFramerateLimit(5);
	sf::View mainView = sf::View();
	sf::View tabView = sf::View();
	
	Inspector& insp = Global::get()._insp;
	Segment s = Segment();
	sf::Vector2i mPos = sf::Mouse::getPosition(window);
	SegmentController* mov = nullptr;
	SegmentController* sel = nullptr;
	std::set<SegTarget*> multi = std::set<SegTarget*>();
	bool drag = false;
	bool editing = false;
	
	int tab = 0;
	
	sf::Text tabText = sf::Text();
	tabText.setFont(Global::get()._font);
	
	
	std::vector<std::string> tabStrings = std::vector<std::string>();
	tabStrings.push_back("N/A");
	tabStrings.push_back("N/A");
	tabStrings.push_back("Board");
	tabStrings.push_back("Segment");
	
	const int tabThickness = 5;
	sf::RectangleShape tabRect = sf::RectangleShape(sf::Vector2f(720 / 4 - (2* tabThickness), 60 - (2*tabThickness)));
	tabRect.setFillColor(sf::Color(27, 96, 124));
	tabRect.setOutlineColor(sf::Color(99, 157, 181));
	tabRect.setOutlineThickness(5);
	
	TargetGroupHierarchy hier = TargetGroupHierarchy();
	//vector<SegTarget*> targets = vector<SegTarget*>();
	
	mainView.reset(sf::FloatRect(0, 0, 720, 720));
	mainView.setViewport(sf::FloatRect(0.2f, 0.05f, 0.6f, 0.95f));
	tabView.reset(sf::FloatRect(0, 0, 720, 60));
	tabView.setViewport(sf::FloatRect(0.2f, 0.0f, 0.6f, 0.05f));
	
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
					segWindow.processEvent(event);
					if(event.mouseButton.button == sf::Mouse::Button::Left){
						if(sf::FloatRect(sf::Vector2f(), tabView.getSize()).contains(window.mapPixelToCoords(mPos, tabView))){
							cout << tabStrings[(int)window.mapPixelToCoords(mPos, tabView).x * 4 / 720] << endl;
						}
					}
					/*
					if(event.mouseButton.button == sf::Mouse::Button::Left){
						//Reset selected objet
						mov = nullptr;
						if(editing){
							insp.updateData();
							s.updateData();
							hier.preDraw();
						}
						if(sf::FloatRect(sf::Vector2f(), tabView.getSize()).contains(window.mapPixelToCoords(mPos, tabView))){
							cout << tabStrings[(int)window.mapPixelToCoords(mPos, tabView).x * 4 / 720] << endl;
						}else if(insp.select(window.mapPixelToCoords(mPos, insp.getView()), [&](){
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
					*/
				}break;
				case sf::Event::MouseButtonReleased:{
					segWindow.processEvent(event);
					/*
					if(event.mouseButton.button == sf::Mouse::Button::Middle){
						drag = false;
					}
					if(mov){
						mov->snap();
					}
					mov = nullptr;
					*/
				}break;
				case sf::Event::MouseMoved:{
					segWindow.processEvent(event);
					/*
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
					*/
				}break;
				case sf::Event::MouseWheelScrolled:{
					segWindow.processEvent(event);
					/*
					sf::Vector2f point = window.mapPixelToCoords(mPos, hier.getView());
					point.y = 0;
					if(sf::FloatRect(0, 0, 250, 5).contains(point)){
						hier.scroll((float)event.mouseWheelScroll.delta * 10);
					}else{
						mainView.zoom((float)event.mouseWheelScroll.delta / 5 + 1);
					}
					*/
				}break;
				case sf::Event::KeyPressed:{
					segWindow.processEvent(event);
					/*
					if(!editing){
						switch(event.key.code){
							case sf::Keyboard::Key::D:{
								for(auto target : multi){
									hier.remove(target);
									s.remove(target);
								}
								multi.clear();
								insp.clear();
								sel = nullptr;
								mov = nullptr;
								///std::cout << targets.size() << std::endl;
							}break;
							case sf::Keyboard::Key::A:{
								//if(sel == &s){
								//	hier.newTarget(window.mapPixelToCoords(mPos, mainView), s.getSnap());
								//}
								if(sel){
									SegTarget& seg = hier.newTarget(&(sel->newTarget(window.mapPixelToCoords(mPos, mainView))));
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
					*/
				}break;
				case sf::Event::TextEntered:{
					segWindow.processEvent(event);
					/*
					if(editing && event.text.unicode > 31 && event.text.unicode < 127){
						insp.write(sf::String(event.text.unicode));
					}
					*/
				}break;
				default:{
					
				}
			}
		}
		
		//cout << (bool)mov << (bool)sel << drag << editing << endl;
		//Draw Cycle
		window.clear();
		
		//Tab Draw Cycle
		window.setView(tabView);
		for(int i = 0; i < 4; i++){
			tabRect.setPosition(sf::Vector2f(i * 180 + tabThickness, tabThickness));
			window.draw(tabRect);
			tabText.setString(tabStrings[i]);
			sf::FloatRect rec = tabText.getLocalBounds();
			rec.left = i * 180 + (180 - rec.width) / 2.0;
			tabText.setPosition(rec.left, rec.top);
			window.draw(tabText);
		}
		//End Tab Draw
		segWindow.preDraw();
		window.draw(segWindow);
		
		/*
		window.setView(mainView);
		window.draw(s);
		{
			auto pos = s.getCenter();
			for(auto& t : hier.getActive()){
				t->updateSource(pos);
			}
		}
		*/
		/*for(auto t : targets){
			t->updateSource(s.getCenter());
			window.draw(*t);
		}*/
		
		window.draw(insp);
		//window.draw(hier);
		
		window.display();
	}

	return 0;
}
