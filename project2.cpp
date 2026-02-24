/**
* Author: Xiling Wang
* Assignment: Pong Clone
* Date due: [02/28/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60;

//constexpr char BG_COLOUR[]  = "#524368ff";

constexpr float CAT_SPD  = 300.0f,
                INIT_SPD = 200.0f;

constexpr int MAX_BALLS = 7;
constexpr int WIN_SCR   = 20;

constexpr char CAT1_FP[] = "images/cat1.png";
constexpr char CAT2_FP[] = "images/cat2.png";
constexpr char BALL_FP[] = "images/ball.png";

AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;

int gCurrBalls = 1;
bool gOnePlayer = false;
bool gGameOver = false;
int gScr1 = 0;
int gScr2 = 0;
bool gBallSpedUp[MAX_BALLS];
float gFlip1 = 1.0f;
float gFlip2 = 1.0f;

Vector2 gCat1Pos = {60.0f, SCREEN_HEIGHT / 2.0f},
        gCat1Move = {0.0f, 0.0f},
        gCat1Size = {28.12f, 87.48},

        gCat2Pos = {SCREEN_WIDTH - 60.0f, SCREEN_HEIGHT / 2.0f},
        gCat2Move = {0.0f, 0.0f},
        gCat2Size = {28.12f, 87.48},

        gBallPos[MAX_BALLS],
        gBallSpd[MAX_BALLS],
        gBallSize = {27.0f, 27.0f},
        gBallHitSize = {21.0f, 21.0f};

Texture2D gCat1Texture;
Texture2D gCat2Texture;
Texture2D gBallTexture;
Texture2D gBgTexture;


bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB);
void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale);
void hitScreenY(Vector2 *pos, const Vector2 *scale);
void changedSpd(int i);
void restartBall(int i);
void restartAll();


bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    return (xDistance < 0.0f && yDistance < 0.0f);
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale, float flipY)
{
    Rectangle textureArea;
    if(flipY == 1.0f)
    {
        textureArea = {0.0f, 0.0f,
            static_cast<float>(texture->width),
            static_cast<float>(texture->height)
        };
    }
    else if(flipY == -1.0f)
    {
        textureArea = {0.0f, static_cast<float>(texture->height),
            static_cast<float>(texture->width),
            -static_cast<float>(texture->height)
        };
    }
    

    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        0.0f, WHITE
    );
}

void hitScreenY(Vector2 *pos, const Vector2 *scale)
{
    float h = scale->y / 2.0f;
    if(pos->y - h < 0.0f) pos->y = h;
    if(pos->y + h > SCREEN_HEIGHT) pos->y = SCREEN_HEIGHT -h;
}

void changedSpd(int i)
{
    if(!gBallSpedUp[i]){
        gBallSpd[i].x *= 2;
        gBallSpd[i].y *= 2;
        gBallSpedUp[i] = true;
    }
}

void restartBall(int i)
{
    float minY = SCREEN_HEIGHT * 0.25f;
    float maxY = SCREEN_HEIGHT * 0.75f;
    float r = (float)GetRandomValue(0, 100) / 100.0f;
    float Y = minY + r * (maxY - minY);
    gBallPos[i] = {SCREEN_WIDTH / 2.0f, Y};
    float s = INIT_SPD / sqrt(2);
    float spdX = (i % 2 == 0) ? s : -s;
    float spdY = (Y < SCREEN_HEIGHT / 2.0f) ? s : -s;
    gBallSpd[i] = {spdX, spdY};
    gBallSpedUp[i] = false;
}

void restartAll()
{
    for (int i = 0; i < MAX_BALLS; i++){
        restartBall(i);
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Catch that yarn ball!");

    gBgTexture = LoadTexture("images/bg.png");
    gCat1Texture = LoadTexture(CAT1_FP);
    gCat2Texture = LoadTexture(CAT2_FP);
    gBallTexture = LoadTexture(BALL_FP);

    restartAll();

    SetTargetFPS(FPS);
}

void processInput(){
    if(IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if(IsKeyPressed(KEY_T)) gOnePlayer = !gOnePlayer;

    if(IsKeyPressed(KEY_ONE))   gCurrBalls = 1;
    if(IsKeyPressed(KEY_TWO))   gCurrBalls = 2;
    if(IsKeyPressed(KEY_THREE)) gCurrBalls = 3;
    if(IsKeyPressed(KEY_FOUR))  gCurrBalls = 4;
    if(IsKeyPressed(KEY_FIVE))  gCurrBalls = 5;
    if(IsKeyPressed(KEY_SIX))   gCurrBalls = 6;
    if(IsKeyPressed(KEY_SEVEN)) gCurrBalls = 7;

    if(gCurrBalls < 1) gCurrBalls = 1;
    if(gCurrBalls > MAX_BALLS) gCurrBalls = MAX_BALLS;

    if(IsKeyPressed(KEY_R))
    {
        gGameOver = false;
        gScr1 = 0;
        gScr2 = 0;
        gCat1Pos.y = SCREEN_HEIGHT / 2.0f;
        gCat2Pos.y = SCREEN_HEIGHT / 2.0f;
        restartAll();
    }

    if(gGameOver) return;

    gCat1Move = {0.0f, 0.0f};
    if(IsKeyDown(KEY_W))
    {
        gCat1Move.y = -1.0f;
        gFlip1 = 1.0f;
    }
    else if(IsKeyDown(KEY_S))
    {
        gCat1Move.y = 1.0f;
        gFlip1 = -1.0f;
    }

    gCat2Move = {0.0f, 0.0f};
    if(IsKeyDown(KEY_UP))
    {
        gCat2Move.y = -1.0f;
        gFlip2 = 1.0f;
    }
    else if(IsKeyDown(KEY_DOWN))
    {
        gCat2Move.y = 1.0f;
        gFlip2 = -1.0f;
    }
}

void update()
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if(gGameOver) return;

    if(gOnePlayer)
    {
        if(gCat1Pos.y < gBallPos[0].y - 10.0f)
        {
            gCat1Move.y = 1.0f;
            gFlip1 = -1.0f;
        }
        else if(gCat1Pos.y > gBallPos[0].y + 10.0f)
        {
            gCat1Move.y = -1.0f;
            gFlip1 = 1.0f;
        }
        else gCat1Move.y = 0.0f;
    }

    gCat1Pos.y += CAT_SPD * gCat1Move.y * deltaTime;
    gCat2Pos.y += CAT_SPD * gCat2Move.y * deltaTime;

    hitScreenY(&gCat1Pos, &gCat1Size);
    hitScreenY(&gCat2Pos, &gCat2Size);

    for (int i = 0; i < gCurrBalls; i++)
    {
        gBallPos[i].x += gBallSpd[i].x * deltaTime;
        gBallPos[i].y += gBallSpd[i].y * deltaTime;

        if(gBallPos[i].y - gBallHitSize.y / 2.0f < 0.0f)
        {
            gBallPos[i].y = gBallHitSize.y / 2.0f;
            gBallSpd[i].y *= -1.0f;
            changedSpd(i);
        }
        else if(gBallPos[i].y + gBallHitSize.y / 2.0f > SCREEN_HEIGHT)
        {
            gBallPos[i].y = SCREEN_HEIGHT - gBallHitSize.y / 2.0f;
            gBallSpd[i].y *= -1.0f;
            changedSpd(i);
        }

        if(isColliding(&gBallPos[i], &gBallHitSize, &gCat1Pos, &gCat1Size))
        {
            gBallPos[i].x = gCat1Pos.x + (gCat1Size.x / 2.0f) + (gBallHitSize.x / 2.0f);
            gBallSpd[i].x *= -1.0f;
            changedSpd(i);
        }

        if(isColliding(&gBallPos[i], &gBallHitSize, &gCat2Pos, &gCat2Size))
        {
            gBallPos[i].x = gCat2Pos.x - (gCat2Size.x / 2.0f) - (gBallHitSize.x / 2.0f);
            gBallSpd[i].x *= -1.0f;
            changedSpd(i);
        }

        if(gBallPos[i].x < 0.0f)
        {
            gScr2++;
            restartBall(i);
        }

        else if(gBallPos[i].x > SCREEN_WIDTH)
        {
            gScr1++;
            restartBall(i);
        }
    }

    if(gScr1 >= WIN_SCR || gScr2 >= WIN_SCR) gGameOver = true;
}

void render()
{
    BeginDrawing();

    Rectangle textureAreaBg = {
        0.0f, 0.0f,
        static_cast<float>(gBgTexture.width),
        static_cast<float>(gBgTexture.height)
    };
    
    Rectangle destinationAreaBg = {
        0.0f, 0.0f,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };
    
    Vector2 originBg = {0.0f, 0.0f};

    DrawTexturePro(
        gBgTexture,
        textureAreaBg,
        destinationAreaBg,
        originBg,
        0.0f,
        WHITE
    );

    renderObject(&gCat1Texture, &gCat1Pos, &gCat1Size, gFlip1);
    renderObject(&gCat2Texture, &gCat2Pos, &gCat2Size, gFlip2);

    for (int i = 0; i < gCurrBalls; i++)
    {
        renderObject(&gBallTexture, &gBallPos[i], &gBallSize, 1.0f);
    }

    DrawText(TextFormat("Cat 1:  %d/20", gScr1), 75, 10, 20, PURPLE);
    DrawText(TextFormat("Cat 2:  %d/20", gScr2), 600, 10, 20, PURPLE);
    DrawText(TextFormat("Balls(1-7): %d   Mode(T): %s   Restart(R))",
                        gCurrBalls, gOnePlayer ? "1 Plr" : "2 Plrs"),
                        10, 425, 18, PURPLE);

    if (gGameOver)
    {
        const char *tx = (gScr1 >= WIN_SCR) ? "CAT 1 WINS!                Press R to restart" : "CAT 2 WINS! Press R to restart";
        DrawText(tx, 180, SCREEN_HEIGHT / 2 - 10, 24, PURPLE);
    }

    EndDrawing();
}

void shutdown()
{
    CloseWindow();
    UnloadTexture(gBgTexture);
    UnloadTexture(gCat1Texture);
    UnloadTexture(gCat2Texture);
    UnloadTexture(gBallTexture);
}

int main()
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}