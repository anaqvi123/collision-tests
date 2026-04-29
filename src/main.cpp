#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

float vectorMagnitude(sf::Vector2f vec){
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}
float vectorMagnitudeSquared(sf::Vector2f vec){
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}
float distanceSquared(sf::Vector2f pos1, sf::Vector2f pos2){
	return vectorMagnitudeSquared({pos2.x - pos1.x, pos2.y - pos1.y});
}
sf::Vector2f unitVector(sf::Vector2f vec){
	if(vectorMagnitude(vec) == 0){return {1, 0};}
	return {vec.x / vectorMagnitude(vec), vec.y / vectorMagnitude(vec)};
}
float dotProduct(sf::Vector2f vec1, sf::Vector2f vec2){
	return vec1.x * vec2.x + vec1.y * vec2.y;
}
float vectorProjection(sf::Vector2f vec1, sf::Vector2f vec2){
	float magnitudeSquared = vectorMagnitudeSquared(vec2);
	return dotProduct(vec1, vec2) / magnitudeSquared;
}




struct Rectangle{
	sf::RectangleShape rectangle;
	float x;
	float y;
	float width;
	float height;
	float rotation;

	Rectangle(float width, float height, float x, float y, float rotation){
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->rotation = rotation;
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setOutlineColor(sf::Color::White);
		rectangle.setRotation(sf::degrees(rotation));
		rectangle.setPosition({x, y});
	}

	std::vector<sf::Vector2f> getCorners(){
		sf::Vector2f direction1 = {std::cos(rotation * 3.14159f / 180.0f), std::sin(rotation * 3.14159f / 180.0f)};
		sf::Vector2f direction2 = {-direction1.y, direction1.x};
		sf::Vector2f corner1 = {x, y};
		sf::Vector2f corner2 = {x + direction1.x * width, y + direction1.y * width};
		sf::Vector2f corner3 = {x + direction1.x * width + direction2.x * height, y + direction1.y * width + direction2.y * height};
		sf::Vector2f corner4 = {x + direction2.x * height, y + direction2.y * height};
		return std::vector<sf::Vector2f>{corner1, corner2, corner3, corner4};
	}

	std::vector<sf::Vector2f> findNormals(){
		std::vector<sf::Vector2f> corners = getCorners();
		sf::Vector2f normal1 = unitVector(corners[1] - corners[0]);
		sf::Vector2f normal2 = unitVector(corners[3] - corners[0]);
		return {normal1, normal2};
	}
};

struct Circle{
	sf::CircleShape circle;
	float radius = 20;
	int points = 100;
	float restitution = 0.9;
	float mass;
	float x;
	float y;
	float vx;
	float vy;
	float ax;
	float ay;

	Circle(float x, float y, float vx, float vy, float ax, float ay, const float mass, const float radius){
		this->x = x;
		this->y = y;
		this->vx = vx;
		this->vy = vy;
		this->ax = ax;
		this->ay = ay;
		this->mass = mass;
		circle.setRadius(radius);
		circle.setOrigin({radius, radius}); // make the center (0, 0)
		circle.setPointCount(points);
		circle.setFillColor(sf::Color::Blue);
	}

	void update(float dt){
		x = x + vx * dt; 
		y = y + vy * dt;
		vx = vx + ax * dt;
		vy = vy + ay * dt;
		vx *= 0.999f;
		vy *= 0.999f;
	}
/*
	void wallCollision(sf::RenderWindow& window, std::vector<std::vector<sf::Vector2f>> lines){
		for(std::vector<sf::Vector2f>& line : lines){
			sf::Vector2f lineVec = {line[1].x - line[0].x, line[1].y - line[0].y};
			sf::Vector2f relativePos = sf::Vector2f{x, y} - line[0];
			sf::Vector2f lineNormalUnitVec = unitVector({-lineVec.y, lineVec.x});
			
			float closestPoint = vectorProjection(relativePos, lineVec);
			if(closestPoint > 1){closestPoint = 1;}
			if(closestPoint < 0){closestPoint = 0;}
			sf::Vector2f closestPointPos = line[0] + closestPoint * lineVec;

			float distanceFromClosestPoint = distanceSquared({x,y}, closestPointPos);

			if(distanceFromClosestPoint < radius * radius && dotProduct({vx, vy}, lineNormalUnitVec) < 0){ // distance < radius and moving toward line
				sf::Vector2f normalVel = dotProduct({vx, vy}, -lineNormalUnitVec) * -lineNormalUnitVec;
				sf::Vector2f sidewaysVel = sf::Vector2f{vx, vy} - normalVel;
				
				vx = sidewaysVel.x - normalVel.x * restitution;
				vy = sidewaysVel.y - normalVel.y * restitution;

				float overlap = radius - std::sqrt(distanceFromClosestPoint);
				x += overlap * lineNormalUnitVec.x;
				y += overlap * lineNormalUnitVec.y;
			}
		}
	}
		*/
	void draw(sf::RenderWindow& window){
		circle.setPosition({x, y});
		window.draw(circle);
	};
};



sf::Vector2f minAndMaxOnAxis(Rectangle& r, sf::Vector2f axis){
	std::vector<sf::Vector2f> corners = r.getCorners();
	float corner1 = dotProduct(corners[0], axis);
	float corner2 = dotProduct(corners[1], axis);
	float corner3 = dotProduct(corners[2], axis);
	float corner4 = dotProduct(corners[3], axis);
	return sf::Vector2f(std::min({corner1, corner2, corner3, corner4}), std::max({corner1, corner2, corner3, corner4}));
}
sf::Vector2f minAndMaxOnAxis(Circle& c, sf::Vector2f axis){
	float projection = dotProduct({c.x, c.y}, axis);
	return sf::Vector2f(projection - c.radius, projection + c.radius);
}


bool detectCollision(Rectangle& r1, Rectangle& r2){
	std::vector<sf::Vector2f> normalsR1 = r1.findNormals();
	std::vector<sf::Vector2f> normalsR2 = r2.findNormals();
	bool colliding = true;

	for(sf::Vector2f axis : normalsR1){
		sf::Vector2f r1MinMaxAxis = minAndMaxOnAxis(r1, axis);
		sf::Vector2f r2MinMaxAxis = minAndMaxOnAxis(r2, axis);

		if(!(r2MinMaxAxis.x <= r1MinMaxAxis.y && r1MinMaxAxis.x <= r2MinMaxAxis.y)){
			colliding = false;
			break;
		}
	}
	if(colliding == true){
	for(sf::Vector2f axis : normalsR2){
		sf::Vector2f r1MinMaxAxis = minAndMaxOnAxis(r1, axis);
		sf::Vector2f r2MinMaxAxis = minAndMaxOnAxis(r2, axis);

		if(!(r2MinMaxAxis.x <= r1MinMaxAxis.y && r1MinMaxAxis.x <= r2MinMaxAxis.y)){
			colliding = false;
			break;
		}
	}
	}
	return colliding;
}
bool detectCollision(Circle& c, Rectangle& r){
	std::vector<sf::Vector2f> normalsR = r.findNormals();
	std::vector<sf::Vector2f> corners = r.getCorners();
	std::vector<std::vector<sf::Vector2f>> lines = {{corners[1], corners[0]}, {corners[2], corners[1]}, {corners[3], corners[2]}, {corners[3], corners[1]}};

	sf::Vector2f closestPointReal = {0, 0};

	for(std::vector<sf::Vector2f>& line : lines){
		sf::Vector2f lineVec = {line[1].x - line[0].x, line[1].y - line[0].y};
		sf::Vector2f relativePos = sf::Vector2f{c.x, c.y} - line[0];
		float closestPoint = vectorProjection(relativePos, lineVec);
		if(closestPoint > 1){closestPoint = 1;}
		if(closestPoint < 0){closestPoint = 0;}
		sf::Vector2f closestPointPos = line[0] + closestPoint * lineVec;
		if(distanceSquared({c.x, c.y}, closestPointPos) < distanceSquared({c.x, c.y}, closestPointReal)){closestPointReal = closestPointPos;}
	}

	sf::Vector2f circleAxis = unitVector({c.x - closestPointReal.x, c.y - closestPointReal.y});

	bool colliding = true;

	for(sf::Vector2f axis : normalsR){
		sf::Vector2f rMinMaxAxis = minAndMaxOnAxis(r, axis);
		sf::Vector2f cMinMaxAxis = minAndMaxOnAxis(c, axis);

		if(!(cMinMaxAxis.x <= rMinMaxAxis.y && rMinMaxAxis.x <= cMinMaxAxis.y)){
			colliding = false;
			break;
		}
	}

	sf::Vector2f rMinMaxAxis = minAndMaxOnAxis(r, circleAxis);
	sf::Vector2f cMinMaxAxis = minAndMaxOnAxis(c, circleAxis);
	if(!(cMinMaxAxis.x <= rMinMaxAxis.y && rMinMaxAxis.x <= cMinMaxAxis.y)){
			colliding = false;
		}



	return colliding;
}




int main()
{
	sf::RenderWindow window(sf::VideoMode({1280, 720}), "Collision Tests");
	window.setFramerateLimit(60);

	Rectangle rectangle1(100, 30, 300, 300, 30);
	Rectangle rectangle2(100, 30, 500, 500, -50);
	Circle circle(50, 50, 0, 0, 0, 0, 1, 25);

	sf::CircleShape c1;
	c1.setRadius(5);
	sf::CircleShape c2;
	c2.setRadius(5);
	sf::CircleShape c3;
	c3.setRadius(5);
	sf::CircleShape c4;
	c4.setRadius(5);

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
			rectangle1.y -= 1;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
			rectangle1.y += 1;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
			rectangle1.x -= 1;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
			rectangle1.x += 1;
		}



		window.clear();
		rectangle1.rectangle.setPosition({rectangle1.x, rectangle1.y});
		

		bool collidingR = detectCollision(rectangle1, rectangle2);
		if(collidingR){
			rectangle1.rectangle.setFillColor(sf::Color::Red);
			rectangle2.rectangle.setFillColor(sf::Color::Red);
		}
		else{
			rectangle1.rectangle.setFillColor(sf::Color::White);
			rectangle2.rectangle.setFillColor(sf::Color::White);
		}

		bool collidingC1 = detectCollision(circle, rectangle1);
		if(collidingC1){
			rectangle1.rectangle.setFillColor(sf::Color::Red);
			circle.circle.setFillColor(sf::Color::Red);
		}
		else{
			rectangle1.rectangle.setFillColor(sf::Color::White);
			circle.circle.setFillColor(sf::Color::White);
		}

		bool collidingC2 = detectCollision(circle, rectangle2);
		if(collidingC2){
			rectangle2.rectangle.setFillColor(sf::Color::Red);
			circle.circle.setFillColor(sf::Color::Red);
		}
		else{
			rectangle2.rectangle.setFillColor(sf::Color::White);
			circle.circle.setFillColor(sf::Color::White);
		}

		window.draw(rectangle1.rectangle);
		window.draw(rectangle2.rectangle);
		circle.draw(window);

		//std::cout << c1.getPosition().y << ' ';

		//c1.setPosition(corners[0]);
		//c2.setPosition(corners[1]);
		//c3.setPosition(corners[2]);
		//c4.setPosition(corners[3]);
		
		//window.draw(c1);
		//window.draw(c2);
		//window.draw(c3);
		//window.draw(c4);
		


		window.display();
	}
}
