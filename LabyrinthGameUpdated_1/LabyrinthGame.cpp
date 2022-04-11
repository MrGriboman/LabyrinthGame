#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <Windows.h>

#include "labyrinth.h"


int main() {

	int screenWidth = 120;			        // Console Screen Size X (columns)
	int screenHeight = 40;			        // Console Screen Size Y (rows)
	int mapWidth;				            // World Dimensions
	int mapHeight;
	int coins = 0;
	int coinsNeeded;

	float playerX = 1.75f;			        // Player Start Position
	float playerY = 2.f;
	float fPlayerA = 0.0f;			        // Player Start Rotation
	float fieldOfView = 3.14159f / 3.0f;	// Field of View
	float depth;			                // Maximum rendering distance
	float speed = 5.0f;			            // Walking Speed
	float rotationSpeed = 3.75f;            // Rotation Speed

	std::cout << "1.Start\n2.Exit\n";
	int menu;
	std::cin >> menu;
	switch (menu) {
	case 1:
		std::cout << "\033[2J\033[1;1H";
		break;
	case 2:
		return 0;
		break;
	}

	std::cout << "Choose difficulty:\n1.Easy\n2.Medium\n3.Hard(endless suffering)\n";
	int difficulty;
	std::cin >> difficulty;
	switch (difficulty) {
	case 1:
		mapHeight = 16;
		mapWidth = 16;
		depth = 16.0f;
		coinsNeeded = 3;
		break;
	case 2:
		mapHeight = 25;
		mapWidth = 25;
		depth = 16.0f;
		coinsNeeded = 5;
		break;
	case 3:
		mapHeight = 50;
		mapWidth = 50;
		depth = 50.0f;
		coinsNeeded = 8;
		break;
	default:
		mapHeight = 16;
		mapWidth = 16;
		depth = 16.0f;
		coinsNeeded = 3;
		break;
	}

	// Create Screen Buffer
	wchar_t* screen = new wchar_t[screenWidth * screenHeight];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	// Create Map of world space # = wall block, ' ' = space
	std::wstring map = L"";
	Labyrinth labyrinth(mapHeight, mapWidth);
	labyrinth.genWay();
	labyrinth.money(coinsNeeded);
	map += labyrinth.draw();
	map[mapWidth * mapHeight - 2] = '#';

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	while (true) {

		// Making the movement undependent from the framerate
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> duration = tp2 - tp1;
		tp1 = tp2;
		float elapsedTime = duration.count();

		// Checking winning
		if (playerX > mapHeight) {
			break;
		}

		// Checking coins
		if (coins == coinsNeeded) {
			map[mapWidth * mapHeight - 2] = ' ';
		}

		// Collecting coins
		if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '$') {

			coins++;
			map[(int)playerX * mapWidth + (int)playerY] = ' ';
		}

		// Handle CCW Rotation
		if (GetAsyncKeyState('A') & 0x8000)
			fPlayerA -= (rotationSpeed) * elapsedTime;

		// Handle CW Rotation
		if (GetAsyncKeyState('D') & 0x8000)
			fPlayerA += (rotationSpeed) * elapsedTime;

		// Handle Forwards movement & collision
		if (GetAsyncKeyState('W') & 0x8000) {

			playerX += sinf(fPlayerA) * speed * elapsedTime;;
			playerY += cosf(fPlayerA) * speed * elapsedTime;;
			if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#') {

				playerX -= sinf(fPlayerA) * speed * elapsedTime;
				playerY -= cosf(fPlayerA) * speed * elapsedTime;
			}
		}

		// Handle backwards movement & collision
		if (GetAsyncKeyState('S') & 0x8000) {

			playerX -= sinf(fPlayerA) * speed * elapsedTime;;
			playerY -= cosf(fPlayerA) * speed * elapsedTime;;
			if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#') {

				playerX += sinf(fPlayerA) * speed * elapsedTime;
				playerY += cosf(fPlayerA) * speed * elapsedTime;
			}
		}

		for (int x = 0; x < screenWidth; x++) {

			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fieldOfView / 2.0f) + ((float)x / (float)screenWidth) * fieldOfView;

			// Find distance to wall
			float fStepSize = 0.1f;								
			float distanceToWall = 0.0f;

			bool hitWall = false;		// Set when ray hits wall block
			bool hitCoin = false;       // Set when ray hits a coin
			bool boundary = false;		// Set when ray hits boundary between two wall blocks

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
			while (!hitWall && !hitCoin && distanceToWall < depth) {

				distanceToWall += fStepSize;
				int nTestX = (int)(playerX + fEyeX * distanceToWall);
				int nTestY = (int)(playerY + fEyeY * distanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= mapWidth || nTestY < 0 || nTestY >= mapHeight) {

					hitWall = true;			// Just set distance to maximum depth
					distanceToWall = depth;
				}
				else {

					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map.c_str()[nTestX * mapWidth + nTestY] == '#') {

						// Ray has hit wall
						hitWall = true;

						
						std::vector<std::pair<float, float>> p;

						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++) {

								// Angle between the casted vector and the perfect vector to a corner
								float vy = (float)nTestY + ty - playerY;
								float vx = (float)nTestX + tx - playerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(std::make_pair(d, dot));
							}

						// Sort Pairs from closest to farthest
						sort(p.begin(), p.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right) {return left.first < right.first; });

						// First two are closest
						float bound = 0.01;
						if (acos(p.at(0).second) < bound)
							boundary = true;
						if (acos(p.at(1).second) < bound)
							boundary = true;
					}

					if (map.c_str()[nTestX * mapWidth + nTestY] == '$') {
						hitCoin = true;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
			int floor = screenHeight - ceiling;
			short shade;

			if (hitCoin)
				shade = '$';

			// Shader walls based on distance
			if (hitWall) {

				if (distanceToWall <= depth / 4.0f)
					shade = 0x2588; // Very close	
				else if (distanceToWall < depth / 3.0f)
					shade = 0x2593;
				else if (distanceToWall < depth / 2.0f)
					shade = 0x2592;
				else if (distanceToWall < depth)
					shade = 0x2591;
				else
					shade = ' '; // Too far away

				if (boundary)
					shade = 0x2591; // Corners
			}

			for (int y = 0; y < screenHeight; y++) {

				// Each Row
				if (y <= ceiling)
					screen[y * screenWidth + x] = ' ';
				else if (y > ceiling && y <= floor)
					screen[y * screenWidth + x] = shade;
				else { // Floor				
					shade = '~';
					screen[y * screenWidth + x] = shade;
				}
			}			
		}

		// Display Stats
		swprintf_s(screen, 10, L"Coins = %d", coins);		

		// Display Map
		if (difficulty != 3) {

			for (int nx = 0; nx < mapWidth; nx++)
				for (int ny = 0; ny < mapWidth; ny++) {

					screen[(ny + 1) * screenWidth + nx] = map[ny * mapWidth + nx];
				}
			screen[((int)playerX + 1) * screenWidth + (int)playerY] = 'P';
		}

		// Display Frame
		WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0,0 }, &bytesWritten);
	}

	std::cout << "\033[2J\033[1;1H";
	std::cout << "Congrats! You found the exit";
	return 0;
}