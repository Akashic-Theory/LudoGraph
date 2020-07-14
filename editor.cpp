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
	
	sf::RenderWindow window(resolution, "SFML works!");
	SegWindow segWindow = SegWindow(window);
	
	sf::View tabView = sf::View();
	
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
	
	tabView.reset(sf::FloatRect(0, 0, 720, 60));
	tabView.setViewport(sf::FloatRect(0.2f, 0.0f, 0.6f, 0.05f));
	
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
						sf::Vector2i mPos = sf::Mouse::getPosition(window);
						if(sf::FloatRect(sf::Vector2f(), tabView.getSize()).contains(window.mapPixelToCoords(mPos, tabView))){
							cout << tabStrings[(int)window.mapPixelToCoords(mPos, tabView).x * 4 / 720] << endl;
						}
					}
					segWindow.processEvent(event);
				}break;
				case sf::Event::MouseButtonReleased:
				case sf::Event::MouseMoved:
				case sf::Event::MouseWheelScrolled:
				case sf::Event::KeyPressed:
				case sf::Event::TextEntered:{
					segWindow.processEvent(event);
				}break;
				default:{
					
				}
			}
		}
		
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
		
		window.draw(Global::get()._insp);
		//window.draw(hier);
		
		window.display();
	}

	return 0;
}
