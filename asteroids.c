#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

const int height = 800;
const int width = 800;
const int fps = 60;

const Color bg = BLACK;
const Color fg = GREEN;

const float NOSE = PI/8;
const int LENGTH = 36;
const float THRUST = 0.1;
const int SKY_SIZE = 256;

Vector2 pos = {(float) height / 2, (float) width / 2};
Vector2 vel = {0.0f, 0.0f};
float angle = 0;

Vector2 * sky;

Vector2 * generateSky() {
	Vector2 s[SKY_SIZE];
	for (int i = 0; i < SKY_SIZE; i++) {
		s[i] = (Vector2){rand() % width, rand() % height};
	} 
	return s;
}

Vector2 wingpos(int w) {
	// optimise this maths
	int x, y;
	float a;
	
	if (w > 0) a = angle + (NOSE / 2.0f);
	else if (w < 0) a = angle - (NOSE / 2.0f);

	x = pos.x - LENGTH * sin(a);
	y = pos.y - LENGTH * cos(a);
	return (Vector2){x, y};
}

int accelerate(float m) {
	vel.x += sin(angle) * m;
	vel.y += cos(angle) * m;
}

int drawPlayer() {
	DrawTriangleLines(
		pos,
		wingpos(-1),
		wingpos(1),	
		fg
	);
}

int draw() {
	BeginDrawing();
	ClearBackground(BLACK);
	
	drawPlayer();
	EndDrawing();
}

int update() {
	if (IsKeyDown(KEY_RIGHT)) angle -= 0.1f;
	if (IsKeyDown(KEY_LEFT)) angle += 0.1f;
	
	if (IsKeyDown(KEY_UP)) accelerate(THRUST);


	pos.x += vel.x;
	pos.y += vel.y;
}

int main() {
	sky = generateSky();
	InitWindow(width, height, "game");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		update();
		draw();
	}
	CloseWindow();
	return 0;
}
