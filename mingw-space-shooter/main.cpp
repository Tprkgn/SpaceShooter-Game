#include <iostream>
#include <windows.h>
#include <conio.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>

using namespace std;

#define COLUMNS_IN_LINE 80
#define LINES 25
#define ENTER_KEYCODE 13
#define LEFT_ARROW_KEYCODE 75
#define RIGHT_ARROW_KEYCODE 77
#define UP_ARROW_KEYCODE 72
#define SPACE_KEYCODE 32
const char *blank = "   ";
// meteor
int meteors[5][2];
int meteorY = 0;
int meteorX = 10;
const char *meteor_s = "[ ]";
// mermi
int bulletX;
int bulletY;
int shooted = 0;
const char *bullet_s = "|";
// gemi
int shipX = COLUMNS_IN_LINE/2;
int shipY = 23;
int shipDir = 0;
const char *ship_s = "/\\";


int gameStart = 0;
int health = 3;
int score = 0;
int meteors_init = 0;
int restart = 0;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void moveCursor(short x, short y)
{
    COORD position = {x,y};
    SetConsoleCursorPosition(console, position);
}

void drawStr(const char* str, unsigned int x, unsigned int y)
{
    moveCursor(x,y);
    cout << str;
}

void drawInt(int val, unsigned int x, unsigned int y)
{
    moveCursor(x,y);
    cout << val;
}

void fps(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void startScreen()
{
    system("CLS");
    moveCursor(0,11);
    cout << "                           Welcome to Space Invaders!                           ";
    moveCursor(0,12);
    cout << "                            Press \"Enter\" to Start!                         ";
    char key;
    while(1)
    {
        key = _getch();
        if(key == ENTER_KEYCODE) gameStart = 1;
        if(gameStart==1)
        {
            break;
        }
    }
}

int randomNum(int start, int end)
{
    return (rand() % (end-start))+start;
}

void clearMeteor(int x, int y)
{
    drawStr(blank,x,y);
}

void drawMeteor(int x, int y)
{
    drawStr(meteor_s,x,y);
}

void meteorHandler(void)
{


    if(meteors_init == 0)
    {
        int i;
        for(i = 0; i<5; i++)
        {

            meteors[i][0] = randomNum(15, 60);
            meteors[i][1] = randomNum(-50,0);
        }
        meteors_init = 1;
    }
    int i;
    for(i = 0; i<5; i++)
    {

        //meteor gemi çarpýþma algýlama
        if(meteors[i][1] == shipY && (meteors[i][0] > shipX-2 && meteors[i][0] < shipX+4))
        {
            health -=1;
        }


        clearMeteor(meteors[i][0],meteors[i][1]);
        if(meteors[i][1] < 25)
        {
            meteors[i][1] = meteors[i][1] + 1;
            if(meteors[i][1] < 0) continue;
            drawMeteor(meteors[i][0],meteors[i][1]);
        }
        else
        {
            meteors[i][1] = randomNum(-50,0);
            meteors[i][0] = randomNum(15,60);
        }
    }

}
void drawShip(void)
{
    // gemi oluþturma
    drawStr(ship_s,shipX,shipY);
}

void renderShip(void)
{
    if(shipDir== 0)
    {
        drawShip();
        return;
    }
    if(shipDir == 1)
    {
        drawStr(blank, shipX-2, shipY);
        drawShip(); // saða hareket
        return;
    }
    if(shipDir == -1)
    {
        drawStr(blank, shipX+2, shipY);
        drawShip(); // sola hareket
        return;
    }
}

void bulletHandler(void)
{
    // mermi sýfýrlama
    if(shooted == 0)
    {
        bulletX = 0;
        bulletY = 0;
    }
    // mermi hareketi
    if(shooted == 1)
    {
        if(bulletX == 0 && bulletY == 0)
        {
            bulletX = shipX+1;
            bulletY = shipY + 1;
        }
        const char *blank = " ";
        drawStr(blank,bulletX,bulletY);
        bulletY = bulletY - 2;
        drawStr(bullet_s,bulletX,bulletY);
        if(bulletY <= 0)
        {
            drawStr(blank,bulletX,bulletY);
            bulletY = shipY + 1;
            shooted = 0;
        }
    }
    int i;
    for(i = 0; i<5; i++)
    {
        // mermi meteor çarpýþmasý
        if(meteors[i][0]-1 <= bulletX && meteors[i][0]+2 >= bulletX && (meteors[i][1] <= bulletY && meteors[i][1] >= bulletY-2))
        {
            score += 77;
            clearMeteor(meteors[i][0], meteors[i][1]);
            clearMeteor(meteors[i][0], meteors[i][1]+1);
            clearMeteor(meteors[i][0], meteors[i][1]+2);
            meteors[i][0] = randomNum(15, 60);
            meteors[i][1] = randomNum(-50,0);
            bulletY= shipY + 1;
            shooted = 0;
        }
    }
}

void renderStatus(void)
{


    const char *clear_status_bar = 	"                                                                                ";
    drawStr(clear_status_bar,0,0);


    const char *status_bar = 		"Health:                                                               Score:    ";
    drawStr(status_bar, 0,0);

    ;
    drawInt(health,7,0);

    drawInt(score,76,0);
}

void endScreen()
{
    system("CLS");
    moveCursor(0,11);
    cout << "                                   Game Over                                  ";
    moveCursor(0,12);
    cout << "                            Press \"Enter\" to Restart!                     ";
    moveCursor(0,13);
    cout << "                                 Your Score:";
    cout << score;

    char key;
    while(1)
    {
        key = _getch();
        if(key == ENTER_KEYCODE)
        {
            // baþtan baþlat
            gameStart = 1;
            system("CLS");
            meteors_init = 0;
            health = 3;
            score = 0;
            restart = 1;
            break;
        }
    }
}

void game()
{
    system("CLS");
    while(1)
    {
        shipDir = 0;
        meteorHandler();
        bulletHandler();
        renderShip();
        renderStatus();
        if(health <= 0)
        {
            // oyun bitiþi
            gameStart = 0;
            endScreen();
        }

        // kontroller
        char key;
        if(kbhit())
        {
            key = _getch();

            if(key == RIGHT_ARROW_KEYCODE)
            {
                if(gameStart == 1)
                {
                    if(shipX == COLUMNS_IN_LINE-5)
                    {
                        fps(90);
                        continue;
                    }
                    shipX = shipX + 2;
                    shipDir = 1;
                    renderShip();
                }
            }
            if(key == LEFT_ARROW_KEYCODE)
            {
                if(gameStart == 1)
                {
                    if(shipX == 0)
                    {
                        fps(90);
                        continue;
                    }
                    shipX = shipX - 2;
                    shipDir = -1;
                    renderShip();
                }
            }
            if(key == UP_ARROW_KEYCODE)
            {
                if(shooted == 0)
                {
                    shooted = 1;
                }
            }
        }
        fps(90); // oyun hýzý
    }
}

int main()
{
    time_t t;
    srand((unsigned) time(&t));

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(console, &bufferInfo);

    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;

    SetConsoleCursorInfo(console, &info);
    SMALL_RECT windowSize = {0, 0, 80, 25};
    SetConsoleWindowInfo(console, TRUE, &windowSize);


    //oyun yaþam döngüsü
    startScreen();
    game();

    getchar();
    return 0;
}

