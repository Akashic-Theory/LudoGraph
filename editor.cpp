#include <iostream>
#include <vector>
#include <string>

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
	
	Global::get()._font.loadFromFile("SpaceMono.ttf");
	
	sf::RenderWindow window(resolution, "SFML works!");
	sf::Clock clock = sf::Clock();
	
	Inspector insp = Inspector(window);
	//cout << "1" << endl;
	Segment s = Segment();
	//cout << "2" << endl;
	sf::Vector2i mPos = sf::Mouse::getPosition(window);
	SegmentController* mov = nullptr;
	
	vector<SegTarget*> targets = vector<SegTarget*>();
	
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
						//LMB
						mPos = sf::Mouse::getPosition(window);
						for(auto t : targets){
							if(t->getGlobalBounds().contains(mPos.x, mPos.y)){
								mov = t;
							}
						}
						if(mov == nullptr && s.getGlobalBounds().contains(mPos.x, mPos.y)){
							mov = &s;
						}
						if(mov){
							mov->mark(true);
							insp.generate(mov->getData());
						}
					}
				}break;
				case sf::Event::MouseButtonReleased:{
					if(mov){
						mov->mark(false);
					}
					mov = nullptr;
				}break;
				case sf::Event::MouseMoved:{
					if(mov){
						sf::Vector2i last = mPos;
						mPos = sf::Mouse::getPosition(window);
						sf::Vector2i diff = mPos - last;
						mov->move(diff.x, diff.y);
					}
				}break;
				case sf::Event::KeyPressed:{
					if(event.key.code == sf::Keyboard::Key::A){
						targets.push_back(new SegTarget());
					}
				}break;
				default:{
					
				}
			}
		}
		
		//Draw Cycle
		window.clear();
		window.draw(s);
		window.draw(insp);
		for(auto t : targets){
			t->updateSource(s.getOrigin());
			window.draw(*t);
		}
		window.display();
	}

	return 0;
}
