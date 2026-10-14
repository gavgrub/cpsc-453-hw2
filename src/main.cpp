#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;
const float SPACE = 0.525f;
const std::string FONT = std::filesystem::current_path().string() + "/cmunit";

// This function gets the size (width, height) of a list of vertices
sf::FloatRect getRect(const std::vector<sf::Vertex>& vertices) {

    // Return 0 if list of vertices is empty
    if (vertices.empty()) {
        return sf::FloatRect({0.0f, 0.0f}, {0.0f, 0.0f});
    }

    float xMin = vertices[0].position.x;
    float xMax = xMin;
    float yMin = vertices[0].position.y;
    float yMax = yMin;

    // Iterate through vertices and check if they are larger / smaller than the current largest / smallest found
    for (const sf::Vertex& vertex : vertices) {
        xMin = std::min(xMin, vertex.position.x);
        xMax = std::max(xMax, vertex.position.x);
        yMin = std::min(yMin, vertex.position.y);
        yMax = std::max(yMax, vertex.position.y);
    }

    return sf::FloatRect({xMin, yMin}, {xMax - xMin, yMax - yMin});
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
                vertices.push_back(sf::Vertex{currentPoint});
                vertices.push_back(sf::Vertex{nextPoint});

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

                    // Calculate x(t) using the Bezier equation
                    sf::Vector2f xt = std::pow(1.0f - t, 3.0f) * p0 + 3.0f * t * std::pow(1.0f - t, 2.0f) * p1 + 3.0f * std::pow(t, 2.0f) * (1.0f - t) * p2 + std::pow(t, 3.0f) * p3;

                    // Add the line segment to the vertices
                    vertices.push_back(sf::Vertex{currentPoint});
                    vertices.push_back(sf::Vertex{xt});

                    // Update the current point
                    currentPoint = xt;
                }

                break;
            }
            
            // End path by connecting the current point to the beginning of the path via a line segment
            case 'Z':
                vertices.push_back(sf::Vertex{currentPoint});
                vertices.push_back(sf::Vertex{startPoint});
                currentPoint = startPoint;
                break;
        }
    }

    // Return true if the file was processed
    return true;
}

// This function creates a string by generating paths for each character in the string
void loadString(const std::string& text, const std::string& directory, std::vector<sf::Vertex>& vertices) {

    float x = 0.0f;

    for (char character : text) {

        // File path of given character
        std::string filename = directory + "/gly_" + std::to_string(static_cast<int>(character));

        // Vertices of the character
        std::vector<sf::Vertex> characterVertices;

        // Add the character if it exists, shifted by the current position
        if (loadCharacter(filename, characterVertices)) {
            for (sf::Vertex& vertex : characterVertices) {
                vertex.position.x += x;
                vertices.push_back(vertex);
            }
        }

        // Move to the next character
        x += SPACE;
    }
}

int main(int argc, char* argv[]) {

    // Load the string
    std::vector<sf::Vertex> vertices;
    loadString(argv[1], FONT, vertices);

    // Make the string fit within the window
    sf::FloatRect rect = getRect(vertices);
    float margin = 20.0f;
    float scale = 1.0f;

    if (rect.size.x > 0.0f) {
        scale = (WIDTH - 2.0f * margin) / rect.size.x;
    }

    // Middle of the string
    float xMid = rect.position.x + rect.size.x / 2.0f;
    float yMid = rect.position.y + rect.size.y / 2.0f;

    // Put string in the middle of the window
    float x = WIDTH / 2.0f - xMid * scale;
    float y = HEIGHT / 2.0f + yMid * scale;

    // Movement and zoom amounts per frame
    float speed = 8.0f;
    float zoom = 1.02f;

    // Create window
	sf::RenderWindow window(
    	sf::VideoMode({WIDTH, HEIGHT}), 
    	"CPSC 453 Assignment 2",
        sf::Style::Default, 
    	sf::State::Windowed
	);

    // set additional window properties
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);

    // Event loop
    while (window.isOpen()) {
		while ( const std::optional event = window.pollEvent() ) {

            // Close the window
			if ( event->is<sf::Event::Closed>() ) {
                window.close();
			}
		}

        // Scroll
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            x += speed;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            x -= speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            y += speed;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            y -= speed;
        }

        // Zoom in / out
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal)) {
            x = WIDTH / 2.0f - (WIDTH / 2.0f - x) * zoom;
            y = HEIGHT / 2.0f - (HEIGHT / 2.0f - y) * zoom;
            scale *= zoom;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Hyphen)) {
            x = WIDTH / 2.0f - (WIDTH / 2.0f - x) / zoom;
            y = HEIGHT / 2.0f - (HEIGHT / 2.0f - y) / zoom;
            scale /= zoom;
        }

        // Move the window to correctly 
        sf::Transform transform;
        transform.translate({x, y});
        transform.scale({scale, -scale});

        // Draw to the screen
        window.clear();
        window.draw(vertices.data(), vertices.size(), sf::PrimitiveType::Lines, sf::RenderStates(transform));
        window.display();
    }

    return 0;
}