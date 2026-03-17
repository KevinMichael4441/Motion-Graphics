// platform game.....
// Author :Noel O' Hara

#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h> 



class Particle
{
public:
	int timetoLive;
	sf::Vector2f velocity;
	sf::RectangleShape shape;

	void Draw(sf::RenderWindow& win)
	{
		if (timetoLive > 0)
		{
			win.draw(shape);
		}
	}
	void Update()
	{
		if (timetoLive > 0)
		{
			shape.move(velocity);

			timetoLive--;
		}
	}

	Particle() {}

	Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color t_color)
	{
		shape.setSize(sf::Vector2f(3, 3));
		shape.setPosition(pos);
		shape.setFillColor(t_color);
		velocity = vel;
		timetoLive = rand() % 50;
	}
};


enum class ParticleType {JUMP, SPEED};


#define maxParticles 50
class ParticleSystem
{
public:

	Particle particles[maxParticles];
	sf::Vector2f position;
	
	void Initialise(sf::Vector2f pos, ParticleType t_type)
	{
		position = pos;

		if (t_type == ParticleType::JUMP)
		{
			for (int i = 0; i < maxParticles; i++)
			{
				particles[i] = Particle(position, sf::Vector2f(rand() / double(RAND_MAX) * 4 - 2, rand() / double(RAND_MAX) * 4 - 2), sf::Color::Yellow);
			}
		}
		else if (t_type == ParticleType::SPEED)
		{
			for (int i = 0; i < maxParticles; i++)
			{
				particles[i] = Particle(position, sf::Vector2f(rand() / double(RAND_MAX) * 4 - 4, rand() / double(RAND_MAX) * 4 - 2), sf::Color::White);
			}
		}

	}
	
	void Update()
	{
		for (int i = 0; i < maxParticles; i++)
		{

			particles[i].Update();


		}
	}
	
	void Draw(sf::RenderWindow& win)
	{
		for (int i = 0; i < maxParticles; i++)
		{
			particles[i].Draw(win);
		}
	}

	void reset()
	{
		for (int i = 0; i < maxParticles; i++)
		{
			particles[i].timetoLive = 0;
		}
	}

	ParticleSystem() {}
};




static const int MAX_LAVA = 4;

class Game
{
public:
	//create Window
	sf::RenderWindow window;
	sf::View view;
	float randomNum;


	sf::RectangleShape playerShape;


	float velocityX = 0, velocityY = 0, gravity = 0.3;

	float horizontalSpeed = -3.7;
	const float speedDelay = 2.0f;
	float speedTimer = 0.0f;
	bool speedTimerActive = false;

	float lavaSpeeds[MAX_LAVA];

	static const int numRows = 20;
	static const int numCols = 45;
	int levelData[numRows][numCols] =
	{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,6,0,0,0,0,0,0,1,0,0,0,0,0,0,3},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,6,0,0,0,0,1,0,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,3},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,4,1,0,0,0,0,1,0,0,0,0,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,4,0,0,0,1,1,1,0,0,0,0,0,0,1,0,1,0,0,0,1,1,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,0,0,6,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,6,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	};

	sf::RectangleShape level[numRows][numCols];


	ParticleSystem particleSystem;

	Game()
	{

		window.create(sf::VideoMode({ 800, 600 }), "Endless Runner Game");
	}
	void init()
	{

		particleSystem.reset();
		speedTimer = 0.0f;
		horizontalSpeed = -3.7;

		velocityX = 0, velocityY = 0, gravity = 0.3;

		for (int index = 0; index < MAX_LAVA; index++)
		{
			lavaSpeeds[index] = horizontalSpeed * 2;
		}

		speedTimerActive = false;

		view = window.getDefaultView();
		playerShape.setSize(sf::Vector2f(20, 20));
		playerShape.setPosition(sf::Vector2f(160, 500));

		for (int row = 0; row < numRows; row++)
		{
			for (int col = 0; col < numCols; col++)
			{

				if (levelData[row][col] == 1)
				{

					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));
					level[row][col].setFillColor(sf::Color::Green);
				}
				if (levelData[row][col] == 0)
				{

					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));
					level[row][col].setFillColor(sf::Color::Black);
				}
				if (levelData[row][col] == 2)
				{
					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));

					level[row][col].setFillColor(sf::Color::Blue);

				}
				if (levelData[row][col] == 3)
				{
					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));

					level[row][col].setFillColor(sf::Color::Magenta);

				}
				if (levelData[row][col] == 4)
				{
					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));

					level[row][col].setFillColor(sf::Color::Yellow);

				}

				if (levelData[row][col] == 5)
				{
					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));

					level[row][col].setFillColor(sf::Color::Cyan);

				}

				if (levelData[row][col] == 6)
				{
					level[row][col].setSize(sf::Vector2f(70, 30));
					level[row][col].setPosition(sf::Vector2f(col * 70, row * 30));

					level[row][col].setFillColor(sf::Color::Red);

				}


			}
			std::cout << std::endl;
		}

	}
	void run()
	{


		sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);


		sf::Time timeSinceLastUpdate = sf::Time::Zero;


		sf::Clock clock;
		clock.restart();

		while (window.isOpen())
		{

			// check if the close window button is clicked on. 
			while (const std::optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>())
				{
					window.close();
				}
				else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
						window.close();
				}
			}

			timeSinceLastUpdate += clock.restart();


			if (timeSinceLastUpdate > timePerFrame)
			{

				for (int row = 0; row < numRows; row++)
				{
					for (int col = 0; col < numCols; col++)
					{

						level[row][col].move(sf::Vector2f(horizontalSpeed, 0));
					}

				}


				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space) && velocityY == 0)
				{
					velocityY = -11.8;
				}

				velocityY = velocityY + gravity;
				playerShape.move(sf::Vector2f(0, velocityY));


				gravity = 0.6;


				if (speedTimerActive)
				{
					speedTimer += timeSinceLastUpdate.asSeconds();
					std::cout << speedTimer << "\n";

					if (speedTimer > speedDelay)
					{
						speedTimer = 0;
						speedTimerActive = false;
						horizontalSpeed = -3.7;
						particleSystem.Initialise(playerShape.getPosition(), ParticleType::SPEED);
					}
				}

				int lavaIndex = 0;
				for (int row = 0; row < numRows; row++)
				{
					for (int col = 0; col < numCols; col++)
					{
						if (levelData[row][col] == 6)
						{
							sf::RectangleShape *toCollideWith = &level[row][col];
							level[row][col].move({ lavaSpeeds[lavaIndex], 0});
							if (lavaSpeeds[lavaIndex] < 0)
							{
								if (col != 0)
								{	
									for (int index = col; index > 0; index--)
									{
										if (levelData[row][index] == 1)
										{
											toCollideWith = &level[row][index];
											break;
										}
									}
									
									if (level[row][col].getGlobalBounds().findIntersection(toCollideWith->getGlobalBounds()))
									{
										lavaSpeeds[lavaIndex] *= -1;
									}
									

								}
								else if (col == 0)
								{
									lavaSpeeds[lavaIndex] *= -1;
								}								
							}
							else
							{
								if (col != numCols - 1)
								{
									for (int index = col; index < numCols; index++)
									{
										if (levelData[row][index] == 1)
										{
											toCollideWith = &level[row][index];
											break;
										}
									}

									if (level[row][col].getGlobalBounds().findIntersection(toCollideWith->getGlobalBounds()))
									{
										lavaSpeeds[lavaIndex] *= -1;
									}
								}
								else if (col == numCols - 1)
								{
									lavaSpeeds[lavaIndex] *= -1;
								}
							}
							
							lavaIndex++;
						}


						if (velocityY >= 0)
						{
							if (levelData[row][col] == 1)
							{

								if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
								{
									if (playerShape.getPosition().y < level[row][col].getPosition().y)
									{
										gravity = 0;
										velocityY = 0;
										playerShape.setPosition(sf::Vector2f(playerShape.getPosition().x, level[row][col].getPosition().y));
										playerShape.move(sf::Vector2f(0, -playerShape.getGlobalBounds().size.y));
										break;
									}
									else {
										init();
									}
								}


							}

						}
						if (velocityY < 0)
						{
							if (levelData[row][col] == 1)
							{
								if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
								{
									init();
								}

							}

						}
						if (levelData[row][col] == 2)
						{
							if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
							{
								init();
							}
						}
						if (levelData[row][col] == 3)
						{
							if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
							{
								window.close();
							}
						}
						if (levelData[row][col] == 4)
						{
							if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
							{
								velocityY = -11.8;
								particleSystem.Initialise(playerShape.getPosition(), ParticleType::JUMP);
							}
						}

						if (levelData[row][col] == 5)
						{
							if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
							{
								speedTimerActive = true;
								horizontalSpeed = -2.0;
							}
						}

						if (levelData[row][col] == 6)
						{
							if (playerShape.getGlobalBounds().findIntersection(level[row][col].getGlobalBounds()))
							{
								init();
							}
						}

						
					}
				}

				particleSystem.Update();

				if (playerShape.getPosition().y > 600)
				{
					init();
				}

				window.clear();

				for (int row = 0; row < numRows; row++)
				{
					for (int col = 0; col < numCols; col++)
					{
						window.draw(level[row][col]);
					}
				}

				for (int row = 0; row < numRows; row++)
				{
					for (int col = 0; col < numCols; col++)
					{
						if (levelData[row][col] == 6)
						window.draw(level[row][col]);
					}
				}
				window.draw(playerShape);

				particleSystem.Draw(window);

				window.display();

				timeSinceLastUpdate = sf::Time::Zero;
			}
		}
	}
};


int main()
{
	Game game;

	game.init();
	game.run();

	return 0;
}
