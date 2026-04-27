#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

float vectorMagnitude(sf::Vector2f vec){
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}
float distance(sf::Vector2f pos1, sf::Vector2f pos2){
	return vectorMagnitude({pos2.x - pos1.x, pos2.y - pos1.y});
}
sf::Vector2f unitVector(sf::Vector2f vec){
	if(vectorMagnitude(vec) == 0){return {1, 0};}
	return {vec.x / vectorMagnitude(vec), vec.y / vectorMagnitude(vec)};
}
float dotProduct(sf::Vector2f vec1, sf::Vector2f vec2){
	return vec1.x * vec2.x + vec1.y * vec2.y;
}
sf::Vector2f vectorProjection(sf::Vector2f vec1, sf::Vector2f vec2){
	float magnitude = vectorMagnitude(vec2);
	return dotProduct(vec1, vec2) / (magnitude * magnitude) * vec2;
}




struct Rectangle{
	sf::RectangleShape rectangle;
	float x;
	float y;
	float width;
	float height;

	Rectangle(float width, float height, float x, float y, float rotation){
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setOutlineColor(sf::Color::White);
		rectangle.setRotation(sf::degrees(rotation));
		rectangle.setPosition({x, y});
	}

	std::vector<sf::Vector2f> getCorners(){
		sf::Vector2f direction = {std::cos(rectangle.getRotation().asDegrees()), std::sin(rectangle.getRotation().asDegrees())};
		sf::Vector2f corner1 = {x, y};
		sf::Vector2f corner2 = {x + direction, y};
	}
};





std::vector<sf::Vector2f> findNormals(Rectangle& r){
	sf::Vector2f normal1 = sf::Vector2f{r.x + r.width, r.y} - sf::Vector2f{r.x, r.y};
	sf::Vector2f normal2 = sf::Vector2f{r.x, r.y + r.height} - sf::Vector2f{r.x, r.y};
	return {normal1, normal2};
}

sf::Vector2f minAndMaxOnAxis(Rectangle r, sf::Vector2f axis){
	float corner1 = dotProduct({r.x, r.y}, axis);
	float corner2 = dotProduct({r.x + r.width, r.y}, axis);
	float corner3 = dotProduct({r.x + r.width, r.y + r.height}, axis);
	float corner4 = dotProduct({r.x, r.y + r.height}, axis);
	
	return sf::Vector2f(std::min({corner1, corner2, corner3, corner4}), std::max({corner1, corner2, corner3, corner4}));
}

bool detectCollision(sf::RenderWindow& window, Rectangle& r1, Rectangle& r2){
		std::vector<sf::Vector2f> normalsR1 = findNormals(r1);
		std::vector<sf::Vector2f> normalsR2 = findNormals(r2);
		
		bool colliding = true;

		for(sf::Vector2f axis : normalsR1){
			sf::Vector2f r1MinMaxAxis = minAndMaxOnAxis(r1, axis);
			sf::Vector2f r2MinMaxAxis = minAndMaxOnAxis(r2, axis);

			if(!(r2MinMaxAxis.x <= r1MinMaxAxis.y && r1MinMaxAxis.x <= r2MinMaxAxis.y)){
				colliding = false;
			}
		}

		if(colliding){
			r1.rectangle.setFillColor(sf::Color::Red);
			r2.rectangle.setFillColor(sf::Color::Red);
		}
		else{
			r1.rectangle.setFillColor(sf::Color::White);
			r2.rectangle.setFillColor(sf::Color::White);
		}

		return colliding;
	}


int main()
{
	sf::RenderWindow window(sf::VideoMode({1280, 720}), "Collision Tests");
	window.setFramerateLimit(60);

	Rectangle rectangle1(100, 30, 300, 300, 30);
	Rectangle rectangle2(100, 30, 500, 500, 0);


	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear();
		rectangle1.x += 1;
		rectangle1.y += 1;
		rectangle1.rectangle.setPosition({rectangle1.x, rectangle1.y});
		window.draw(rectangle1.rectangle);
		window.draw(rectangle2.rectangle);
		detectCollision(window, rectangle1, rectangle2);
		window.display();
	}
}
