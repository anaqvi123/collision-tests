#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>

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

	Rectangle(float width, float height, float x, float y){
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setOutlineColor(sf::Color::White);
		rectangle.setPosition({x, y});
	}

};

sf::Vector2f minAndMaxOnAxis(Rectangle r, sf::Vector2f axis){
	float corner1 = dotProduct({r.x, r.y}, axis);
	float corner2 = dotProduct({r.x + r.width, r.y}, axis);
	float corner3 = dotProduct({r.x + r.width, r.y + r.height}, axis);
	float corner4 = dotProduct({r.x, r.y + r.height}, axis);
	
	return sf::Vector2f(std::min({corner1, corner2, corner3, corner4}), std::max({corner1, corner2, corner3, corner4}));
}

void detectCollision(sf::RenderWindow& window, Rectangle& r1, Rectangle& r2){
		sf::Vector2f xAxis = {1, 0};
		sf::Vector2f yAxis = {0, 1};

		sf::Vector2f r1MinMaxXAxis = minAndMaxOnAxis(r1, xAxis);
		sf::Vector2f r1MinMaxYAxis = minAndMaxOnAxis(r1, yAxis);

		sf::Vector2f r2MinMaxXAxis = minAndMaxOnAxis(r2, xAxis);
		sf::Vector2f r2MinMaxYAxis = minAndMaxOnAxis(r2, yAxis);
		


		if(r2MinMaxXAxis.x <= r1MinMaxXAxis.y && r1MinMaxXAxis.x <= r2MinMaxXAxis.y && r2MinMaxYAxis.x <= r1MinMaxYAxis.y && r1MinMaxYAxis.x <= r2MinMaxYAxis.y){
			r1.rectangle.setFillColor(sf::Color::Red);
			r2.rectangle.setFillColor(sf::Color::Red);
		}
		else{
			r1.rectangle.setFillColor(sf::Color::White);
			r2.rectangle.setFillColor(sf::Color::White);
		}
	}


int main()
{
	sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Collision Tests");
	window.setFramerateLimit(60);

	Rectangle rectangle1(100, 30, 500, 500);
	Rectangle rectangle2(100, 30, 700, 700);


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
