#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// This function draws lines between a given list of vectors
void drawLines(sf::RenderWindow& window, std::vector<sf::Vertex>& vertices) {
    for (int i = 0; i + 1 < vertices.size(); i += 2) {
        window.draw(&vertices[i], 2, sf::PrimitiveType::Lines);
    }
}

// This function loads a character from a file and stores its lines as vertices
bool loadCharacter(std::string filename, std::vector<sf::Vertex>& vertices) {
    
    // Open the file, check if it's open
    std::ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    char command;
    float x, y;

    sf::Vector2f currentPoint;
    sf::Vector2f startPoint;

    // Iterate through each command in the file
    while (file >> command) {
        switch (command) {
            // Start a new path
            case 'M':
                file >> x >> y;
                currentPoint = {x, y};
                startPoint = currentPoint;
                break;

            // Draw a line from the current point to the specified point
            case 'L': {
                
                // Draw a line from the current point to the next point
                file >> x >> y;
                sf::Vector2f nextPoint = {x, y};

                // Add the line's endpoints to the vertices
                vertices.push_back(sf::Vertex(currentPoint));
                vertices.push_back(sf::Vertex(nextPoint));

                // Update the current point
                currentPoint = nextPoint;
                break;
            }

            // Draw a cubic Bezier curve using the current point and the specified three points as control points
            case 'C': {

                // Read the three points used to define the Bezier curve
                float x1, y1, x2, y2, x3, y3;
                file >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;

                // Determine the four points of the of the curve
                sf::Vector2f p0 = currentPoint;
                sf::Vector2f p1 = {x1, y1};
                sf::Vector2f p2 = {x2, y2};
                sf::Vector2f p3 = {x3, y3};

                // Iterate 20 times
                int iterations = 20;
                for (int i = 1; i <= iterations; i++) {

                    // Calculate the position along the curve
                    float t = static_cast<float>(i) / iterations;
                    float u = 1.0f - t;

                    // Calculate x(t) using the Bezier equation
                    sf::Vector2f xt = std::pow(1.0f - t, 3.0f) * p0 + 3.0f * t * std::pow(1.0f - t, 2.0f) * p1 + 3.0f * std::pow(t, 2.0f) * (1.0f - t) * p2 + std::pow(t, 3.0f) * p3;

                    // Add the line segment to the vertices
                    vertices.push_back(sf::Vertex(currentPoint));
                    vertices.push_back(sf::Vertex(xt));

                    // Update the current point
                    currentPoint = xt;
                }

                break;
            }
            
            // End path by connecting the current point to the beginning of the path via a line segment
            case 'Z':
                vertices.push_back(sf::Vertex(currentPoint));
                vertices.push_back(sf::Vertex(startPoint));
                currentPoint = startPoint;
                break;
        }
    }

    // Return true if the file was processed
    return true;
}

int main() {

    // Create window
	sf::RenderWindow window(
    	sf::VideoMode({WIDTH, HEIGHT}), 
    	"CPSC 453 Assignment 2",
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