
//1024x768
#include "console.h"
#include "page.h"
#include "process.h"
#include "keyboard.h"
#include "mouse.h"
#include "interrupt.h"
#include "clock.h"
#include "ata.h"
#include "device.h"
#include "cdromfs.h"
#include "string.h"
#include "graphics.h"
#include "kernel/ascii.h"
#include "kernel/syscall.h"
#include "rtc.h"
#include "kernelcore.h"
#include "kmalloc.h"
#include "memorylayout.h"
#include "kshell.h"
#include "cdromfs.h"
#include "diskfs.h"
#include "serial.h"
#include "ioports.h"

struct graphics *g;
int LEFT_ARROW_KEY = 75;
int RIGHT_ARROW_KEY = 77;
int SPACE_KEY = 57;
int UP_ARROW_KEY = 72;
int shipX = 512-25;
int shipY = 750;
int meteor1X = 512-25;
int meteor1Y = 0;
int meteor2X = 100;
int meteor2Y = -100;
int meteor3X = 267;
int meteor3Y = -30;
int meteor4X = 712;
int meteor4Y = -200;
int meteor5X = 800;
int meteor5Y = -400;
int meteor6X = 940;
int meteor6Y = -600;
int bulletX = 512-25;
int bulletY = 750;
int score = 0;
int health = 3;


void renderMeteor(){
	if(meteor1Y>=0)graphics_rect(g, meteor1X, meteor1Y, 25, 25);
	if(meteor2Y>=0)graphics_rect(g, meteor2X, meteor2Y, 25, 25);
	if(meteor3Y>=0)graphics_rect(g, meteor3X, meteor3Y, 25, 25);
	if(meteor4Y>=0)graphics_rect(g, meteor4X, meteor4Y, 25, 25);
	if(meteor5Y>=0)graphics_rect(g, meteor5X, meteor5Y, 25, 25);
	if(meteor6Y>=0)graphics_rect(g, meteor6X, meteor6Y, 25, 25);
}
void renderShip(){
	graphics_line(g,shipX, shipY, 25, -25);
	graphics_line(g,shipX,shipY, 50, 0);
	graphics_line(g,shipX+50,shipY, -25, -25);
}
void renderBullet(){
	graphics_line(g,bulletX,bulletY,0,-10);
}
void moveBullet(){
	if(bulletY!=750)bulletY-=15;
	if(bulletY <= 0)bulletY=750;
}
void moveMeteor(){
	meteor1Y += 10;
	meteor2Y += 10;
	meteor3Y += 10;
	meteor4Y += 10;
	meteor5Y += 10;
	meteor6Y += 10;
	if(meteor1Y>=760)meteor1Y = 0;
	if(meteor2Y>=760)meteor2Y = 0;
	if(meteor3Y>=760)meteor3Y = 0;
	if(meteor4Y>=760)meteor4Y = 0;
	if(meteor5Y>=760)meteor5Y = 0;
	if(meteor6Y>=760)meteor6Y = 0;
}
void clearScreen(){
	graphics_clear(g,0,0,1024,768);
}
void keyboardHandler(){
	if(inb(0x60) == LEFT_ARROW_KEY && shipX >= 20){
		shipX -= 15;
		if(bulletY==750)bulletX = shipX;
	}
	if(inb(0x60) == RIGHT_ARROW_KEY && shipX <= 960){
		shipX += 15;
		if(bulletY==750)bulletX = shipX;
	}
	if(inb(0x60) == UP_ARROW_KEY){
		bulletY -= 15;
	}
}
void printString(char *str, int x, int y){
	for(int i = 0; i < strlen(str); i++){
		graphics_char(g, x + (8 * i), y, str[i]);
	}
}
void printInt(int n, int x, int y){
	if(n<0)return;
	char str[20];
	*uint_to_string(n, str);
	printString(str, x, y);
}
void checkCollision(){
	if(meteor1Y>=725){
        if(meteor1X<=shipX+25 && meteor1X>=shipX){
            meteor1Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor2Y>=725){
        if(meteor2X<=shipX+25 && meteor2X>=shipX){
            meteor2Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor3Y>=725){
        if(meteor3X<=shipX+25 && meteor3X>=shipX){
            meteor3Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor4Y>=725){
        if(meteor4X<=shipX+25 && meteor4X>=shipX){
            meteor4Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor1Y>=725){
        if(meteor5X<=shipX+25 && meteor5X>=shipX){
            meteor5Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor6Y>=725){
        if(meteor6X<=shipX+25 && meteor6X>=shipX){
            meteor6Y=0;
            health--;
            renderMeteor();
        }
    }
    if(bulletY >= meteor1Y+25 && bulletY <= meteor1Y){
        if(bulletX>=meteor1X && bulletX <= meteor1X +25){
            meteor1Y = 0;
            bulletY = 750;
            score += 77;
        }
    }
    if(bulletY >= meteor2Y+25 && bulletY <= meteor1Y){
        if(bulletX>=meteor2X && bulletX <= meteor2X +25){
            meteor2Y = 0;
            bulletY = 750;
            score += 77;
        }
    }
    if(bulletY >= meteor3Y+25 && bulletY <= meteor1Y){
        if(bulletX>=meteor3X && bulletX <= meteor3X +25){
            meteor3Y = 0;
            bulletY = 750;
            score += 77;
        }
    }
    if(bulletY >= meteor4Y+25 && bulletY <= meteor1Y){
        if(bulletX>=meteor4X && bulletX <= meteor4X +25){
            meteor4Y = 0;
            bulletY = 750;
            score += 77;
        }
    }
    if(bulletY >= meteor5Y+25 && bulletY <= meteor1Y){
        if(bulletX>=meteor5X && bulletX <= meteor5X +25){
            meteor5Y = 0;
            bulletY = 750;
            score += 77;
        }
    }
    if(bulletY >= meteor6Y+25 && bulletY <= meteor1Y){
        if(bulletX>=meteor6X && bulletX <= meteor6X +25){
            meteor6Y = 0;
            bulletY = 750;
            score += 77;
        }
    }
}
void startScreen(){
	while(1){
		if(inb(0x60)==SPACE_KEY)break;
		printString("Baslamak icin 'SPACE' Basiniz", 400, 768/2);
	}
}
void endScreen(){
	while(1){
		if(inb(0x60)==SPACE_KEY)break;
		printString("Tekrar icin 'SPACE' Basiniz", 400, 768/2);
		printString("skorunuz: ", 400, (768/2)+30);
		printInt(score,500, (768/2)+30);
	}
}
void renderStats(){
	printString("can:", 30, 0);
	printInt(health, 70, 0);
	printString("score:", 900, 0);
	printInt(score, 1000, 0);
}
void gameLoop(){
	while(1){
		startScreen();
		while(1){
			keyboardHandler();
			renderShip();
			renderMeteor();
			moveMeteor();
			renderBullet();
			moveBullet();
			checkCollision();
			renderStats();
			clearScreen();
			renderStats();
			if(health==0)break;
		}
		endScreen();
		health=3;
		score=0;
	}
}

int kernel_main()
{
	g = graphics_create_root();
	struct graphics *g = graphics_create_root();
	keyboard_init();
	gameLoop();
	return 0;
}
