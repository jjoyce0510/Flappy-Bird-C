//flappy bird!
//final.c
//John Joyce/David Durkin

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "gfx5.h"
#define PI 3.141592653589793238462643

typedef struct Pipe {
	int holeDiameter; 
	double topHeight; 
	double bottomHeight; 
	double width; 
	double leadingX; 
    double trailingX;
} Pipe; 

void presentHomeScreen(int width, int height);
void initializePipe(Pipe* pipe, int scrWidth, int scrHeight); 
int startGame(Pipe* lead, Pipe* trail, int scrWidth, int scrHeight);
void drawPipe(Pipe* pipe, int scrWidth, int scrHeight); 
void updatePipe(Pipe* pipe);
void drawBackground(int width, int height); 
int endGame(int score, int highScore, int width, int height);
int presentEndScreen (int score, int highScore, int width, int height);
int drawBird(double, double, double, int);
void updateBird(double *, double *, double *, double, double, double);
void printScore(int, int);
void stationaryBird(double *y, double degrees);

int main() {

	int width = 400; 
	int height = 550;
    int gameLoop = 1;
    int highScore = 0;
    
    gfx_open(width, height, "Flappy Bird");
    
    //create two pipes to be recycled
    Pipe* leadPipe = (Pipe*)malloc(sizeof(Pipe));
    Pipe* trailPipe = (Pipe*)malloc(sizeof(Pipe));
    
    while (gameLoop) {
    
        //initialize each pipe
        initializePipe(leadPipe, width, height);
        initializePipe(trailPipe, width, height);
    
        //set background color
        gfx_clear_color(85, 203, 217);
        gfx_clear();
        
        //present game home screen
    
        char c;
        int flap = 0;
        double birdX = 150, birdY = 275, birdR = 12, degrees = 0;
        while (1) {
            gfx_clear();
            if(gfx_event_waiting()){
                if(gfx_wait()==' ') break;
            }
            presentHomeScreen(width, height);
            stationaryBird(&birdY,degrees);
            flap = drawBird(birdX, birdY, birdR, flap);
            gfx_flush();
            usleep(50000);
            degrees+=(2*M_PI)/20;
        }
    
        //begin animation
        int score = startGame(leadPipe, trailPipe, width, height);
        highScore = endGame(score, highScore, width, height);
        char d;
	
    }
 
	return 0; 
}

void presentHomeScreen(int width, int height) {
    
    //present home screen
    gfx_color(255, 255, 255);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-250-90-90-p-*-iso8859-1");
    gfx_text(width/2-88, 150, "GET READY");
    gfx_text(width/2-5, 50, "0");
    
    //draw spacebar
    gfx_color(255, 255, 255);
    gfx_fill_rectangle(width/2-100, height/2+60, 200, 30);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-120-75-75-p-*-iso8859-1");
    gfx_color(206, 212, 214);
    gfx_text(width/2-20, height/2+80, "SPACE");
    gfx_rectangle(width/2-100, height/2+60, 200, 30);
    
    //draw tap notice
    gfx_color(206, 212, 214);
    gfx_fill_rectangle(width/2-13, height/2+120, 26, 26);
    gfx_color(250, 0, 46);
    gfx_fill_rectangle(width/2-12, height/2+121, 24, 24);
    gfx_color(255, 255, 255);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-100-60-60-p-*-iso8859-1");
    gfx_text(width/2-8, height/2+136, "TAP");
    gfx_fill_rectangle(width/2-1, height/2+98, 2, 15);
    
    //draw background
    drawBackground(width, height);
    
}

void initializePipe(Pipe* pipe, int scrWidth, int scrHeight) {

	pipe->width = scrWidth/7; 
	pipe->holeDiameter = scrWidth/3; 
	pipe->leadingX = scrWidth;
	pipe->trailingX = scrWidth+pipe->width; 
	
	//random height
	int topHeight = 0; 
	
	while (topHeight < scrHeight/6) {
		topHeight = rand()%(scrHeight*2/3);
	}
	pipe->topHeight = topHeight; 
	pipe->bottomHeight = topHeight + pipe->holeDiameter;

}


int startGame(Pipe* lead, Pipe* trail, int scrWidth, int scrHeight) {

	int loop = 1; 
    int score = 0;
    double birdX = 150, birdY = 275, birdR = 12, birdV = 0, t = .05;
    int flap = 0;
   //#1 - to see game over screen make this while (loop < 5) and uncomment #2
	while (loop) {
        //if bird touches the drawpipe loop = 2

		gfx_clear();
        
        
        //draw background
        drawBackground(scrWidth, scrHeight);
        
        //check for loss
        if ((lead->leadingX-3 <= birdX+16 && lead->trailingX+3 >= birdX-16) &&
            (lead->topHeight >= birdY-16 || lead->bottomHeight <= birdY-16+24)) 
            break;
        if (birdY-16+24 >= scrHeight)
            break;
        
        flap = drawBird(birdX, birdY, birdR, flap);
        updateBird(&birdY, &birdV, &t, birdY, birdV, t);

        if(gfx_event_waiting()){
            char c = gfx_wait();
            if(c == ' ') birdV = -40;
        }
		
        //draw leading pipe
		drawPipe(lead, scrWidth, scrHeight);
        updatePipe(lead);

        //increase the score upon passing through a pair of pipes
        if(lead->trailingX == birdX){
            score++;
        }

        if (lead->trailingX < scrWidth/2) {
            drawPipe(trail, scrWidth, scrHeight);
            updatePipe(trail);
        }
        if (lead->trailingX <= 0) {
            //swap the leader
            Pipe *temp = lead;
            lead = trail;
            trail = temp;
            initializePipe(trail, scrWidth, scrHeight);
        }
   
        //print the score on the top of the screen
        printScore(score, scrWidth);

		gfx_flush(); 
        usleep(10000);
        t+=.001;
		//loop++ #2  -- to see end screen uncomment this and above
	}

    return score;

}


int endGame(int score, int highScore, int width, int height) {
    
    highScore = presentEndScreen(score, highScore, width, height);
    
    while (1) {
            //check click and location
            if(gfx_wait()== 1)
                if (gfx_xpos() > width/2-79 && gfx_xpos() < width/2-79+158 &&
                    gfx_ypos() > height/2+96 && gfx_ypos() < height/2+96+28) break;
        
    }
    
	return highScore; 

}

void updatePipe(Pipe* pipe) {

    pipe->leadingX = pipe->leadingX - 1;
    pipe->trailingX = pipe->trailingX - 1;

}

int presentEndScreen (int score, int highScore, int width, int height) {

    //present end screen
    gfx_color(255, 255, 255);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-250-90-90-p-*-iso8859-1");
    gfx_text(width/2-88, 150, "GAME OVER");
    
    //draw score block
    gfx_color(0,0,0);
    gfx_fill_rectangle(50, 180, width-100, 164);
    gfx_color(237, 206, 111);
    gfx_fill_rectangle(54, 184, width-108, 156);
    gfx_color(209, 181, 98);
    gfx_fill_rectangle(61, 191, width-122, 142);
    gfx_color(245, 228, 176);
    gfx_fill_rectangle(62, 192, width-124, 140);
    
    //labels
    gfx_color(209, 181, 98);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-160-85-85-p-*-iso8859-1");
    gfx_text(width/2-110, height/2-50, "MEDAL");
    gfx_text(width/2+50, height/2-50, "SCORE");
    gfx_text(width/2+65, height/2+8, "BEST");
    
    if (score > highScore) {
        highScore = score;
        //print new best label
        gfx_color(250, 0, 46);
        gfx_fill_rectangle(width/2+14, height/2-9, 46, 20);
        gfx_color(255, 255, 255);
        gfx_text (width/2+15, height/2+8, "NEW");
    }
    
    //scores
    gfx_changefont("-*-helvetica-bold-r-*-*-*-200-90-90-p-*-iso8859-1");
    char scoreStr[5], highScoreStr[5];
    sprintf(scoreStr, "%d", score);
    sprintf(highScoreStr, "%d", highScore);
    gfx_color(255, 255, 255);
    gfx_text(width/2+100, height/2-25, scoreStr);
    gfx_text(width/2+100, height/2+33, highScoreStr);
    
    
    //medals
    int red, green, blue;
    if (score > 40) {
        //platinum
        red = 245;
        green = 245;
        blue = 245;
    }
    else if (score > 30) {
        //gold
        red = 250;
        green = 239;
        blue = 82;
    }
    else if (score > 20) {
        //silver
        red = 214;
        green = 214;
        blue = 214;
    }
    else {
        //bronze
        red = 242;
        green = 152;
        blue = 87;
    }
    gfx_color(red, green, blue);
    gfx_fill_circle(width/2-80, height/2, 28);
    
    //medal bird
    int birdX = 105;
    int birdY = 264;
    
    //draw the bird    
    gfx_color(red, green, blue);
    gfx_fill_arc(birdX, birdY, 29, 24, 0, 360);
    
    gfx_color(red-60,green-20,blue);
    gfx_arc(birdX, birdY, 29, 24, 0, 360);
    
    gfx_color(red, green, blue);
    gfx_fill_circle(birdX+23,birdY+9,(0.50)*12+1);
    
    gfx_color(red-60,green-20,blue);
    gfx_circle(birdX+23,birdY+9,(0.50)*12+1);
    
    gfx_color(red, green, blue);
    gfx_fill_arc(birdX-2,birdY+4,11, 6, 0, 360);
    
    gfx_color(red-60,green-20,blue);
    gfx_arc(birdX-2,birdY+4,11, 6, 0, 360);
    
    gfx_color(red, green, blue);
    gfx_fill_arc(birdX+17, birdY+19, 14, 3, 0, 360);
    
    gfx_color(red-60,green-20,blue);
    gfx_arc(birdX+17, birdY+19, 14, 3, 0, 360);
    
    gfx_color(red, green, blue);
    gfx_fill_arc(birdX+17, birdY+15, 16, 4, 0, 360);
    
    gfx_color(red-60, green-20,blue);
    gfx_arc(birdX+17, birdY+15, 16, 4, 0, 360);
    
    gfx_color(red, green, blue);
    gfx_fill_rectangle(birdX+25,birdY+8, 2, 4);
    
    gfx_color(red-60,green-20,blue);
    gfx_rectangle(birdX+25,birdY+8, 2, 4);   
    
    //play again button
    gfx_color(0, 0, 0);
    gfx_fill_rectangle(width/2-79, height/2+96, 158, 28);
    gfx_color(255, 255, 255);
    gfx_fill_rectangle(width/2-77, height/2+98, 154, 24);
    gfx_color(250, 0, 46);
    gfx_fill_rectangle(width/2-75, height/2+100, 150, 20);
    gfx_color(255, 255, 255);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-120-75-75-p-*-iso8859-1");
    gfx_text(width/2-38, height/2+115, "PLAY AGAIN");
    
	return highScore; 

}

void drawPipe(Pipe* pipe, int scrWidth, int scrHeight) {

	//draw filled bottom half
    gfx_color(45, 214, 70);
	gfx_fill_rectangle(pipe->leadingX, pipe->bottomHeight+30, pipe->width, scrHeight-pipe->bottomHeight-30); 
	gfx_fill_rectangle(pipe->leadingX-3, pipe->bottomHeight, pipe->width+6, 30);
    
    //draw filled top half
    gfx_fill_rectangle(pipe->leadingX, 0, pipe->width, pipe->topHeight);
    gfx_fill_rectangle(pipe->leadingX-3, pipe->topHeight-30, pipe->width+6, 30);
    
    
    //draw bottom half outline
    gfx_color(69, 71, 61);
    gfx_rectangle(pipe->leadingX, pipe->bottomHeight+30, pipe->width, scrHeight-pipe->bottomHeight-30);
    gfx_rectangle(pipe->leadingX-3, pipe->bottomHeight, pipe->width+6, 30);
    //top half outline
    gfx_rectangle(pipe->leadingX, -1, pipe->width, pipe->topHeight-30+1);
    gfx_rectangle(pipe->leadingX-3, pipe->topHeight-30, pipe->width+6, 30);
    
}


void drawBackground(int width, int height) {
    
    //draw clouds
    gfx_color(255, 255, 255);
    int i;
    for (i = 0; i<10; i++) {
        gfx_fill_circle(i*width/8, height - height/10, 50);
    }
    for (i = 0; i<3; i++) {
      gfx_fill_circle(i*width/3, height - height/7, 40);
  	}
    
    gfx_fill_circle(width*3/4+10, height - height/6+10, 45); 
    gfx_fill_circle(width*2/9-5, height - height/7, 45); 
    
    //draw buildings
    
    for (i = 0; i<5; i++) {
        //outlines
        gfx_color(200, 207, 201);
        gfx_rectangle(-30+i*width/4-1, height-60-1, 22, 60);
        gfx_rectangle(-30+i*width/4+20-1, height-55-1, 22, 55);
        gfx_rectangle(-30+i*width/4+25-1, height-45-1, 27, 45);
        gfx_rectangle(-30+i*width/4+30-1, height-75-1, 32, 75);
        gfx_rectangle(-30+i*width/4+30-1, height-80-1, 15, 80);
        gfx_rectangle(-30+i*width/4+50-1, height-62-1, 22, 62);
        gfx_rectangle(-30+i*width/4+55-1, height-55-1, 27, 55);
        //fills
        gfx_color(218, 227, 219);
        gfx_fill_rectangle(-30+i*width/4, height-60, 20, 60);
        gfx_fill_rectangle(-30+i*width/4+20, height-55, 20, 55);
        gfx_fill_rectangle(-30+i*width/4+25, height-45, 25, 45);
        gfx_fill_rectangle(-30+i*width/4+30, height-75, 30, 75);
        gfx_fill_rectangle(-30+i*width/4+30, height-80, 15, 80);
        gfx_fill_rectangle(-30+i*width/4+50, height-62, 20, 62);
        gfx_fill_rectangle(-30+i*width/4+55, height-55, 25, 55);
   
    }
    
    //draw greenery
    for (i = 0; i<10; i++) {
        gfx_color(107, 179, 118);
        gfx_circle(i*width/8, height - 5, 30);
        gfx_color(153, 224, 163);
        gfx_fill_circle(i*width/8, height - 5, 30);
    }
    for (i = 0; i<6; i++) {
        gfx_fill_circle(i*width/6, height - 5, 25);
    }
    for (i = 0; i<10; i++) {
        gfx_color(107, 179, 118);
        gfx_circle(i*width/8+20, height - 5, 24);
        gfx_color(153, 224, 163);
        gfx_fill_circle(i*width/8+20, height - 5, 24);
    }
   
    //draw base rectangle to cover outlines
    gfx_fill_rectangle(0, height-15, width, 15);
        
}

int drawBird(double x, double y, double r, int flap){
	
    //body
    gfx_color(255,255,0);
    //gfx_fill_circle(x,y,r);
	gfx_fill_arc(x-16, y-16, 29, 24, 0, 360 );
    gfx_color(0,0,0);
    gfx_arc(x-16, y-16, 29, 24, 0, 360); 
    
    //eye ball
    gfx_color(255,255,255);
    gfx_fill_circle(x+7,y-7,(0.50)*r+1);

	//wings
    if(flap<=5){
        gfx_fill_arc(x-18,y-11,12, 6, 0, 360);
        gfx_color(0,0,0);
        gfx_arc(x-18,y-12,11, 6, 0, 360);
        flap++;
    }
    else if( (flap>=5)&&(flap<=10) ){
        gfx_fill_arc(x-18,y-7, 12, 6, 0, 360);
        gfx_color(0,0,0);
        gfx_arc(x-18,y-7, 12, 6, 0, 360);
        flap++;     
    }
    else{
        gfx_fill_arc(x-18,y-5,12, 6, 0, 360);
        gfx_color(0,0,0);
        gfx_arc(x-18,y-5,12, 6, 0, 360);
        flap--;
    }
    
    //pupil
    gfx_fill_rectangle(x+9,y-8,2,4);
    gfx_circle(x+7,y-7,(0.50)*r+1);

	//lips
    gfx_color(255,128,0);
    gfx_fill_arc(x+1, y-1, 16, 4, 0, 360); 
	gfx_fill_arc(x+1, y+3, 14, 3, 0 ,360); 

    gfx_color(0,0,0);
    gfx_arc(x+1, y+3, 14, 3, 0, 360);
    gfx_arc(x+1, y-1, 16, 4, 0, 360);  

    gfx_color(255,128,0);
    gfx_fill_rectangle(x+1,y+0,4,6);

    return flap;

}

void updateBird(double *y, double *v, double *t, double yVal, double vVal, double 
tVal){

    double deltaY;
    double vValNew;
    //calculates new y position and velocity using kinematic equations
    deltaY = vVal*tVal + (4.9)*pow(tVal,2);
    yVal = yVal + deltaY;
    vValNew = vVal + (15)*tVal;

    //resets the time variable if the velocity changes sign
    //this prevents the velocity value from increasing wildly
    if((vValNew>=0&&vVal<=0)||(vVal>=0&&vValNew<=0)){
        *t = .05;
    }

    *y = yVal;
    *v = vValNew;

}

void printScore(int score, int width){

    //Prints the score at the top of the screen
    gfx_color(255, 255, 255);
    gfx_changefont("-*-helvetica-bold-r-*-*-*-250-90-90-p-*-iso8859-1");
    char str[10];
    sprintf(str, "%d", score);
    gfx_text(width/2-5, 50, str);

}

void stationaryBird(double *y, double degrees){

    //moves the y position up and down continuously
    *y = 250 + 15*sin(degrees);

}
