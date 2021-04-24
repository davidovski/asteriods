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

const float THRUST = 0.04f;
const float RUDDER = 0.05f;
const float NOSE = PI/8;
const int LENGTH = 36;
const int SPAWN_RATE = 10*60;
const float S_SPEED = 4.0f;
const int LAZER_LENGTH = 3;
const int MAX_SIZE=5;

typedef struct B {
	Vector2 pos;
	Vector2 vel;
	int size;
} Asteroid, Bullet;

Vector2 sky[512];
Asteroid asteroids[128];
Bullet bullets[16];

bool alive = 1;
Vector2 pos;
Vector2 vel;
float angle;

int l = 0;


int clamp(Vector2 *vec) {
	if (vec->x > width+LENGTH) vec->x -= width+LENGTH;
	if (vec->x < -LENGTH) vec->x += width+LENGTH*2;
	if (vec->y > height+LENGTH) vec->y -= height+LENGTH;
	if (vec->y < -LENGTH) vec->y += height+LENGTH*2;
} 

double magnitude(Vector2 *vec) {
	return sqrt(pow(vec->x, 2) + pow(vec->y, 2));
}


int randVec(Vector2 *vec, float m) {
		vec->x = rand() % 256;
		vec->y = rand() % 256;
		double mag = magnitude(vec);
		vec->x /= mag;
		vec->y /= mag;
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

int shoot() {
	Vector2 position = pos;
	Vector2 velocity = (Vector2){
		vel.x + S_SPEED * sin(angle),
		vel.y + S_SPEED * cos(angle)
	};

	int i = 0;
	while (i < sizeof(bullets) / sizeof(Bullet)) {
		if (bullets[i].size == 0) {
			bullets[i].pos = position;
			bullets[i].vel = velocity;
			bullets[i].size = 1; 
			break;
		}
		i++;
	}
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
	if (rand()%2) {
		if (rand()%2) s = (Vector2){0, rand() % height};
		else  	      s = (Vector2){width, rand() % height};
	} else {
		if (rand()%2) s = (Vector2){rand() % width, 0};
		else	      s = (Vector2){rand() % width, height};
	}
	float n = -sqrt(pow(s.x - (width/2), 2) + pow(s.y - (height/2), 2));
	float m = (float)(rand() % 7 + 1);
	v = (Vector2){
		(s.x-(width/2)) / n,
		(s.y-(height/2)) / n
		};
	addAsteroid(s, v, (rand()%MAX_SIZE)+1);
}

int breakAsteroid(int i) {
	asteroids[i].size -= 1;
	if (asteroids[i].size > 0) {
		// split the velocity in 2, adding a bit extra from the lazer
		double m = (magnitude(&asteroids[i].vel) + 0.3*S_SPEED)*0.5;
		randVec(&asteroids[i].vel, m);
		int s = pow(2, asteroids[i].size + 1);
		addAsteroid(
			(Vector2){asteroids[i].pos.x - s, asteroids[i].pos.y - s},
			(Vector2){-asteroids[i].vel.x, -asteroids[i].vel.y},
			asteroids[i].size
		);
		asteroids[i].pos.x += s;
		asteroids[i].pos.y += s;
	}
}

int collideAsteroids(Asteroid *a, Asteroid *b) {

	int aa = pow(pow(2, a->size+1), 2);
	int ba = pow(pow(2, b->size+1), 2);
	int ta = aa+ba;

	const int x = a->pos.x - b->pos.x;
	const int y = a->pos.y - b->pos.y;
	const int d = x * x + y * y;

	const double u1 = (a->vel.x * x + a->vel.y * y) / d; // a to b
	const double u2 = (x * a->vel.y - y * a->vel.x) / d; // adjust a along tangent
	const double u3 = (b->vel.x * x + b->vel.y * y) / d; // from b to a
	const double u4 = (x * b->vel.y - y * b->vel.x) / d; // adjust b along tangent

	// adjust to work with different masses;
	const double vu3 = ((aa - ba) / ta) * u1 + (2 * ba) / ta * u3;
	const double vu1 = ((ba - aa) / ta) * u3 + (2 * aa) / ta * u1;
	
	// set new velocity
	b->vel.x = x * vu1 - y * u4;
	b->vel.y = y * vu1 + x * u4;
	a->vel.x = x * vu3 - y * u2;
	a->vel.y = y * vu3 + x * u2;
}

int reset() {
	for (int i = 0; i < 256; i++) {
		sky[i] = (Vector2){rand() % width, rand() % height};
	}
	for (int i=0; i < sizeof(asteroids) / sizeof(Asteroid); i++) {
		asteroids[i] = (Asteroid){(Vector2){0,0}, (Vector2){0,0}, 0};
	}
	for (int i=0; i < sizeof(bullets) / sizeof(Bullet); i++) {
		bullets[i] = (Bullet){(Vector2){0,0}, (Vector2){0,0}, 0};
	}
	for (int i = 0; i < (rand()%10) + 3; i++) {
		spawnAsteroid();
	}
	alive = 1;
	pos = (Vector2){(float) width / 2, (float) height / 2};
	vel = (Vector2){0.0f, 0.0f};
	angle = 0;
}

int draw() {
	BeginDrawing();
	ClearBackground(BLACK);
	
	for (int i = 0; i < 256; i++) {
		DrawPixelV(sky[i], fg);
	}

	if (alive) {
		if (IsKeyDown(KEY_RIGHT)) angle -= RUDDER;
		if (IsKeyDown(KEY_LEFT)) angle += RUDDER;
	
		if (IsKeyDown(KEY_UP)) {
			vel.x += sin(angle) * THRUST;
			vel.y += cos(angle) * THRUST;
		}

		pos.x += vel.x;
		pos.y += vel.y;
		clamp(&pos);
		drawPlayer();
	}	

	if (IsKeyDown(KEY_SPACE)) {
		if (!l) {
			l = 1;
			if (alive) shoot();
			else {
				reset();
				return 0;
			}
		}
	} else {
		l = 0;
	}

	if (rand() % SPAWN_RATE == 0) {
		spawnAsteroid();
	}
	
	for (int i=0; i < sizeof(bullets) / sizeof(Bullet); i++) {
		Bullet *b = &bullets[i];
		if (b->size > 0) {
			DrawLineV( b->pos,
				(Vector2){b->pos.x - LAZER_LENGTH*b->vel.x, b->pos.y - LAZER_LENGTH*b->vel.y},
				fg
			);
			b->pos.x += b->vel.x;
			b->pos.y += b->vel.y;
			clamp(&b->pos);

			for (int j=0; j < sizeof(asteroids) / sizeof(Asteroid); j++) {
				 if (CheckCollisionPointCircle(b->pos, asteroids[j].pos, pow(2, asteroids[j].size + 1))) {
					b->size=0;
					breakAsteroid(j);
					break;
				 }
			}
			if (CheckCollisionPointTriangle(b->pos, pos, wingpos(1), wingpos(-1))) {
				alive = 0;
			}
		}
	}

	for (int i=0; i < sizeof(asteroids) / sizeof(Asteroid); i++) {
		Asteroid *asteroid = &asteroids[i];
		if (asteroid->size > 0) {
			int csize = pow(2, asteroid->size + 1); 
			DrawCircleLines(

				asteroid->pos.x,
				asteroid->pos.y,
				csize,	
				fg
			);

			for (int j=0; j < sizeof(asteroids) / sizeof(Asteroid); j++) {
				if (i!=j) {
					Asteroid *other = &asteroids[j];
					if (CheckCollisionCircles(asteroid->pos, pow(2, asteroid->size + 1),
						other->pos, pow(2, other->size + 1))) {
						collideAsteroids(asteroid, other);
						break;
					}
				}	
			}

			// Doesnt necessarily catch all collision cases :/
			if (
				   CheckCollisionPointCircle(pos, asteroid->pos, csize)
				|| CheckCollisionPointCircle(wingpos(-1), asteroid->pos, csize)
				|| CheckCollisionPointCircle(wingpos(1), asteroid->pos, csize)
				|| CheckCollisionPointTriangle(asteroid->pos, pos, wingpos(1), wingpos(2))
			) {
				alive = 0;
			}
			
			asteroid->pos.x += asteroid->vel.x;
			asteroid->pos.y += asteroid->vel.y;
			clamp(&asteroid->pos);
		}
	}
	EndDrawing();
}

int main() {
	srand(time(0));
	reset();
	InitWindow(width, height, "game");

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		draw();
	}
	CloseWindow();
	return 0;
}
