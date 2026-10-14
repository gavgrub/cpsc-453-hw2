#include <SFML/Graphics.hpp>
#include <iostream>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

int main() {

    // Create window
	sf::RenderWindow window(
    	sf::VideoMode({WIDTH, HEIGHT}), 
    	"SFML + GLAD (OpenGL 2.1)",
        sf::Style::Default, 
    	sf::State::Windowed
	);

    // set additional window properties
    window.setKeyRepeatEnabled(false);

    // the event/logic/whatever loop
    while (window.isOpen()) {
		while ( const std::optional event = window.pollEvent() ) {

            // Close the window
			if ( event->is<sf::Event::Closed>() ) {
                window.close();
			}

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                
                // Key press events are used to trigger actions, not to hold them down.

            }

            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                
				// key release events are used to trigger actions, not to hold them down.

			}
		}

        // Draw to the screen
        window.clear();
        window.display();

    }

}