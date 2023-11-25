#include <iostream>
#include <Windows.h> 
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define BLOCK_SIZE 20

using namespace std;

/*
to do:
the eating apple part
*/

class Mouse {
public:
    int x,y;
    bool leftButtonPressed = false;
    bool rightButtonPressed = false;
    void updateMouseXY(int x1, int y1) {
        this->x = x1;
        this->y = y1;
    }
};
class Keyboard {
public:
    bool wBtnPressed = false;
    bool sBtnPressed = false;
    bool aBtnPressed = false;
    bool dBtnPressed = false;
    bool spaceBtnPressed = false;
    void printButtonsState() {
        cout << "W: " << this->wBtnPressed << " S: " << this->sBtnPressed << " A: " << this->aBtnPressed << " D: " << this->dBtnPressed << endl;
    }
};

class BoardBlock {
public:
    SDL_Renderer* r;
    int x, y;
    SDL_Rect rect;
    BoardBlock(SDL_Renderer* r, int x,int y) {
        this->r = r;
        this->x = x;
        this->y = y;
        this->rect = { x,y,BLOCK_SIZE,BLOCK_SIZE };
    }
    void draw() {
        SDL_RenderDrawRect(this->r, &this->rect);
    }
};
class Board {
public:
    SDL_Renderer* r;
    int x = 0;
    int y = 0;
    vector<vector<BoardBlock>> board;
    Board(SDL_Renderer* r) {
        this->r = r;
        for (int x1 = this->x; x1 < WIN_WIDTH; x1 += BLOCK_SIZE) {
            vector<BoardBlock> vb = {};
            for (int y1 = this->y; y1 < WIN_HEIGHT; y1 += BLOCK_SIZE) {
                vb.push_back(BoardBlock(r, x1, y1));
            }
            this->board.push_back(vb);
        }
    }
    void draw() {
        // all of the board block are black so we do the color change 1 time instead of for each one
        SDL_SetRenderDrawColor(this->r, 100, 100, 100, 100);
        for (vector<BoardBlock> vb : this->board) {
            for (BoardBlock b : vb) {
                b.draw();
            }
        }
    }
};
int getRandomNum(int min, int max) {
    return (rand() % (max - min + 1)) + min - 1;
}
bool checkVectorInsideAnother(vector<vector<int>> bigV, vector<int> v) {
    for (vector<int> v2 : bigV) {
        if (v2 == v) {
            return true;
        }
    }
    return false;
}
class Snake {
public:
    SDL_Renderer* r;
    int x = BLOCK_SIZE * 15;
    int y = BLOCK_SIZE * 15;
    SDL_Rect rect = { x,y };
    vector<vector<int>> snakeBlocks = { {x,y} };
    char direction = 'r'; // l-left,r-right,u-up-d-down

    int appleX = abs(getRandomNum(0, WIN_WIDTH / BLOCK_SIZE) * BLOCK_SIZE);
    int appleY = abs(getRandomNum(0, WIN_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE);

    Snake(SDL_Renderer* r) {
        this->r = r;
    }
    void handleEatingApple() {
        this->appleX = abs(getRandomNum(0, WIN_WIDTH / BLOCK_SIZE) * BLOCK_SIZE);
        this->appleY = abs(getRandomNum(0, WIN_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE);
        vector<int> applePos = { this->appleX,this->appleY };
        while (checkVectorInsideAnother(this->snakeBlocks, applePos) == true) {
            this->appleX = abs(getRandomNum(0, WIN_WIDTH / BLOCK_SIZE) * BLOCK_SIZE);
            this->appleY = abs(getRandomNum(0, WIN_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE);
            vector<int> applePos = { this->appleX,this->appleY };
        }
        cout << appleX << " " << appleY << endl;
        vector<vector<int>> optionalPositions = {
            {this->snakeBlocks[this->snakeBlocks.size()-1][0] - BLOCK_SIZE,this->snakeBlocks[this->snakeBlocks.size() - 1][1]},
            {this->snakeBlocks[this->snakeBlocks.size() - 1][0] + BLOCK_SIZE,this->snakeBlocks[this->snakeBlocks.size() - 1][1]},
            {this->snakeBlocks[this->snakeBlocks.size() - 1][0],this->snakeBlocks[this->snakeBlocks.size() - 1][1] - BLOCK_SIZE},
            {this->snakeBlocks[this->snakeBlocks.size() - 1][0],this->snakeBlocks[this->snakeBlocks.size() - 1][1] + BLOCK_SIZE}
        };
        for (vector<int> v : optionalPositions) {
            if (checkVectorInsideAnother(this->snakeBlocks, v) == false) {
                this->snakeBlocks.push_back(v);
                break;
            }
        }
    }
    void draw() {
        SDL_SetRenderDrawColor(this->r, 255, 50, 50, 255);
        SDL_Rect appleRect = { this->appleX,this->appleY,BLOCK_SIZE,BLOCK_SIZE };
        SDL_RenderFillRect(this->r, &appleRect);

        SDL_SetRenderDrawColor(this->r, 50,50,255, 255);
        SDL_Rect r;
        for (vector<int> v : this->snakeBlocks) {
            r = { v[0],v[1],BLOCK_SIZE,BLOCK_SIZE };
            SDL_RenderFillRect(this->r, &r);

        }
        //SDL_RenderFillRect(this->r, &this->rect);
    }
    void move(Keyboard k) {
        static int iterNum = 0; // variable that will go up by 1 each time this function is called
        iterNum += 1;
        // changing direction
        if (k.aBtnPressed == true && this->direction != 'r' || k.aBtnPressed == true && this->snakeBlocks.size() == 1) {
            this->direction = 'l';
        } 
        else if (k.dBtnPressed == true && this->direction != 'l' || k.dBtnPressed == true && this->snakeBlocks.size() == 1) {
            this->direction = 'r';
        } 
        else if (k.wBtnPressed == true && this->direction != 'd' || k.wBtnPressed == true && this->snakeBlocks.size() == 1) {
            this->direction = 'u';
        } 
        else if (k.sBtnPressed == true && this->direction != 'u' || k.sBtnPressed == true && this->snakeBlocks.size() == 1) {
            this->direction = 'd';
        }

        // moving itself
        if (iterNum % 7 == 0) { // making it only after the 7th calling for this func for it to be with less speed(not fly off the screen)
            if (this->direction == 'l') {
                if (this->snakeBlocks.size() == 1) {
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0] - BLOCK_SIZE,this->snakeBlocks[0][1] };
                }
                else {
                    for (int i = this->snakeBlocks.size() - 1; i > 0; i--) {
                        this->snakeBlocks.at(i) = this->snakeBlocks.at(i - 1);
                    }
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0] - BLOCK_SIZE,this->snakeBlocks[0][1] };
                }
            }
            else if (this->direction == 'r') {
                if (this->snakeBlocks.size() == 1) {
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0] + BLOCK_SIZE,this->snakeBlocks[0][1] };
                }
                else {
                    for (int i = this->snakeBlocks.size() - 1; i > 0; i--) {
                        this->snakeBlocks.at(i) = this->snakeBlocks.at(i - 1);
                    }
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0] + BLOCK_SIZE,this->snakeBlocks[0][1] };
                }
            }
            else if (this->direction == 'u') {
                if (this->snakeBlocks.size() == 1) {
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0],this->snakeBlocks[0][1] - BLOCK_SIZE };
                }
                else {
                    for (int i = this->snakeBlocks.size() - 1; i > 0; i--) {
                        this->snakeBlocks.at(i) = this->snakeBlocks.at(i - 1);
                    }
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0],this->snakeBlocks[0][1] - BLOCK_SIZE };
                }
            }
            else if (this->direction == 'd') {
                if (this->snakeBlocks.size() == 1) {
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0],this->snakeBlocks[0][1] + BLOCK_SIZE };
                }
                else {
                    for (int i = this->snakeBlocks.size() - 1; i > 0; i--) {
                        this->snakeBlocks.at(i) = this->snakeBlocks.at(i - 1);
                    }
                    this->snakeBlocks[0] = { this->snakeBlocks[0][0],this->snakeBlocks[0][1] + BLOCK_SIZE };
                }
            }
            iterNum = 0;
            vector<int> applePos = { this->appleX, this->appleY };
            if (this->snakeBlocks.at(0) == applePos) {
                this->handleEatingApple();
            }
            // snake out of screen
            if (this->snakeBlocks.at(0).at(0) < 0 || this->snakeBlocks.at(0).at(0) >= WIN_WIDTH || this->snakeBlocks.at(0).at(1) < 0 || this->snakeBlocks.at(0).at(1) >= WIN_HEIGHT) {
                this->snakeBlocks = { { x,y } };
                this->appleX = abs(getRandomNum(0, WIN_WIDTH / BLOCK_SIZE) * BLOCK_SIZE);
                this->appleY = abs(getRandomNum(0, WIN_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE);
            }
            // snake is on itself
            vector<vector<int>> snakeWithoutHead = this->snakeBlocks;
            snakeWithoutHead.erase(snakeWithoutHead.begin());
            if (checkVectorInsideAnother(snakeWithoutHead, this->snakeBlocks.at(0)) == true) {
                this->snakeBlocks = { { x,y } };
                this->appleX = abs(getRandomNum(0, WIN_WIDTH / BLOCK_SIZE) * BLOCK_SIZE);
                this->appleY = abs(getRandomNum(0, WIN_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE);
            }
        }
        this->rect = { this->x,this->y,BLOCK_SIZE,BLOCK_SIZE };
    }
};

int main(int argc, char* argv[]) {
    srand(time(0)); // by doing this it will be real random numbers

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }

    window = SDL_CreateWindow("Snake game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT,
        0);

    if (!window)
    {
        cout << "Failed to create window\n";
        return -1;
    }

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    renderer = SDL_CreateRenderer(window, -1, 0); // creating a render so we will be able to do stuff on the screen


    if (!window_surface)
    {
        cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    TTF_Font* font20 = TTF_OpenFont("fonts\\MADEINFINITY PERSONALUSE-Regular.otf", 20);
    TTF_Font* font30 = TTF_OpenFont("fonts\\MADEINFINITY PERSONALUSE-Regular.otf", 30);


    Mouse cMouse = Mouse();
    Keyboard cKeyboard = Keyboard();
    Board cBoard = Board(renderer);
    Snake cSnake = Snake(renderer);

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    SDL_Event e;
    bool bGameOn = true;
    int fps = 0;
    time_t startTime = time(0);
    while (bGameOn) {
        // fps stuff
        frameStart = SDL_GetTicks();

        fps += 1;
        if (difftime(time(0), startTime) >= 1.0) {
            cout << "fps: " << fps << endl;
            startTime = time(0);
            fps = 0;
        }

        // code
        while (SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
                bGameOn = false;
                break;
            }
            else if (e.type == SDL_MOUSEMOTION) {
                cMouse.updateMouseXY(e.motion.x, e.motion.y);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    cMouse.leftButtonPressed = true;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    cMouse.rightButtonPressed = true;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    cMouse.leftButtonPressed = false;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    cMouse.rightButtonPressed = false;
                }
            }

            if (e.type == SDL_KEYDOWN) {
                // updating the wsad buttons state in the keyboard class
                if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP) {
                    cKeyboard.wBtnPressed = true;
                }
                if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN) {
                    cKeyboard.sBtnPressed = true;
                }
                if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT) {
                    cKeyboard.aBtnPressed = true;
                }
                if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT) {
                    cKeyboard.dBtnPressed = true;
                }
                if (e.key.keysym.sym == SDLK_SPACE) {
                    cKeyboard.spaceBtnPressed = true;
                }
            }
            else if (e.type == SDL_KEYUP) {
                // updating the wsad buttons state in the keyboard class
                if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP) {
                    cKeyboard.wBtnPressed = false;
                }
                if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN) {
                    cKeyboard.sBtnPressed = false;
                }
                if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT) {
                    cKeyboard.aBtnPressed = false;
                }
                if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT) {
                    cKeyboard.dBtnPressed = false;
                }
                if (e.key.keysym.sym == SDLK_SPACE) {
                    cKeyboard.spaceBtnPressed = false;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // parameters; render, red value,green value, blue value, alpha level
        SDL_RenderClear(renderer);// clrear the current rendering target

        SDL_SetRenderDrawColor(renderer, 0,0,0, 255);

        cBoard.draw();
        cSnake.draw();
        cSnake.move(cKeyboard);

        SDL_RenderPresent(renderer);

        // fps stuff
        // Calculate frame time and delay if necessary
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}