#include "SFML.h"

#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <sstream>
#include <math.h>

using namespace sf;

struct target
{
	RectangleShape shape;
	float speed;
	Vector2i direction;
	int id;

	static int counter;

	target()
		: speed(0), direction(Vector2i(0, 0)), id(counter)
	{
		shape.setSize(Vector2f(50.f, 50.f));
		shape.setFillColor(Color::White);
		shape.setPosition(400.f, 300.f);
		counter++;

		std::stringstream ss;

		ss << id;
	}
};

struct Projectile
{
	CircleShape shape;
	float speed;
	Vector2f direction;

	Projectile()
		: speed(5), direction(Vector2f(0.0f, 0.0f))
	{
		shape.setRadius(5.f);
		shape.setFillColor(Color::Red);
	}

};

int target::counter = 0;

void spawnTarget(std::vector<target>& list, RenderWindow& window)
{
	target newTarget;
	newTarget.shape.setPosition(rand() % (window.getSize().x - (int)newTarget.shape.getGlobalBounds().width + 1), rand() % (window.getSize().y - (int)newTarget.shape.getGlobalBounds().height + 1));

	int ways[2] = { -1, 1 };

	newTarget.speed = 2.0f;
	newTarget.direction.x = ways[rand() % 2];
	newTarget.direction.y = ways[rand() % 2];

	list.push_back(newTarget);
}

void spawnProjectile(std::vector<Projectile>& list, RenderWindow& window, CircleShape& player, Vector2f targetPos)
{
	std::cout << "SHOOT!\n";
	std::cout << targetPos.x << " " << targetPos.y << std::endl;

	Projectile newProjectile;

	newProjectile.shape.setPosition(player.getPosition().x + player.getRadius(), player.getPosition().y + player.getRadius());

	float xdiff = targetPos.x - newProjectile.shape.getPosition().x;
	float ydiff = targetPos.y - newProjectile.shape.getPosition().y;

	float distance = std::sqrt(std::pow(std::abs(xdiff), 2) + std::pow(std::abs(ydiff), 2));

	newProjectile.direction.x = xdiff / distance;
	newProjectile.direction.y = ydiff / distance;

	list.push_back(newProjectile);
}

int main()
{
	srand(time(NULL));

	VideoMode mainVideoMode(800, 600);
	RenderWindow window(mainVideoMode, "SFML GAME");
	window.setFramerateLimit(60);

	FloatRect windowBounds(0.0f, 0.0f, mainVideoMode.width, mainVideoMode.height);

	Event ev;

	int spawnTimer = 0;
	int spawnTimerMax = 60;
	int maxTarget = 10;

	bool spaceHeld = false;

	int shootTimer = 10;

	std::vector<target> targets;
	std::vector<Projectile> projectiles;

	CircleShape player;
	player.setRadius(25.f);
	player.setFillColor(Color::Green);
	player.setPosition(350, 250);

	int playerSpeed = 10;

	while (window.isOpen())
	{
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed || (ev.KeyPressed && ev.key.code == Keyboard::Escape))
				window.close();
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				std::cout << projectiles.size() << std::endl;

				if (!spaceHeld)
				{
					spaceHeld = true;
					spawnTarget(targets, window);
				}
			}
			else
			{
				spaceHeld = false;
			}
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (shootTimer < 5)
					shootTimer++;

				if (shootTimer >= 2)
				{
					spawnProjectile(projectiles, window, player, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
					shootTimer = 0;
				}
			}

		}

		if (Keyboard::isKeyPressed(Keyboard::A) && player.getPosition().x > 0)
		{
			player.move(playerSpeed * -1.f, 0.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::D) && player.getPosition().x + 2 * player.getRadius() < window.getSize().x)
		{
			player.move(playerSpeed * 1.f, 0.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::W) && player.getPosition().y > 0)
		{
			player.move(0.f, playerSpeed * -1.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::S) && player.getPosition().y + 2 * player.getRadius() < window.getSize().y)
		{
			player.move(0.f, playerSpeed * 1.f);
		}

		// Update

		// Spawn targets

		if (spawnTimer < spawnTimerMax)
		{
			spawnTimer++;
		}
		else if (targets.size() < maxTarget)
		{
			spawnTimer = 0;
			spawnTarget(targets, window);
		}



		// Move targets
		for (auto& t : targets)
		{
			t.shape.move(t.speed * t.direction.x, t.speed * t.direction.y);

			if (t.shape.getPosition().x <= 0)
			{
				t.direction.x = 1;
			}
			if (t.shape.getPosition().x >= window.getSize().x - t.shape.getGlobalBounds().width)
			{
				t.direction.x = -1;
			}
			if (t.shape.getPosition().y <= 0)
			{
				t.direction.y = 1;
			}
			if (t.shape.getPosition().y >= window.getSize().y - t.shape.getGlobalBounds().height)
			{
				t.direction.y = -1;
			}

		}

		// Move projectiles
		bool delProj = false;
		for (size_t i = 0; i < projectiles.size(); i++)
		{
			delProj = false;
			projectiles[i].shape.move(projectiles[i].speed * projectiles[i].direction.x, projectiles[i].speed * projectiles[i].direction.y);

			for (size_t j = 0; j < targets.size(); j++)
			{
				if (projectiles[i].shape.getGlobalBounds().intersects(targets[j].shape.getGlobalBounds())) // Projectile shoot a target!
				{
					targets.erase(targets.begin() + j);
					j--;
					delProj = true;
					break;
				}
			}

			if (!delProj && !projectiles[i].shape.getGlobalBounds().intersects(windowBounds)) // Projectile gone out of window
			{
				delProj = true;
			}

			if (delProj)
			{
				projectiles.erase(projectiles.begin() + i);
				i--;
			}
		}

		// Render

		window.clear();

		// Draw targets
		for (auto& t : targets)
			window.draw(t.shape);

		window.draw(player);

		// Draw Projectiles
		for (auto& p : projectiles)
			window.draw(p.shape);

		window.display();
	}

	return 0;
}