#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
using namespace std;
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define FOOD_SIZE
#define PACMAN_SIZE 48
#define FOOD_WIDTH 23
#define FOOD_HEIGHT 25

bool init();
int SPEED = 5;

enum Direction
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

SDL_Window *win = NULL;
SDL_Surface *Pacman = NULL;
SDL_Surface *food = NULL;
SDL_Surface *trueSurface = NULL;
SDL_Surface *foodSurface = NULL;
TTF_Font *nerdFont = NULL;
Direction pacmanDirection = NONE;
bool gameover = false;

int loadHighScore()
{
    int highScore = 0;
    ifstream highScoreFile("highscore.txt");

    if (highScoreFile.is_open())
    {
        highScoreFile >> highScore;
        highScoreFile.close();
    }

    return highScore;
}

void saveHighScore(int highScore)
{
    ofstream highScoreFile("highscore.txt");

    if (highScoreFile.is_open())
    {
        highScoreFile << highScore;
        highScoreFile.close();
    }
}

bool init()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Could not Initialize SDL " << SDL_GetError() << endl;
        success = false;
    }
    else
    {
        win = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT + 200, SDL_WINDOW_SHOWN);
        if (win == NULL)
        {
            cout << "Window could not be made : " << SDL_GetError() << endl;
            success = false;
        }
        else
        {
            trueSurface = SDL_GetWindowSurface(win);
        }
    }
    if (TTF_Init() == -1)
    {
        cout << "SDL_ttf could not initialize: " << TTF_GetError() << endl;
        success = false;
    }
    else
    {
        nerdFont = TTF_OpenFont("JetBrainsMonoNerdFont-Bold.ttf", 24);
        if (nerdFont == NULL)
        {
            cout << "Failed to load font: " << TTF_GetError() << endl;
            success = false;
        }
    }

    return success;
}

void renderText(const string &text, SDL_Color color, int x, int y)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(nerdFont, text.c_str(), color);
    if (textSurface == NULL)
    {
        cout << "Unable to render text surface: " << TTF_GetError() << endl;
        return;
    }

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_BlitSurface(textSurface, NULL, trueSurface, &textRect);
    SDL_FreeSurface(textSurface);
}

void LoadPacman(Direction a)
{
    switch (a)
    {
    case UP:
        Pacman = IMG_Load("test/Pacman/pacman_up.png");
        break;
    case DOWN:
        Pacman = IMG_Load("test/Pacman/pacman_down.png");
        break;
    case LEFT:
        Pacman = IMG_Load("test/Pacman/pacman_side.png");
        break;
    case RIGHT:
        Pacman = IMG_Load("test/Pacman/pacman_front.png");
        break;
    default:
        Pacman = IMG_Load("test/Pacman/pacman_front.png");
        break;
    }
}

void SetDirection(SDL_Rect &ballRect, Direction a)
{
    switch (a)
    {
    case UP:
        ballRect.y -= SPEED;
        if (ballRect.y < 100)
        {
            ballRect.y = 99;
        }
        break;
    case DOWN:
        ballRect.y += SPEED;
        if (ballRect.y > 500 - PACMAN_SIZE)
        {
            ballRect.y = 500 - PACMAN_SIZE;
        }
        break;
    case LEFT:

        ballRect.x -= SPEED;
        if (ballRect.x < 0)
        {
            ballRect.x = 0;
        }
        break;
    case RIGHT:
        ballRect.x += SPEED;
        if (ballRect.x > WINDOW_WIDTH - PACMAN_SIZE)
        {
            ballRect.x = WINDOW_WIDTH - PACMAN_SIZE;
        }
        break;
    case NONE:
        break;
    }
}

bool isCollided(SDL_Rect a, SDL_Rect b)
{
    if (a.x + a.w <= b.x || b.x + b.w <= a.x)
        return false;

    if (a.y + a.h <= b.y || b.y + b.h <= a.y)
        return false;

    return true;
}

void spawnFood(SDL_Rect &food)
{
    food.x = rand() % (WINDOW_WIDTH - FOOD_WIDTH);
    food.y = 200 + rand() % (400 - 200 - FOOD_HEIGHT);
}

void pauseGame(SDL_Event &e, SDL_Surface *background, SDL_Rect pacman, SDL_Rect food)
{
    bool paused = true;
    SDL_Surface *pause = IMG_Load("test/Pacman/pause.png");
    SDL_Rect pauseRect = {(WINDOW_WIDTH - 300) / 2, (WINDOW_HEIGHT) / 2, 300, 200};

    while (paused)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                paused = false;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                paused = false;
            }
        }

        SDL_FillRect(trueSurface, NULL, SDL_MapRGB(trueSurface->format, 0, 0, 0));
        SDL_BlitSurface(background, NULL, trueSurface, NULL);
        SDL_BlitScaled(Pacman, NULL, trueSurface, &pacman);
        SDL_BlitScaled(foodSurface, NULL, trueSurface, &food);

        SDL_BlitScaled(pause, NULL, trueSurface, &pauseRect);

        SDL_UpdateWindowSurface(win);
        SDL_Delay(16);
    }
}
void showGameOver(SDL_Surface *background, SDL_Rect pacman, SDL_Rect food)
{
    SDL_Surface *gameOverImage = IMG_Load("test/Pacman/retry.png"); // Load the Game Over image
    SDL_Rect gameOverRect = {(WINDOW_WIDTH - 300) / 2, (WINDOW_HEIGHT) / 2, 300, 200};

    bool gameOver = true;
    SDL_Event e;

    while (gameOver)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                gameOver = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_Quit();
                    gameOver = false;
                }
                else if (e.key.keysym.sym == SDLK_r)
                {
                    gameOver = false;
                }
            }
        }

        SDL_FillRect(trueSurface, NULL, SDL_MapRGB(trueSurface->format, 0, 0, 0));
        SDL_BlitSurface(background, NULL, trueSurface, NULL);
        SDL_BlitScaled(Pacman, NULL, trueSurface, &pacman);
        SDL_BlitScaled(foodSurface, NULL, trueSurface, &food);
        SDL_BlitScaled(gameOverImage, NULL, trueSurface, &gameOverRect);

        SDL_UpdateWindowSurface(win);
        SDL_Delay(16);
    }
}

void resetGame(SDL_Rect &pacman, SDL_Rect &food)
{
    pacman.x = (WINDOW_WIDTH - PACMAN_SIZE) / 2;
    pacman.y = (WINDOW_HEIGHT - PACMAN_SIZE + 200) / 2;
    Pacman = IMG_Load("test/Pacman/pacman_front.png");
    pacmanDirection = NONE;
    SPEED = 5;
    spawnFood(food);

}

int score = 0;
int highScore = loadHighScore();

int main()
{
    if (!init())
    {
        cout << "Failed to initialize window\n";
    }
    else
    {
        SDL_Color textColor = {0, 0, 0};
        SDL_Surface *background = IMG_Load("test/Pacman/background.png");
        foodSurface = IMG_Load("test/Pacman/food.png");
        SDL_Rect pacman = {300, 300, 48, 48};
        SDL_Rect food = {rand() % WINDOW_WIDTH - FOOD_WIDTH, 200 + rand() % (400 - 200 - FOOD_HEIGHT), FOOD_WIDTH, FOOD_HEIGHT};
        LoadPacman(pacmanDirection);

        bool quit = false, escape = false, gameOver = false;

        SDL_Event e;
        while (!quit)
        {
            SDL_FillRect(trueSurface, NULL, SDL_MapRGB(trueSurface->format, 0, 0, 0));
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_w:
                        pacmanDirection = UP;
                        LoadPacman(pacmanDirection);
                        break;
                    case SDLK_s:
                        pacmanDirection = DOWN;
                        LoadPacman(pacmanDirection);
                        break;
                    case SDLK_a:
                        pacmanDirection = LEFT;
                        LoadPacman(pacmanDirection);
                        break;
                    case SDLK_d:
                        pacmanDirection = RIGHT;
                        LoadPacman(pacmanDirection);
                        break;
                    case SDLK_ESCAPE:
                        escape = true;
                    default:
                        break;
                    }
                }
            }

            if (pacman.x <= 0 || pacman.x >= WINDOW_WIDTH - PACMAN_SIZE ||
                pacman.y <= 100 || pacman.y >= 500 - PACMAN_SIZE)
            {
                gameOver = true;
            }
            if (gameOver)
            {
                showGameOver(background, pacman, food);
                resetGame(pacman, food);
                gameOver = false;
                score = 0;
                highScore = loadHighScore();
            }
            if (isCollided(pacman, food))
            {
                spawnFood(food);
                score++;
                if (score > 1 && score % 10 == 0)
                {
                    SPEED += 1;
                }
                if (score > highScore)
                {
                    saveHighScore(score);
                }
            }

            string scoreString = "SCORE : " + to_string(score);
            string highScoreString = "HIGH SCORE : " + to_string(highScore);

            int highScoreTextWidth, highScoreTextHeight;
            TTF_SizeText(nerdFont, highScoreString.c_str(), &highScoreTextWidth, &highScoreTextHeight);
            int highScoreX = (WINDOW_WIDTH - highScoreTextWidth) / 2;
            int highScoreY = (500 + 600) / 2 - highScoreTextHeight / 2;

            int ScoreTextWidth, ScoreTextHeight;
            TTF_SizeText(nerdFont, scoreString.c_str(), &ScoreTextWidth, &ScoreTextHeight);
            int ScoreX = (WINDOW_WIDTH - ScoreTextWidth) / 2;
            int ScoreY = (100) / 2 - ScoreTextHeight / 2;

            SetDirection(pacman, pacmanDirection);
            SDL_BlitSurface(background, NULL, trueSurface, NULL);
            SDL_BlitScaled(foodSurface, NULL, trueSurface, &food);
            SDL_BlitScaled(Pacman, NULL, trueSurface, &pacman);
            renderText(highScoreString, textColor, highScoreX, highScoreY);
            renderText(scoreString, textColor, ScoreX, ScoreY);
            SDL_UpdateWindowSurface(win);
            SDL_Delay(16);
            if (escape)
            {
                pauseGame(e, background, pacman, food);
                escape = false;
            }
        }

        if (background)
        {
            SDL_FreeSurface(background);
        }
        if (foodSurface)
        {
            SDL_FreeSurface(foodSurface);
        }
        if (Pacman)
        {
            SDL_FreeSurface(Pacman);
        }
        if (win)
        {
            SDL_DestroyWindow(win);
        }

        SDL_Quit();
    }
    return 0;
}
