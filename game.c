#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

#include <stdio.h>

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)
#define MAX_PROJECTILES (100)
#define MAX_ENEMIES (100)

#define WINDOW_TITLE "Window title"

typedef struct Projectile
{
    Vector2 pos;
    Vector2 dir;
    float size;
    float speed;
    int pierce;
} Projectile;

typedef struct Enemy
{
    Vector2 pos;
    float size;
} Enemy;

typedef enum State
{
    STATE_MAIN_MENU,
    STATE_RUNNING,
    STATE_GAME_OVER
}State;

typedef struct GameState
{
    State state;
    float gameTime;
    int kills;

    Vector2 playerPos;
    float playerSize;
    float playerSpeed;
    float shootDelay;
    float shootTime;
    float projSpeed;
    float projSize;
    int projPierce;

    int projCount;
    Projectile projectiles[MAX_PROJECTILES];

    float spawnTime;

    int enemyCount;
    Enemy enemies[MAX_ENEMIES];
}GameState;

static const GameState DefaultState = {
    .state = STATE_MAIN_MENU,
    .playerPos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
    .playerSize = 20.0f,
    .playerSpeed = 50.0f,
    .shootDelay = 0.3f,
    .projSpeed = 100.0f,
    .projSize = 4.0f,
};


void draw_text_centered(const char* text, Vector2 pos, float fontSize)
{
	const Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
    pos.x -= textSize.x / 2.0f;
	DrawText(text, pos.x, pos.y, fontSize, WHITE);
}


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    GameState gameState = DefaultState;
    Texture2D texture = LoadTexture("assets/test.png"); // Check README.md for how this works

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (gameState.state)
        {
            case STATE_MAIN_MENU:
            {
				draw_text_centered("Bullet Hell", (Vector2) { SCREEN_WIDTH / 2.0f, 40.0f }, 80);
				draw_text_centered("Press 'Enter' to start!", (Vector2) { SCREEN_WIDTH / 2.0f, 200.0f }, 20);

				if (IsKeyPressed(KEY_ENTER))
				{
					gameState = DefaultState;
					gameState.state = STATE_RUNNING;
				}
                break;
            }

            case STATE_RUNNING:
            {
				gameState.gameTime += GetFrameTime();

				char killCounter[100] = { 0 };
				sprintf(killCounter, "Kills: %d", gameState.kills);
				draw_text_centered(killCounter, (Vector2){ 40, 30 }, 20);

				// Shoot Proj
				{
					if (gameState.shootTime < gameState.shootDelay)
					{
						gameState.shootTime += GetFrameTime();
					} 

					while (IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
						   gameState.shootTime >= gameState.shootDelay)
					{
						if (gameState.projCount >= MAX_PROJECTILES)
						{
							break;
						}

						Vector2 dir = Vector2Normalize(Vector2Subtract(GetMousePosition(), gameState.playerPos));

						Projectile proj =
						{
							.pos = gameState.playerPos,
							.dir = dir,
							.speed = gameState.projSpeed,
							.size = gameState.projSize,
						};
						gameState.projectiles[gameState.projCount++] = proj;
						gameState.shootTime -= gameState.shootDelay;
					}
				}

				// Move the player
				{
					if (IsKeyDown(KEY_A))
					{
						gameState.playerPos.x -= gameState.playerSpeed * GetFrameTime();
					}
					if (IsKeyDown(KEY_D))
					{
						gameState.playerPos.x += gameState.playerSpeed * GetFrameTime();
					}
					if (IsKeyDown(KEY_W))
					{
						gameState.playerPos.y -= gameState.playerSpeed * GetFrameTime();
					}
					if (IsKeyDown(KEY_S))
					{
						gameState.playerPos.y += gameState.playerSpeed * GetFrameTime();
					}

					DrawCircle(gameState.playerPos.x, gameState.playerPos.y, gameState.playerSize, GREEN);
				}

				// Update Projectiles
				{
					for (int projIdx = 0; projIdx < gameState.projCount; projIdx++)
					{
						Projectile* proj = &gameState.projectiles[projIdx];

						proj->pos.x += proj->dir.x * proj->speed * GetFrameTime();
						proj->pos.y += proj->dir.y * proj->speed * GetFrameTime();

						const Rectangle screenRect = {
							.height = SCREEN_HEIGHT,
							.width = SCREEN_WIDTH,
							.x = 0,
							.y = 0
						};

						if(!CheckCollisionPointRec(proj->pos, screenRect))
						{
							// Replace with last projectile
							*proj = gameState.projectiles[--gameState.projCount];
						}

						DrawCircle(proj->pos.x, proj->pos.y, proj->size, YELLOW);

						// Collide with enemies
						for (int enemyIdx = 0; enemyIdx < gameState.enemyCount; enemyIdx++)
						{
							Enemy* enemy = &gameState.enemies[enemyIdx];

							if (CheckCollisionCircles(enemy->pos, enemy->size, proj->pos, proj->size))
							{
								// Replace with last enemy
								*enemy = gameState.enemies[--gameState.enemyCount];
								proj->pierce--;
								gameState.kills++;

								// Scaling
								if (gameState.kills % 10 == 0)
								{
									gameState.projSpeed += 10;
								}
								if (gameState.kills % 15 == 0)
								{
									gameState.projSize += 1;
								}
								if (gameState.kills % 25 == 0)
								{
									gameState.shootDelay -= 0.01f; // 0.3
								}
								if (gameState.kills % 30 == 0)
								{
									gameState.projPierce += 1;
								}

								if (gameState.kills % 12 == 0)
								{
									gameState.playerSize += 2;
								}


								if (proj->pierce < 0)
								{
									// Replace with last projectile
									*proj = gameState.projectiles[--gameState.projCount];
								}

								break;
							}
						}
					} 
				}

				// Update Enemies
				{
					//Spawn Enemies
					const float spawnFrequency = 1.0f / (gameState.gameTime * 0.1f);
					const float enemySize = 8 - gameState.gameTime / 20.0f; // Get Smaller over time
					const float enemySpeed = 40 + gameState.gameTime / 10.0f;
					gameState.spawnTime += GetFrameTime();

					while (gameState.spawnTime >= spawnFrequency)
					{
						if (gameState.enemyCount >= MAX_ENEMIES)
						{
							break;
						}

						float radians = GetRandomValue(0, 360) * PI / 180.0f;
						Vector2 dir = { cosf(radians), sinf(radians) };
						const float dist = SCREEN_WIDTH / 2.0f;

						Enemy enemy =
						{
							.pos = {gameState.playerPos.x + dir.x * dist, gameState.playerPos.y + dir.y * dist},
							.size = enemySize,
						};
						gameState.enemies[gameState.enemyCount++] = enemy;
						gameState.spawnTime -= spawnFrequency;
					}


					// Update Enemies
					{
						for (int enemyIdx = 0; enemyIdx < gameState.enemyCount; enemyIdx++)
						{
							Enemy* enemy = &gameState.enemies[enemyIdx];

							Vector2 dir = Vector2Normalize(Vector2Subtract(gameState.playerPos, enemy->pos));

							enemy->pos.x += dir.x * enemySpeed * GetFrameTime();
							enemy->pos.y += dir.y * enemySpeed * GetFrameTime();

							// Draw Enemy
							DrawCircle(enemy->pos.x, enemy->pos.y, enemySize, RED);

							// Touch the Player = Game OVER
							if(CheckCollisionCircles(enemy->pos, enemy->size, gameState.playerPos, gameState.playerSize))
							{
								gameState.playerSize -= 10;
								if (gameState.playerSize <= 0)
								{
									gameState.state = STATE_GAME_OVER;
								}

								// Replace with last enemy
								*enemy = gameState.enemies[--gameState.enemyCount];
							}
						}
					}
				}

                break;
            }

            case STATE_GAME_OVER:
            {
				draw_text_centered("DEFEAT", (Vector2) { SCREEN_WIDTH / 2.0f, 40.0f }, 80);
				
				char killCounter[100] = { 0 };
				sprintf(killCounter, "Kills: %d", gameState.kills);
				draw_text_centered(killCounter, (Vector2){ 40, 100.0f }, 20);

				draw_text_centered("Press 'Enter' to restart!", (Vector2) { SCREEN_WIDTH / 2.0f, 200.0f }, 20);

				if (IsKeyPressed(KEY_ENTER))
				{
					gameState = DefaultState;
					gameState.state = STATE_RUNNING;
				}
                break;
                break;
            }
        }

        //const int texture_x = SCREEN_WIDTH / 2 - texture.width / 2;
        //const int texture_y = SCREEN_HEIGHT / 2 - texture.height / 2;
        //DrawTexture(texture, texture_x, texture_y, WHITE);


        EndDrawing();
    }

    CloseWindow();

    return 0;
}