#include "keyboard_map.h" 
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C //0x1C = "\n"
#define LEFT_KEY_CODE 0x4B 
#define RIGHT_KEY_CODE 0x4D 
#define SPACE_KEY_CODE 0x39 
#define UP_KEY_CODE 0x48
#define SIZE 15

int board[SIZE-3][SIZE];
const char *BBG = "Space Odyysey";
char *meteor = "[ ]";
char *bullet = "|";
char *leftBorder = "| ";
char *rightBorder = " |";
int startGame = 0;
int isStart = 0;
int gameOver = 0;
int moveShip = 0;
int score=0;
int lifeLeft = 3;
int instr = 0;
int shipX = 38;
int shipY = 24;
int bulletX = 38;
int bulletY = 25;
int meteor1X = 38;
int meteor1Y = -10;
int meteor2X = 18;
int meteor2Y = -5;
int meteor3X = 48;
int meteor3Y = 0;
int meteor4X = 10;
int meteor4Y = -15;
int meteor5X = 67;
int meteor5Y = -20;
int meteor6X = 52;
int meteor6Y = -8;


void cursorXY(unsigned int x, unsigned int y);
void writeSTR(const char *str,unsigned int x, unsigned int y);
void renderShip(void);
void renderMeteor(void);
void renderBullet(void);
void clearMeteor(void);
void clearBullet(void);
void clearShipRight(void);
void clearShipLeft(void);
void checkCollision(void);
void checkCollision();
void moveMeteor(void);
void moveBullet(void);
void print_integer(int n,int color);
void gameBoard(void);
void gameLoop(void);
void directionUpdate(void);
void bulletUpdate(void);
void updateMessage(void);
void startScreen(void);
extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void kprint(const char *str,int color)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = color;
	}
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}
void print_integer(int n, int color) // sayı printleme
{
    int i = 0;
    char buffer[20];  
    do {
        buffer[i++] = (n % 10) + '0';  
        n /= 10;
    } while (n != 0);
    while (i > 0) {
      vidptr[current_loc++] = buffer[--i];
      vidptr[current_loc++] = color;
    }
}
void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
}

void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;
          		
		if(keycode == SPACE_KEY_CODE && startGame==0) {
		        
			startGame = 1;
                        moveShip = 1;
                        
                        updateMessage();
			return;
		}

		if(keycode == ENTER_KEY_CODE) {
			isStart = 1;	// oyunu başlat
			
			return;
		}
		if(keycode == UP_KEY_CODE) {
			moveBullet();
			return;
		}
		
		if(keycode == LEFT_KEY_CODE) {
		        if(shipX > 0 && isStart && moveShip)
		        {
		          clearShipLeft();
	                  shipX = shipX - 1;	//sola hareket ettirme
	                  if(bulletY == 25)
	                  {
	                  	bulletX = shipX;
	                  }
	                  renderShip();
		        }
			return;
		}
		
		if(keycode == RIGHT_KEY_CODE) {
		        if(shipX < 78 && isStart && moveShip)
		        {
		          clearShipRight();
	                  shipX = shipX + 1;	//sağa hareket ettirme
	                  if(bulletY == 25)
	                  {
	                  	bulletX = shipX;
	                  }
	                  renderShip();
		        }
			return; 
		}
		
		vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
		vidptr[current_loc++] = 0x07;
	}
}
void fps(int sec) // saniyede 60 kare için bekletme
{
	int i;
  for(i=0;i<sec;i++);
  if(moveShip==1)
  {
  	moveMeteor();
  	renderMeteor();
  	if(bulletY!=25)moveBullet();
  }
}
void cursorXY(unsigned int x, unsigned int y) //cursor hareket ettirme
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = BYTES_FOR_EACH_ELEMENT * (x * COLUMNS_IN_LINE + y);
}
void writeSTR(const char *str,unsigned int x, unsigned int y) //koordinata yazı yazma
{
	cursorXY(y,x);
	kprint(str,0x0F);
}
void renderShip(void) //gemi oluşturma
{
	const char *ship = "/\\";
	if(!gameOver) writeSTR(ship,shipX,shipY);
}
void renderBullet(void) //mermi oluşturma
{
	const char *bullet = "|";
	if(!gameOver) writeSTR(bullet,bulletX,bulletY);
}
void renderMeteor(void) //meteor çizimi
{
	writeSTR(meteor,meteor1X,meteor1Y);
	writeSTR(meteor,meteor2X,meteor2Y);
	writeSTR(meteor,meteor3X,meteor3Y);
	writeSTR(meteor,meteor4X,meteor4Y);
	writeSTR(meteor,meteor5X,meteor5Y);
	writeSTR(meteor,meteor6X,meteor6Y);
}
void clearMeteor(void) // meteor yer değiştirirken eski konumu silmelidir
{
	const char *meteor_delete = "   ";
	writeSTR(meteor_delete,meteor1X,meteor1Y);
	writeSTR(meteor_delete,meteor2X,meteor2Y);
	writeSTR(meteor_delete,meteor3X,meteor3Y);
	writeSTR(meteor_delete,meteor4X,meteor4Y);
	writeSTR(meteor_delete,meteor5X,meteor5Y);
	writeSTR(meteor_delete,meteor6X,meteor6Y);
}
void clearBullet(void) // mermi yer değiştirirken eski konumu silmelidir
{
	const char *bullet_delete = " ";
	writeSTR(bullet_delete,bulletX,bulletY);
}
void clearShipRight(void) // gemi yer değiştirirken eski konumu silmelidir
{
	const char *ship_delete = "  ";
	writeSTR(ship_delete,shipX,shipY);
}

void clearShipLeft(void) //gemi yer değiştirirken eski konumu silmelidir
{
	const char *ship_delete = "  ";
	writeSTR(ship_delete,shipX,shipY);
}
void clear_ship(void) //gemiyi yok etme
{
	const char *ship_delete = "  ";
	writeSTR(ship_delete,shipX,shipY);
}
void updateScore(void) // skor değişimi
{
        const char *scoreS = "Skor:";
        writeSTR(scoreS,1,0); 
        print_integer(score, 0x02);
}
void updateLife(void) // can değişimi
{
        const char *life = "Can:";
        writeSTR(life,70,0); 
        print_integer(lifeLeft, 0x04);
}
void updateMessage(void) // başlangıç ekranı yazdırma sildirme
{
  if(moveShip == 1)
      {
        const char *message = "                     ";
	writeSTR(message,32,15);
      }
}
void directionUpdate() //meteorları hareket ettirme
{	
  meteor1Y+=1; 
  meteor2Y+=1; 
  meteor3Y+=1; 
  meteor4Y+=1; 
  meteor5Y+=1; 
  meteor6Y+=1; 
  if(meteor1Y>25)meteor1Y=0;
  if(meteor2Y>25)meteor2Y=0;
  if(meteor3Y>25)meteor3Y=0;
  if(meteor4Y>25)meteor4Y=0;
  if(meteor5Y>25)meteor5Y=0;
  if(meteor6Y>25)meteor6Y=0;
}
void bulletUpdate() //mermi hareket ettirme
{	
  bulletY-=1;
  if(bulletY==0)bulletY=25;
}
void moveMeteor(void) // meteorları hareket ettirme
{
    clearMeteor();
    directionUpdate();
    checkCollision();
    renderMeteor();
}
void moveBullet(void)
{
	clearBullet();
	bulletUpdate();
	checkCollision();
	renderBullet();
}
void gameBoard(void) //oyun ekranı
{
    updateScore();
    updateLife();
    directionUpdate();
    kprint_newline();
    kprint_newline();
    renderShip();
    renderMeteor();
}
void checkCollision() // meteorlarla çarpışmayı kontrol eder
{
  
    if(meteor1Y==shipY||meteor1Y+1==shipY||meteor1Y+2==shipY)
    {
      if(meteor1X==shipX||meteor1X+1==shipX||meteor1X+2==shipX){
        lifeLeft--;
        meteor1Y=0;
        updateLife();
        if(lifeLeft==0){
          gameOver=1;
        }
      }
    }
    if(meteor2Y==shipY||meteor2Y+1==shipY||meteor2Y+2==shipY)
    {
      if(meteor2X==shipX||meteor2X+1==shipX||meteor2X+2==shipX){
        lifeLeft--;
        meteor2Y=0;
        updateLife();
        if(lifeLeft==0){
          gameOver=1;
        }
      }
    }
    if(meteor3Y==shipY||meteor3Y+1==shipY||meteor3Y+2==shipY)
    {
      if(meteor3X==shipX||meteor3X+1==shipX||meteor3X+2==shipX){
        lifeLeft--;
        meteor3Y=0;
        updateLife();
        if(lifeLeft==0){
          gameOver=1;
        }
      }
    }
    if(meteor4Y==shipY||meteor4Y+1==shipY||meteor4Y+2==shipY)
    {
      if(meteor4X==shipX||meteor4X+1==shipX||meteor4X+2==shipX){
        lifeLeft--;
        meteor4Y=0;
        updateLife();
        if(lifeLeft==0){
          gameOver=1;
        }
      }
    }
    if(meteor5Y==shipY||meteor5Y+1==shipY||meteor5Y+2==shipY)
    {
      if(meteor5X==shipX||meteor5X+1==shipX||meteor5X+2==shipX){
        lifeLeft--;
        meteor5Y=0;
        updateLife();
        if(lifeLeft==0){
          gameOver=1;
        }
      }
    }
    if(meteor6Y==shipY||meteor6Y+1==shipY||meteor6Y+2==shipY)
    {
      if(meteor6X==shipX||meteor6X+1==shipX||meteor6X+2==shipX){
        lifeLeft--;
        meteor6Y=0;
        updateLife();
        if(lifeLeft==0){
          gameOver=1;
        }
      }
    }

  if(meteor1Y==bulletY||meteor1Y+1==bulletY||meteor1Y+2==bulletY)
    {
      if(meteor1X==bulletX||meteor1X+1==bulletX||meteor1X+2==bulletX){
        clearMeteor();
        clearBullet();
      	bulletY=25;
      	bulletX=shipX;
        meteor1Y=0;
        score+=77;
        updateScore();
      }
    }
    if(meteor2Y==bulletY||meteor2Y+1==bulletY||meteor2Y+2==bulletY)
    {
      if(meteor2X==bulletX||meteor2X+1==bulletX||meteor2X+2==bulletX){
        clearMeteor();
        clearBullet();
        bulletY=25;
        bulletX=shipX;
        meteor2Y=0;
        score+=77;
        updateScore();
        
      }
    }
    if(meteor3Y==bulletY||meteor3Y+1==bulletY||meteor3Y+2==bulletY)
    {
      if(meteor3X==bulletX||meteor3X+1==bulletX||meteor3X+2==bulletX){
        clearMeteor();
        clearBullet();
        bulletY=25;
        bulletX=shipX;
        meteor3Y=0;
        score+=77;
        updateScore();
        
      }
    }
    if(meteor4Y==bulletY||meteor4Y+1==bulletY||meteor4Y+2==bulletY)
    {
      if(meteor4X==bulletX||meteor4X+1==bulletX||meteor4X+2==bulletX){
        clearMeteor();
        clearBullet();
        bulletY=25;
        bulletX=shipX;
        meteor4Y=0;
        score+=77;
        updateScore();
      }
    }
    if(meteor5Y==bulletY||meteor5Y+1==bulletY||meteor5Y+2==bulletY)
    {
      if(meteor5X==bulletX||meteor5X+1==bulletX||meteor5X+2==bulletX){
        clearMeteor();
        clearBullet();
        bulletY=25;
        bulletX=shipX;
        meteor5Y=0;
        score+=77;
        updateScore();
        
      }
    }
    if(meteor6Y==bulletY||meteor6Y+1==bulletY||meteor6Y+2==bulletY)
    {
      if(meteor6X==shipX||meteor6X+1==shipX||meteor6X+2==shipX){
        clearMeteor();
        clearBullet();
        bulletY=25;
        bulletX=shipX;
        meteor6Y=0;
        score+=77;
        updateScore();
        
      }
    }
}
void endScreen(void) // bitiş ekranı
{
  int i;
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint("        	                       Skorunuz : ",0x0F);
  print_integer(score,0x02);
  kprint_newline();
  kprint_newline();
  kprint("                               Lutfen Bekleyin                        ",0x0F);
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  while(gameOver == 1){fps(500000000);clear_screen();break;}
  kprint("Recep Toprak Gun                                          ",0x0B);
}
void startScreen(void) // başlangıç ekranı
{
  int i;
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint("                                  Space Odyysey                                  ",0x0F);
  kprint_newline();
  kprint_newline();
  kprint("                               Baslamak Icin ENTER                              ",0x0F);
  for(i = 0; i < 10;i++)
  {
    kprint_newline();
  }
  kprint(" Recep Toprak Gun                                                                ",0x0B);
  while(!isStart && !instr);
    
  clear_screen();
}
void gameLoop(void) // oyun yaşam döngüsü
{	
    gameBoard();
    while(1)
    {
      if(moveShip == 0)
      {
        const char *start_s = "Baslamak Icin Space";
	writeSTR(start_s,32,15);
      }
      
      if(gameOver == 1)
      {
        break;
      }
      fps(50000000); // oyun hızı
  }  
  clear_screen();
}
void kmain(void)
{
    idt_init();
    kb_init();  
    clear_screen();
    startScreen();
    gameLoop();
    endScreen(); 
}
