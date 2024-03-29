#include "graphics.h"

#define LEFT_ARROW_KEYCODE 75
#define RIGHT_ARROW_KEYCODE 77
#define UP_ARROW_KEYCODE 72
#define SPACE_KEYCODE 32
int screenWidth = 1280; // Örnek olarak 1280x720 çözünürlük kullanýyoruz
int screenHeight = 720;
int shipWidth = 50; // Uzay gemisi geniþliði
int shipHeight = 50; // Uzay gemisi yüksekliði
int shipBottomY = 700; // Uzay gemisinin alt kenarý ekranýn altýna 20 birim mesafede
int shipX = screenWidth / 2; // Uzay gemisinin baþlangýç x konumu
int meteorRadius = 20;
int meteor1X = 100;
int meteor1Y = -200;
int meteor2X = 230;
int meteor2Y = -20;
int meteor3X = 380;
int meteor3Y = -420;
int meteor4X = 600;
int meteor4Y = 0;
int meteor5X = 820;
int meteor5Y = -180;
int meteor6X = 1110;
int meteor6Y = -600;
int bulletX = screenWidth / 2;
int bulletY = 720;
int isShooted = 0;
int health = 3;
int score = 0;
void startScreen(){
    while(1){
       settextjustify(1,1);
       outtextxy(640,300,"Space Odyysey");
       settextjustify(1,1);
       outtextxy(640,360,"Baþlamak için 'Space' Basýnýz");
       if(getch()==SPACE_KEYCODE)break;
    }
}
void renderShip(){

    // Uzay gemisinin çizimi
    line(shipX - shipWidth / 2, shipBottomY, shipX + shipWidth / 2, shipBottomY); // Gövde
    line(shipX - shipWidth / 2, shipBottomY, shipX, shipBottomY - shipHeight); // Sol üçgen
    line(shipX, shipBottomY - shipHeight, shipX + shipWidth / 2, shipBottomY); // Sað üçgen
}
void renderBullet(){
    line(bulletX,bulletY-5,bulletX,bulletY+5);
}
void renderMeteor(){
    circle(meteor1X,meteor1Y,25);
    circle(meteor2X,meteor2Y,25);
    circle(meteor3X,meteor3Y,25);
    circle(meteor4X,meteor4Y,25);
    circle(meteor5X,meteor5Y,25);
    circle(meteor6X,meteor6Y,25);
}
void fps(int sec){
    delay(sec);
    meteor1Y+=5;
    meteor2Y+=5;
    meteor3Y+=5;
    meteor4Y+=5;
    meteor5Y+=5;
    meteor6Y+=5;
    if(bulletY<720) bulletY -=5;
    if(meteor1Y>=720)meteor1Y=0;
    if(meteor2Y>=720)meteor2Y=0;
    if(meteor3Y>=720)meteor3Y=0;
    if(meteor4Y>=720)meteor4Y=0;
    if(meteor5Y>=720)meteor5Y=0;
    if(meteor6Y>=720)meteor6Y=0;
    if(bulletY<=0){
        bulletY=720;
        isShooted = 0;
    }
}
void checkCollision(){
    if(meteor1Y>=650){
        if(meteor1X<=shipX+25 && meteor1X>=shipX-25){
            meteor1Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor2Y>=650){
        if(meteor2X<=shipX+25 && meteor2X>=shipX-25){
            meteor2Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor3Y>=650){
        if(meteor3X<=shipX+25 && meteor3X>=shipX-25){
            meteor3Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor4Y>=650){
        if(meteor4X<=shipX+25 && meteor4X>=shipX-25){
            meteor4Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor1Y>=650){
        if(meteor5X<=shipX+25 && meteor5X>=shipX-25){
            meteor5Y=0;
            health--;
            renderMeteor();
        }
    }
    if(meteor6Y>=650){
        if(meteor6X<=shipX+25 && meteor6X>=shipX-25){
            meteor6Y=0;
            health--;
            renderMeteor();
        }
    }
    if(bulletY <= meteor1Y+20 && bulletY>=meteor1Y-20){
        if(meteor1X-20<=bulletX && meteor1X+20>=bulletX){
            meteor1Y = 0;
            bulletY = 720;
            isShooted = 0;
            score += 77;
        }
    }
    if(bulletY <= meteor2Y+20 && bulletY>=meteor2Y-20){
        if(meteor2X-20<=bulletX && meteor2X+20>=bulletX){
            meteor2Y = 0;
            bulletY = 720;
            isShooted = 0;
            score += 77;
        }
    }
    if(bulletY <= meteor3Y+20 && bulletY>=meteor3Y-20){
        if(meteor3X-20<=bulletX && meteor3X+20>=bulletX){
            meteor3Y = 0;
            bulletY = 720;
            isShooted = 0;
            score += 77;
        }
    }
    if(bulletY <= meteor4Y+20 && bulletY>=meteor4Y-20){
        if(meteor4X-20<=bulletX && meteor4X+20>=bulletX){
            meteor4Y = 0;
            bulletY = 720;
            isShooted = 0;
            score += 77;
        }
    }
    if(bulletY <= meteor5Y+20 && bulletY>=meteor5Y-20){
        if(meteor5X-20<=bulletX && meteor5X+20>=bulletX){
            meteor5Y = 0;
            bulletY = 720;
            isShooted = 0;
            score += 77;
        }
    }
    if(bulletY <= meteor6Y+20 && bulletY>=meteor6Y-20){
        if(meteor6X-20<=bulletX && meteor6X+20>=bulletX){
            meteor6Y = 0;
            bulletY = 720;
            isShooted = 0;
            score += 77;
        }
    }

}
void checkMove(){
    if(kbhit()){
        char keycode = getch();
        if(keycode==LEFT_ARROW_KEYCODE) {
            if(shipX >= 20)shipX -= 20;

        }
        if(keycode==RIGHT_ARROW_KEYCODE) {
            if(shipX <= 1250)shipX += 20;

        }
        if(keycode==UP_ARROW_KEYCODE) {
            if(bulletY == 720){
                bulletY-=5;
                isShooted=1;
            }
        }
    }
}
void renderGUI(){
    char healthSTR[1];
    char scoreSTR[10];
    sprintf(healthSTR,"%d",health);
    sprintf(scoreSTR,"%d",score);
    outtextxy(30,30,"CAN:");
    outtextxy(65,30,healthSTR);
    outtextxy(1200,30,"SKOR:");
    outtextxy(1250,30,scoreSTR);
}
void gameLoop(){
    while(1){
        clearviewport();
        renderShip();
        checkMove();
        renderMeteor();
        renderBullet();
        checkCollision();
        renderGUI();
        if(isShooted==0)bulletX = shipX;
        fps(5);
        if(health==0)break;
    }
}
void endScreen(){
    while(1){
       settextjustify(1,1);
       outtextxy(640,300,"Öldünüz");
       settextjustify(1,1);
       char scoreSTR[10];
       sprintf(scoreSTR,"%d",score);
       settextjustify(1,1);
       outtextxy(640,330,scoreSTR);
       settextjustify(1,1);
       outtextxy(640,360,"Yeniden denemek için 'Space' Basýnýz");
       if(getch()==SPACE_KEYCODE)break;
    }
}
int main()
{
    initwindow(1280, 720, "Space Odyysey");
    outtextxy(0,0,"");
    while(1){
      startScreen();
      clearviewport();
      health=3;
      gameLoop();
      clearviewport();
      endScreen();
      clearviewport();
    }

}
