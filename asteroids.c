#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

const int width = 800;
const int height = 800;
const int fps = 60;

const Color bg = BLACK;
const Color fg = GREEN;

const float NOSE = PI/8;
const int LENGTH = 36;
const float THRUST = 0.1;


typedef struct Asteroid{
	Vector2 pos;
	Vector2 velocity;
	int size
};

Vector2 sky[256];
Vector2 asteroids[64];

Vector2 pos = {(float) height / 2, (float) width / 2};
Vector2 vel = {0.0f, 0.0f};
float angle = 0;


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


int draw() {
	BeginDrawing();
	ClearBackground(BLACK);
	
	for (int i = 0; i < 256; i++) {
		DrawPixelV(sky[i], fg);
	}
	drawPlayer();
	EndDrawing();
}

int clamp(Vector2 *vec) {
	if (vec->x > width+LENGTH) vec->x -= width+LENGTH;
	if (vec->x < -LENGTH) vec->x += width;
	if (vec->y > height+LENGTH) vec->y -= height+LENGTH;
	if (vec->y < -LENGTH) vec->y += height;
} 

int update() {
	if (IsKeyDown(KEY_RIGHT)) angle -= 0.1f;
	if (IsKeyDown(KEY_LEFT)) angle += 0.1f;
	
	if (IsKeyDown(KEY_UP)) {
		vel.x += sin(angle) * THRUST;
		vel.y += cos(angle) * THRUST;
	}

	pos.x += vel.x;
	pos.y += vel.y;
	clamp(&pos);
}

int main() {
	for (int i = 0; i < 256; i++) {
		sky[i] = (Vector2){rand() % width, rand() % height};
	}

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
