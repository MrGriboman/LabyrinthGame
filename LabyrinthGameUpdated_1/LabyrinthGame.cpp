#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <utility>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdio>
#include <Windows.h>

#include "labyrinth.h"

#define PI 3.14159f

std::string opposite(std::string x) {
	if (x == "L")
		return "R";
	else if (x == "R")
		return "L";
	else if (x == "U")
		return "D";
	else if (x == "D")
		return "U";
}

bool findEnd(const std::wstring map, std::string moves, int start, int end, int size, std::string& path) {
	int pos = start;
	for (int i = 0; i < moves.length(); ++i) {
		if (moves[i] == 'L')
			pos -= 1;
		else if (moves[i] == 'R')
			pos += 1;
		else if (moves[i] == 'U')
			pos -= size;
		else if (moves[i] == 'D')
			pos += size;
	}
	if (pos == end) {
		path += moves;
		return true;
	}
	return false;
}

bool valid(const std::wstring map, std::string moves, int start, int size) {
	int pos = start;
	for (int i = 0; i < moves.length(); ++i) {
		if (moves[i] == 'L')
			pos -= 1;
		else if (moves[i] == 'R')
			pos += 1;
		else if (moves[i] == 'U')
			pos -= size;
		else if (moves[i] == 'D')
			pos += size;
	}
	if (pos < 0 || pos > size * size - 1)
		return false;
	else if (map[pos] == '#')
		return false;
	return true;
}

const std::string& gameCheat(std::wstring &map, int size, int coins, std::string &path) {
	int* dest = new int[coins]();
	int k = 0;
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			if (map[size * i + j] == '$')
				dest[k++] = size * i + j;

	std::string add;
	std::string directions[4]{ "L", "R", "U", "D" };

	for (int i = 0; i < coins + 1; ++i) {
		std::queue<std::string> q;
		q.push("");
		int start = (i) ? dest[i - 1] : size + 1;
		int end = (i == coins) ? size * size - 2 : dest[i];
		while (!findEnd(map, add, start, end, size, path)) {
			add = q.front();
			q.pop();
			for (auto dir : directions) {
				if (&add.back() == opposite(dir))
					continue;
				std::string put = add + dir;
				if (valid(map, put, start, size))
					q.push(put);
			}
		}
	}
	int spot = size + 1;
	for (auto dir : path) {
		int step;
		if (dir == 'L')
			step = -1;
		else if (dir == 'R')
			step = 1;
		else if (dir == 'D')
			step = size;
		else if (dir == 'U')
			step = -size;
		spot += step;
		if (map[spot] != '$')
			map[spot] = '.';
	}
}

void maintainAngle(double &fPlayerA) {
	if (fPlayerA < 0)
		fPlayerA = PI * (double)2.0f + fPlayerA;
	if (fPlayerA > PI * (double)2.0f)
		fPlayerA -= PI * (double)2.0f;
}


int main() {

	int screenWidth = 120;			        // Console Screen Size X (columns)
	int screenHeight = 40;			        // Console Screen Size Y (rows)
	int mapWidth;				            // World Dimensions
	int mapHeight;
	int coins = 0;
	int coinsNeeded;

	double playerX = 1.f;			        // Player Start Position
	double playerY = 1.f;
	double fPlayerA = 0.0f;			        // Player Start Rotation
	double fieldOfView = PI / 3.0f;	        // Field of View
	double depth;			                // Maximum rendering distance
	double speed = 5.0f;			        // Walking Speed
	double rotationSpeed = 3.75f;           // Rotation Speed

	bool cheatMode = false;
	std::string path;

	std::cout << "1.Start\n2.Exit\n";
	int menu;
	std::cin >> menu;
	if (menu == 2)
		return 0;

	std::cout << "Choose difficulty:\n1.Easy\n2.Medium\n3.Hard(endless suffering)\n";
	int difficulty;
	std::cin >> difficulty;
	if (difficulty == 1) {
		mapHeight = 8;
		mapWidth = 8;
		depth = 16.0f;
		coinsNeeded = 1;
	}
	else if (difficulty == 2) {
		mapHeight = 25;
		mapWidth = 25;
		depth = 16.0f;
		coinsNeeded = 2;
	}
	else if (difficulty == 3) {
		mapHeight = 50;
		mapWidth = 50;
		depth = 50.0f;
		coinsNeeded = 3;
	}
	else {
		mapHeight = 8;
		mapWidth = 8;
		depth = 16.0f;
		coinsNeeded = 1;
	}

	std::cout << "Mode: ";
	std::wstring mode;
	std::wcin >> mode;
	if (mode == L"uuddlrlrba")
		cheatMode = true;

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

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	if (cheatMode)
		gameCheat(map, mapHeight, coinsNeeded, path);
	map[mapWidth * mapHeight - 2] = '#';

	while (true) {

		// Making the movement independent of the framerate
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<double> duration = tp2 - tp1;
		tp1 = tp2;
		double elapsedTime = duration.count();

		if (cheatMode) {
			path += 'D';
			static int k = 0;			
			char step = path[k];
		
			if (step == 'R') {				
				while (fabs(fPlayerA) > 0.1) {
					fPlayerA += (rotationSpeed) * 0.01;
					maintainAngle(fPlayerA);					
				}
				playerY += 1;
				k++;
			}

			if (step == 'L') {			
				while (fabs(fPlayerA - PI) > 0.1) {
					fPlayerA += (rotationSpeed) * 0.01;
					maintainAngle(fPlayerA);					
				}
				playerY -= 1;
				k++;
			}

			if (step == 'U') {
				while (fabs(fPlayerA - ((double)3.0f * PI / (double)2.0f)) > 0.1) {
					fPlayerA += (rotationSpeed) * 0.01;
					maintainAngle(fPlayerA);					
				}	
				playerX -= 1;
				k++;
			}

			if (step == 'D') {
				while (fabs(fPlayerA - (PI / (double)2.0f)) > 0.1) {
					fPlayerA += (rotationSpeed) * 0.01;
					maintainAngle(fPlayerA);					
				}
				playerX += 1;
				k++;
			}
			Sleep(200);
		}

		// Checking winning
		if (playerX > mapHeight) {
			break;
		}
		
		if (coins == coinsNeeded) {
			map[mapWidth * mapHeight - 2] = ' ';
		}

		// Collecting coins
		if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '$') {

			coins++;
			map[(int)playerX * mapWidth + (int)playerY] = ' ';
		}

		// Handle CCW Rotation
		if (GetAsyncKeyState('A') & 0x8000 && !cheatMode)
			fPlayerA -= (rotationSpeed)*elapsedTime;

		// Handle CW Rotation
		if (GetAsyncKeyState('D') & 0x8000 && !cheatMode)
			fPlayerA += (rotationSpeed)*elapsedTime;

		// Handle Forwards movement & collision
		if (GetAsyncKeyState('W') & 0x8000 && !cheatMode) {

			playerX += sinf(fPlayerA) * speed * elapsedTime;
			playerY += cosf(fPlayerA) * speed * elapsedTime;
			if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#') {

				playerX -= sinf(fPlayerA) * speed * elapsedTime;
				playerY -= cosf(fPlayerA) * speed * elapsedTime;
			}
		}

		// Handle backwards movement & collision
		if (GetAsyncKeyState('S') & 0x8000 && !cheatMode) {

			playerX -= sinf(fPlayerA) * speed * elapsedTime;
			playerY -= cosf(fPlayerA) * speed * elapsedTime;
			if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#') {

				playerX += sinf(fPlayerA) * speed * elapsedTime;
				playerY += cosf(fPlayerA) * speed * elapsedTime;
			}
		}

		// Maintain camera angle
		maintainAngle(fPlayerA);
		
		for (int x = 0; x < screenWidth; x++) {

			// For each column, calculate the projected ray angle into world space
			double fRayAngle = (fPlayerA - fieldOfView / 2.0f) + ((double)x / (double)screenWidth) * fieldOfView;

			// Find distance to wall
			double fStepSize = 0.1f;
			double distanceToWall = 0.0f;

			bool hitWall = false;		// Set when ray hits wall block
			bool hitCoin = false;       // Set when ray hits a coin
			bool boundary = false;		// Set when ray hits boundary between two wall blocks

			double fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			double fEyeY = cosf(fRayAngle);

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


						std::vector<std::pair<double, double>> p;

						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++) {

								// Angle between the casted vector and the perfect vector to a corner
								double vy = (double)nTestY + ty - playerY;
								double vx = (double)nTestX + tx - playerX;
								double d = sqrt(vx * vx + vy * vy);
								double dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(std::make_pair(d, dot));
							}

						// Sort Pairs from closest to farthest
						sort(p.begin(),
							p.end(),
							[](const std::pair<double, double>& left, const std::pair<double, double>& right) {return left.first < right.first; }
						);

						// First two are closest
						double bound = 0.01;
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
			int ceiling = (double)(screenHeight / 2.0) - screenHeight / ((double)distanceToWall);
			int floor = screenHeight - ceiling;
			short shade;

			//Hit coin
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
		swprintf_s(screen, 100, L"Coins = %d angle = %f X = %f Y = %f", coins, fPlayerA, playerX, playerY);

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

	std::cout << ((!cheatMode) ? "Congrats! You found the exit" : "Congrats! A bot found the exit for you!");
	return 0;
}
