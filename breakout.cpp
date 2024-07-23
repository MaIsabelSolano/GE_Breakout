#include <iostream>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

const int MAX_FPS = 60;


const int BALL_SPEED = 200;
const int BALL_SIZE = 25;

const int BAR_HEIGTH = 15;
const int BAR_WIDTH = 100;
// const int BAR_WIDTH = 640;
const int BAR_SPEED = 500;

const int BLOCK_WIDTH = 60;
const int BLOCK_HEIGHT = 30;
const int BLOCK_ROWS = 6;
const int BLOCK_COLUMNS = 9;


struct RectSq {
    SDL_Rect rect = { 0, 0, 100, 100 };
    int vx = 0; // velocidad en x
    int vy = 0; // velocidad en y
    SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
};

struct RectBl {
    SDL_Rect rect = { 0, 0, 100, 100 };
    SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
};

SDL_Color red = { 0xFF, 0x00, 0x00, 0xFF };
SDL_Color blue = { 0x00, 0x00, 0xFF, 0xFF };
SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };
SDL_Color black = { 0x00, 0x00, 0x00, 0xFF };

// iniciamos una pelota con los siguientes valores
RectSq ball = { {(SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 3), BALL_SIZE, BALL_SIZE}, BALL_SPEED, BALL_SPEED, red };
RectSq bar = { {(SCREEN_WIDTH / 2), (SCREEN_HEIGHT - (5 * BAR_HEIGTH)), BAR_WIDTH, BAR_HEIGTH}, BAR_SPEED, BAR_SPEED, white };
// RectSq bar = { {0, (SCREEN_HEIGHT - (5 * BAR_HEIGTH)), BAR_WIDTH, BAR_HEIGTH}, BAR_SPEED, BAR_SPEED, white};

// RectSq ball2 = { {SCREEN_WIDTH - BALL_SIZE, 0, 100, 100}, 100, 100, blue };

bool gameOver = false;

std::vector<RectSq> blocks;

void renderRect(SDL_Renderer* renderer, RectSq& b) {
    SDL_SetRenderDrawColor(renderer, b.color.r, b.color.g, b.color.b, b.color.a);
    SDL_RenderFillRect(renderer, &b.rect);
}

bool checkColission(const SDL_Rect& a, const SDL_Rect& b) {
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
        );
}

void handleInput(SDL_Event& e) {
    // solo editamos la barra
    const Uint8* ks = SDL_GetKeyboardState(NULL);

    bar.vx = 0;

    if (ks[SDL_SCANCODE_A]) {
        bar.vx = -BAR_SPEED;
    }
    if (ks[SDL_SCANCODE_D]) {
        bar.vx = BAR_SPEED;
    }
}

void Update(float dT) {

    // revisar si la bola no se sale del mapa
    if (ball.rect.x < 0 || ball.rect.x + ball.rect.w > SCREEN_WIDTH) {
        ball.vx *= -1;
    }
    if (ball.rect.y < 0) {
        ball.vy *= -1;
    }
    if (ball.rect.y + ball.rect.h > SCREEN_HEIGHT) {
        // game over
        gameOver = true;
    }

    // revisar si la barra no se sale del mapa
    if (bar.rect.x < 0 || bar.rect.x + bar.rect.w > SCREEN_WIDTH) {
        bar.vx *= -1;
    }

    // revisar colisiones
    // contra la barra
    // std::cout << "Hello, World!" << std::endl;
    if (checkColission(bar.rect, ball.rect)) {
        std::cout << "Collision!" << std::endl;
        // ball.vx *= -1;
        ball.vy *= -1;
        ball.vy += -30;
        // std::swap(bar.vy, bar.vy);
    }

    // constra los bloques
    // check collision with blocks
    for (auto it = blocks.begin(); it != blocks.end();) {
        if (checkColission(ball.rect, it->rect)) {
            std::cout << "Collision with block!" << std::endl;
            ball.vy *= -1;
            // ball.vx *= -1;
            it = blocks.erase(it); // remove the block on collision
        }
        else {
            ++it;
        }
    }

    // aplicar movimiento a la bola
    ball.rect.x += ball.vx * dT;
    ball.rect.y += ball.vy * dT;

    // aplicar el movimiento a la barra
    bar.rect.x += bar.vx * dT;

}

void createBlocks() {
    for (int i = 0; i < BLOCK_ROWS; ++i) {
        for (int j = 0; j < BLOCK_COLUMNS; ++j) {
            RectSq block = { {(j * (BLOCK_WIDTH + 5)), (i * (BLOCK_HEIGHT + 5)), BLOCK_WIDTH, BLOCK_HEIGHT}, 0, 0, blue };
            blocks.push_back(block);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    bool quit = false;
    SDL_Event e;

    Uint32 frameCount = 1;
    Uint32 frameStartTimestamp;
    Uint32 frameEndTimestamp;
    Uint32 lastFrameTime = SDL_GetTicks();
    Uint32 lastUpdateTime = 0;
    float frameDuration = (1.0 / MAX_FPS) * 1000.0;
    float actualFrameDuration;
    int FPS = MAX_FPS;

    createBlocks();

    // game loop
    while (!quit) {
        frameStartTimestamp = SDL_GetTicks();

        // deltatime
        Uint32 currentFrameTime = SDL_GetTicks();
        float dT = (currentFrameTime - lastFrameTime) / 1000.0;
        lastFrameTime = currentFrameTime;


        // process events
        // poll event - escuchamos al teclado
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handleInput(e);
        }

        // Update
        Update(dT);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        renderRect(renderer, ball);
        renderRect(renderer, bar);

        for (auto& block : blocks) {
            renderRect(renderer, block);
        }

        SDL_RenderPresent(renderer);

        // corrección del frame
        frameEndTimestamp = SDL_GetTicks();
        actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

        if (actualFrameDuration < frameDuration) {
            SDL_Delay(frameDuration - actualFrameDuration);
        }

        // fps calculation
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - lastUpdateTime;
        if (elapsedTime > 1000) {
            FPS = (float)frameCount / (elapsedTime / 1000.0);
            lastUpdateTime = currentTime;
            frameCount = 0;
        }
        SDL_SetWindowTitle(window, ("FPS: " + std::to_string(FPS)).c_str());

        if (gameOver) {
            std::cout << "Perdiste whomp whomp" << std::endl;
            quit = true;
        }

        if (blocks.size() == 0) {
            std::cout << "Ganaste, YAY!" << std::endl;
            quit = true;
        }

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
