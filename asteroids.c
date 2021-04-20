#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <time.h>


const int width = 800;
const int height = 800;
const int fps = 60;

const Color bg = BLACK;
const Color fg = GREEN;

const float NOSE = PI/8;
const int LENGTH = 36;
const float THRUST = 0.1;
const int SPAWN_RATE = 20*60;

typedef struct Asteroids {
	Vector2 pos;
	Vector2 vel;
	int size;
} Asteroid;

Vector2 sky[256];
Asteroid asteroids[64];

Vector2 pos = {(float) height / 2, (float) width / 2};
Vector2 vel = {0.0f, 0.0f};
float angle = 0;

int clamp(Vector2 *vec) {
	if (vec->x > width+LENGTH) vec->x -= width+LENGTH;
	if (vec->x < -LENGTH) vec->x += width+LENGTH;
	if (vec->y > height+LENGTH) vec->y -= height+LENGTH;
	if (vec->y < -LENGTH) vec->y += height+LENGTH;
} 

double magnitude(Vector2 *vec) {
	return sqrt(pow(vec->x, 2) + pow(vec->y, 2));
}


int randVec(Vector2 *vec, int m) {
		vec->x = rand() % 256;
		vec->y = rand() % 256;
		vec->x /= magnitude(vec);
		vec->y /= magnitude(vec);
		vec->x *= m;
		vec->y *= m;
}


Vector2 wingpos(int w) {
	int x, y;
	float a;
	
	if (w > 0) a = angle + (NOSE / 2.0f);
	else if (w < 0) a = angle - (NOSE / 2.0f);

	x = pos.x - LENGTH * sin(a);
	y = pos.y - LENGTH * cos(a);
	return (Vector2){x, y};
}

int drawPlayer() {
	DrawTriangleLines(
		pos,
		wingpos(-1),
		wingpos(1),
		fg
	);
}

int addAsteroid(Vector2 position, Vector2 velocity, int size) {
	int i = 0;
	while (i < sizeof(asteroids) / sizeof(Asteroid)) {
		if (asteroids[i].size == 0) {
			asteroids[i].pos.x = position.x;
			asteroids[i].pos.y = position.y;
			asteroids[i].vel.x = velocity.x;
			asteroids[i].vel.y = velocity.y;
			asteroids[i].size = size;
			return 0;
		}
		i++;
	}
	return 1;
}

int spawnAsteroid() {
	Vector2 s,v;
	if (rand() % 2 == 0) {
		s = (Vector2){0, rand() % height};
	} else {
		s = (Vector2){rand() % width, 0};
	}
	float n = -sqrt(pow(s.x - (width/2), 2) + pow(s.y - (height/2), 2));
	float m = (float)(rand() % 4 + 1);
	v = (Vector2){
		(s.x-(width/2)) / n,
		(s.y-(height/2)) / n
		};
	addAsteroid(s, v, (rand()%4)+1);
}

int breakAsteroid(int i) {
	asteroids[i].size -= 1;
	if (asteroids[i].size > 0) {
		double m = magnitude(&asteroids[i].vel)/0.5d;
		randVec(&asteroids[i].vel, m);
		addAsteroid(asteroids[i].pos, (Vector2){-asteroids[i].vel.x -asteroids[i].vel.y}, asteroids[i].size);
	}
}
//DEBUG HERE
static int lpress = 0;
//

int draw() {
	if (IsKeyDown(KEY_RIGHT)) angle -= 0.1f;
	if (IsKeyDown(KEY_LEFT)) angle += 0.1f;
	
	if (IsKeyDown(KEY_UP)) {
		vel.x += sin(angle) * THRUST;
		vel.y += cos(angle) * THRUST;
	}

	//DEBUG AUTOBREAKING ASTEROIDS
	if (IsKeyDown(KEY_SPACE)) {
		if (lpress == 0) {
		for (int i=0; i < sizeof(asteroids) / sizeof(Asteroid); i++) {
			if (asteroids[i].size > 0) {
				breakAsteroid(i);
				break;
				}
		}
		lpress = 1;
		}
	} else {
		lpress = 0;
	}


	pos.x += vel.x;
	pos.y += vel.y;
	clamp(&pos);
	
	if (rand() % SPAWN_RATE == 0) {
		spawnAsteroid();
	}

	BeginDrawing();
	ClearBackground(BLACK);
	
	for (int i = 0; i < 256; i++) {
		DrawPixelV(sky[i], fg);
	}
	drawPlayer();

	for (int i=0; i < sizeof(asteroids) / sizeof(Asteroid); i++) {
		if (asteroids[i].size > 0) {
			DrawCircleLines(
				asteroids[i].pos.x,
				asteroids[i].pos.y,
				pow(2, asteroids[i].size + 1), 
				fg
			);
			asteroids[i].pos.x += asteroids[i].vel.x;
			asteroids[i].pos.y += asteroids[i].vel.y;
			clamp(&asteroids[i].pos);
		}
	}
	EndDrawing();
}


int update() {
}

int main() {
	srand(time(0));

	for (int i = 0; i < 256; i++) {
		sky[i] = (Vector2){rand() % width, rand() % height};
	}
	for (int i=0; i < sizeof(asteroids) / sizeof(Asteroid); i++) {
		asteroids[i] = (Asteroid){(Vector2){0,0}, (Vector2){0,0}, 0, };
	}

	spawnAsteroid();

	InitWindow(width, height, "game");

	Vector2 m = GetMonitorPosition(1);
	SetWindowPosition(m.x, m.y);
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		update();
		draw();
	}
	CloseWindow();
	return 0;
}
