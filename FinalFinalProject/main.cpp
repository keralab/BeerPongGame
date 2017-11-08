/*
	Nolan Slade  		001401079
	Vyome Kishore		001406201
	Kerala Brendon		001424625

	CS 3GC3 - Final Project
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cstring>
#include <string>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif


/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	Constant and Global Declarations
*/

/* Program */
#define SCREEN_WIDTH 		1000.0
#define SCREEN_HEIGHT 		1000.0

/* Game Room */
#define ROOM_DEPTH			2000.0
#define ROOM_WIDTH			1500.0
#define ROOM_HEIGHT			1500.0

/* Table */
#define TABLE_HEIGHT		500.0
#define TABLE_DEPTH			1000.0
#define TABLE_WIDTH			600.0
#define TABLE_THICKNESS		50.0
#define LEG_THICKNESS		25.0

/* Players */
#define PLAYER_HEAD_HEIGHT 	1000.0 			// Midpoint of head
#define PLAYER_BODY_HEIGHT	800.0 			// Midpoint of body
#define PLAYER_LEG_HEIGHT	250.0 			// Midpoint of legs
#define PLAYER_ARM_HEIGHT 	750.0 			// Midpoint of arms
#define PLAYER_TABLE_DIST	400.0 			// Distance from table

/* Game Specific */
#define NUM_CUPS 			6
#define BALL_RADIUS			20.0
#define BALL_SLICES 		100
#define CUP_RADIUS			40.0
#define CUP_HEIGHT      	70.0 
#define GRAVITY				9.8 			// Used for projectile motion for ball trajectory
#define PI 					3.14159265
#define RETICULE_HEIGHT		150 			// How high above the table the target will show up
#define COMPONENT_INC		5.0 			// How much a player's trajectory vectory will change on keystrokes
#define THROW_HEIGHT		100.0 			// Height that the ball is thrown from (distance above table)
#define POWER_CONST			15.0 			// Multiplier used for ball speed in conjunction with shot power
#define WINDOW_SIZE_WIDTH 	1000
#define WINDOW_SIZE_HEIGHT 	1000

/* Gameplay Globals */
int mouseX = 0, mouseY = 0; //ADDED FOR RAY PICKING
int screen=0; //0=Menu 1=Game 2=Controls 3,4... depending on what other menus we need like controls,help etc.
bool paused=false;
char currentPlayer = 'f';													// 'f' for first player, 's' for second
int score[] = {6, 6};														// Number of games won, playerOne - playerTwo
int firstPlayerPower = 5;													// 1 - 10 for player 1's shot power
int secondPlayerPower = 5;													// 1 - 10 for player 2's shot power
float firstPlayerShotVec[] = {0, 0, 0};										// Where playerOne is aiming their shot (x,y,z)
float secondPlayerShotVec[] = {0, 0, 0};									// where playerTwo is aiming their shot
bool playerOneShooting = false;												// Whether or not player 1 is shooting their ball
bool playerTwoShooting = false;												// Whether or not player 2 is shooting their ball
float shootingAngle = -75;													// Ball throwing rotation angle
float endPosition[] = {-1000,1000,-1000};									// Impact point

// Player scores
int redScore = 0;
int greenScore = 0;

bool playerOneActiveCups[] = {true, true, true, true, true, true};			// Cups still in play for player one

// Player one's cup coordinates (x,z)
float playerOneCupsX[] = {-2 * CUP_RADIUS, 0, 2 * CUP_RADIUS, -1 * CUP_RADIUS, CUP_RADIUS, 0};		
float playerOneCupsZ[] = {-1 * TABLE_DEPTH / 2 + (2*CUP_RADIUS), -1 * TABLE_DEPTH / 2 + (2*CUP_RADIUS), 
						-1 * TABLE_DEPTH / 2 + (2*CUP_RADIUS), -1 * TABLE_DEPTH / 2 + (4 *CUP_RADIUS), 
						-1 * TABLE_DEPTH / 2 + (4 *CUP_RADIUS), -1 * TABLE_DEPTH / 2 + (6 *CUP_RADIUS)};	

bool playerTwoActiveCups[] = {true, true, true, true, true, true};			// Cups still in play for player two

// Player one's cup coordinates (x,z)

float playerTwoCupsX[] = {-2 * CUP_RADIUS, 0, 2 * CUP_RADIUS, -1 * CUP_RADIUS, CUP_RADIUS, 0};		
float playerTwoCupsZ[] = {TABLE_DEPTH / 2 - (2*CUP_RADIUS), TABLE_DEPTH / 2 - (2*CUP_RADIUS), 
						TABLE_DEPTH / 2 - (2*CUP_RADIUS), TABLE_DEPTH / 2 - (4 *CUP_RADIUS), 
						TABLE_DEPTH / 2 - (4 *CUP_RADIUS), TABLE_DEPTH / 2 - (6 *CUP_RADIUS)};
float playerScale = 1.0;

// Rotation of player members to aniamte them
int rotations=0;
int rotateAmt=0;

// Switching players when a turn is over
bool switchingPlayer=false;

// Sway effect
bool swayDir=false;
float swayRed,swayGreen;

// Fog effect
bool fogEnabled=false;
float fogColor[4]={1,0,0,1};

// Menu interaction
bool onPlay=false;
bool onQuit=false;

/* Camera Globals */
float camPos[] = {0.0f, ROOM_HEIGHT / 2, (-1 * ROOM_DEPTH / 2) + ROOM_DEPTH / 7};
float camUp[] = { 0.0f, 1, 0};
float camTarget[] = { 0.0f, TABLE_HEIGHT, 0.0f };
float camSpeed = 0.4f;
float playerOneCam[] = {0.0f, ROOM_HEIGHT / 2, (-1 * ROOM_DEPTH / 2) + ROOM_DEPTH / 7};
float playerTwoCam[] = {0.0f, ROOM_HEIGHT / 2, (ROOM_DEPTH / 2) - ROOM_DEPTH / 7};

/* Lighting Globals */
float light_pos[] = {0, ROOM_HEIGHT-50 , 0, 1};
float amb0[4]  = {1, 1, 1, 1}; 
float diff0[4] = {1, 1, 1, 1};
float spec0[4] = {1, 1, 1, 1}; 

/* Material Globals */
struct Material {
	int id;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shine;
} obsidian, blackrubber, brass, pearl, greenrubber, redrubber, redplastic, whiteplastic, wood;


/*
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/*
	Mesh information, struct information for key objects

	Defines mesh information for:
		- Table
		- Game room
 
    Plus, data is defined for our textures and ball struct.
*/

/* Beer Pong Table */
float table_verts[8][3] = {
	// Coordinates are relative to room center and size
	// List is structured the same as cube in week 6 slides, slide 15
	// Positive Depth (z) goes into the screen, x goes left to right, y goes bottom to top
	{ -1 * (TABLE_WIDTH / 2), TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) },	// 0
	{ -1 * (TABLE_WIDTH / 2), TABLE_HEIGHT, -1 * (TABLE_DEPTH / 2) }, 					// 1
	{ TABLE_WIDTH / 2, TABLE_HEIGHT, -1 * (TABLE_DEPTH / 2) }, 							// 2
	{ TABLE_WIDTH / 2, TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) }, 		// 3
	{ -1 * (TABLE_WIDTH / 2), TABLE_HEIGHT - TABLE_THICKNESS, TABLE_DEPTH / 2 }, 		// 4 
	{ -1 * (TABLE_WIDTH / 2), TABLE_HEIGHT, TABLE_DEPTH / 2 },							// 5
	{ TABLE_WIDTH / 2, TABLE_HEIGHT, TABLE_DEPTH / 2 },									// 6
	{ TABLE_WIDTH / 2, TABLE_HEIGHT - TABLE_THICKNESS, TABLE_DEPTH / 2 }				// 7
};
float table_faces[6][4] = {
	{1, 2, 3, 0},		// Front Face
	{4, 5, 6, 7},		// Back Face
	{7, 6, 2, 3},		// Right Face
	{0, 1, 5, 4},		// Left Face
	{2, 6, 5, 1},		// Top Face
	{7, 3, 0, 4}		// Bottom Face
};

/* Table Legs */
/* Leg order defined here is back-left, back-right, front-left, front-right */
float leg_verts[32][3] = {
	// Back-left leg
	{ -1 * (TABLE_WIDTH / 2), 0, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS},													// 0
	{ -1 * (TABLE_WIDTH / 2), TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS},						// 1
	{ -1 * (TABLE_WIDTH / 2) + LEG_THICKNESS, TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS },		// 2
	{ -1 * (TABLE_WIDTH / 2) + LEG_THICKNESS, 0, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS },									// 3
	{ -1 * (TABLE_WIDTH / 2), 0, -1 * (TABLE_DEPTH / 2) },																	// 4
	{ -1 * (TABLE_WIDTH / 2), TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) },										// 5
	{ -1 * (TABLE_WIDTH / 2) + LEG_THICKNESS, TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) },						// 6
	{ -1 * (TABLE_WIDTH / 2) + LEG_THICKNESS, 0, -1 * (TABLE_DEPTH / 2) },													// 7

	// Back-right leg
	{ (TABLE_WIDTH / 2) - LEG_THICKNESS,0, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS},											// 8  (0)
	{ (TABLE_WIDTH / 2) - LEG_THICKNESS,TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS},			// 9  (1)
	{(TABLE_WIDTH / 2),TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS},								// 10 (2)
	{(TABLE_WIDTH / 2),0, -1 * (TABLE_DEPTH / 2) + LEG_THICKNESS},															// 11 (3)
	{ (TABLE_WIDTH / 2) - LEG_THICKNESS,0, -1 * (TABLE_DEPTH / 2) },														// 12 (4)
	{ (TABLE_WIDTH / 2) - LEG_THICKNESS,TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) },							// 13 (5)
	{(TABLE_WIDTH / 2),TABLE_HEIGHT - TABLE_THICKNESS, -1 * (TABLE_DEPTH / 2) },											// 14 (6)
	{(TABLE_WIDTH / 2),0, -1 * (TABLE_DEPTH / 2) },																			// 15 (7)

	// Front-left leg
	{-1 * (TABLE_WIDTH / 2),0,(TABLE_DEPTH / 2) - LEG_THICKNESS},															// 16 (0)
	{-1 * (TABLE_WIDTH / 2),TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2) - LEG_THICKNESS},								// 17 (1)
	{-1 * (TABLE_WIDTH / 2)+ LEG_THICKNESS,TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2) - LEG_THICKNESS},				// 18 (2)
	{-1 * (TABLE_WIDTH / 2)+ LEG_THICKNESS,0,(TABLE_DEPTH / 2) - LEG_THICKNESS},											// 19 (3)
	{-1 * (TABLE_WIDTH / 2),0,(TABLE_DEPTH / 2)},																			// 20 (4)
	{-1 * (TABLE_WIDTH / 2),TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2)},												// 21 (5)
	{-1 * (TABLE_WIDTH / 2)+ LEG_THICKNESS,TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2)},								// 22 (6)
	{-1 * (TABLE_WIDTH / 2)+ LEG_THICKNESS,0,(TABLE_DEPTH / 2)},															// 23 (7)

	// Front-right leg
	{(TABLE_WIDTH / 2) - LEG_THICKNESS,0,(TABLE_DEPTH / 2) - LEG_THICKNESS},												// 24 (0)
	{(TABLE_WIDTH / 2) - LEG_THICKNESS,TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2) - LEG_THICKNESS},					// 25 (1)
	{(TABLE_WIDTH / 2),TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2) - LEG_THICKNESS},									// 26 (2)
	{(TABLE_WIDTH / 2),0,(TABLE_DEPTH / 2) - LEG_THICKNESS},																// 27 (3)
	{(TABLE_WIDTH / 2) - LEG_THICKNESS,0,(TABLE_DEPTH / 2)},																// 28 (4)
	{(TABLE_WIDTH / 2) - LEG_THICKNESS,TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2)},									// 29 (5)
	{(TABLE_WIDTH / 2),TABLE_HEIGHT - TABLE_THICKNESS,(TABLE_DEPTH / 2)},													// 30 (6)
	{(TABLE_WIDTH / 2),0,(TABLE_DEPTH / 2)},																				// 31 (7)
};
float leg_faces[24][4] = {
	// Back-left leg
	{1, 2, 3, 0},			// Front Face
	{4, 5, 6, 7},			// Back Face
	{7, 6, 2, 3},			// Right Face
	{0, 1, 5, 4},			// Left Face
	{2, 6, 5, 1},			// Top Face
	{7, 3, 0, 4},			// Bottom Face

	// Back-right leg
	{9, 10, 11, 8},			// Front Face
	{12, 13, 14, 15},		// Back Face
	{15, 14, 10, 11},		// Right Face
	{8, 9, 13, 12},			// Left Face
	{10, 14, 13, 9},		// Top Face
	{15, 11, 8, 12},		// Bottom Face

	// Front-left leg
	{17, 18, 19, 16},		// Front Face
	{20, 21, 22, 23},		// Back Face
	{23, 22, 18, 19},		// Right Face
	{16, 17, 21, 20},		// Left Face
	{18, 22, 21, 17},		// Top Face
	{23, 19, 16, 20},		// Bottom Face

	// Front-right leg
	{25, 26, 27, 24},		// Front Face
	{28, 29, 30, 31},		// Back Face
	{31, 30, 26, 27},		// Right Face
	{24, 25, 29, 28},		// Left Face
	{26, 30, 29, 25},		// Top Face
	{31, 27, 24, 28}		// Bottom Face
};


/* Walls, floor and ceiling to make up the room */
float room_verts[8][3] = {
	// Vertices organized as in table_verts (cube from slides), relative to room center and size
	{ -1 * (ROOM_WIDTH / 2), 0.0f, -1 * (ROOM_DEPTH / 2) }, 			// 0
	{ -1 * (ROOM_WIDTH / 2), ROOM_HEIGHT, -1 * (ROOM_DEPTH / 2) }, 		// 1
	{ ROOM_WIDTH / 2, ROOM_HEIGHT, -1 * (ROOM_DEPTH / 2) }, 			// 2
	{ ROOM_WIDTH / 2, 0.0f, -1 * (ROOM_DEPTH / 2) }, 					// 3
	{ -1 * (ROOM_WIDTH / 2), 0.0f, ROOM_DEPTH / 2 }, 					// 4 
	{ -1 * (ROOM_WIDTH / 2), ROOM_HEIGHT, ROOM_DEPTH / 2 },				// 5
	{ ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_DEPTH / 2 },					// 6
	{ ROOM_WIDTH / 2, 0.0f, ROOM_DEPTH / 2 }							// 7
};
float room_faces[6][4] = {
	// Room faces are drawn so that they are visible from the inside of the room
	{ 1, 2, 3, 0 },		// Front Face
	{ 6, 5, 4, 7 },		// Back Face
	{ 2, 6, 7, 3 },		// Right Face
	{ 5, 1, 0, 4 },		// Left Face
	{ 6, 2, 1, 5 },		// Top Face
	{ 3, 7, 4, 0 }		// Bottom Face
};

// Face normals for a rectangular prism shape
float rect_prism_face_normals[6][3] = {
	{0,0,1},
	{0,0,-1},
	{-1,0,0},
	{1,0,0},
	{0,-1,0},
	{0,1,0}
};

/* Ball Position for Trajectory and Ray Picking */
struct shapeStruct{
	float posX, posY, posZ;
	bool intersect;
} ball;


/* Textures */
GLubyte* image;
GLubyte* image2;
int width, height, max;
GLuint myTex[2];
bool drawingTable=false;
bool drawingRoom=false;


/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	Instruction Printing Method

	Prints clear instructions for user in order for them to understand
	all aspects of game usage and controls.
*/

void printInstructions() {
	printf("\n**************************\nWelcome to 3GC3 Beer Pong!\n**************************\n\n");
	printf("Written by: Nolan Slade, Kerala Brendon, Vyome Kishore\n");

	printf("\nGameplay Instructions:\n");
	printf("\t - Each player's goal is to throw the ball into the opposing player's cups to eliminate them.\n");
	printf("\t - Once all of the opposing player's cups have been removed, the player wins that round.\n");
	printf("\t - Upon losing a round, a player will become more intoxicated and will have a harder time playing.\n");

	printf("\nControl Instructions:\n");
	printf("\t- When it is their turn, a target will be shown to the player to give an approximation as to where their ball will land.\n");
	printf("\t- The current player may alter their shot power (1-10), with 'p' for increase, and 'alt + p' for decrease.\n");
	printf("\t- The current player may also alter their ball trajectory with the arrow keys.\n");
	printf("\t- Finally, the player may throw the ball by clicking the ball.\n");
	printf("\t- To pause the game at any time during play, use the capital 'P' key.\n");
	printf("\t- To exit to the main menu at any time during play, use 'esc' key.\n");
}

/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	Auxiliary Methods

	Defines methods for use throughout program, including:
		- Texture PPM file loading
		- Resetting program
		- Cup counting
		- Switch materials
*/

/* Switch materials */
void switchMaterial(int matNo) {
	// Given a material number, switch to the right material
	switch (matNo) {
		case 0:	
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  obsidian.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  obsidian.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  obsidian.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  obsidian.shine);
			break;

		case 1:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  blackrubber.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  blackrubber.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  blackrubber.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  blackrubber.shine);
			break;

		case 2:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  brass.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  brass.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  brass.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  brass.shine);
			break;

		case 3:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  pearl.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  pearl.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pearl.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  pearl.shine);
			break;

		case 4:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  greenrubber.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  greenrubber.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  greenrubber.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  greenrubber.shine);
			break;

		case 5:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  redrubber.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  redrubber.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  redrubber.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  redrubber.shine);
			break;

		case 6:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  redplastic.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  redplastic.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  redplastic.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  redplastic.shine);
			break;

		case 7:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  whiteplastic.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  whiteplastic.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  whiteplastic.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  whiteplastic.shine);
			break;

		case 8:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  wood.ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  wood.diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  wood.specular); 
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  wood.shine);
			break;
	}
}


/* Retrieves the number of active cups for a given player, first 'f', or second 's' */
int cupsRemaining (char c) {
	int active = 0;

	// We run through the corresponding cup array for the player and count the active cups
	if(c == 'f'){
		for (int i = 0; i < NUM_CUPS; i++) {
			if (playerOneActiveCups[i]) {
				active++;	
			}
		}
	}
	else if (c == 's') {
		for (int i = 0; i < NUM_CUPS; i++){
			if (playerTwoActiveCups[i]) {
				active++;	
			}
		}
	}
	return active;
}

/* Resets the game for the next round of play */
void resetProgram() {
	playerOneShooting = false;
	playerTwoShooting = false;

	for (int i = 0; i < NUM_CUPS; i++) {
		playerOneActiveCups[i] = true;
		playerTwoActiveCups[i] = true;
	}

	score[0]=6;
	score[1]=6;
	currentPlayer = 'f';
	ball.posX = 0;
	ball.posY = TABLE_HEIGHT + THROW_HEIGHT;
	ball.posZ = (-1 * TABLE_DEPTH / 2) + (3 * BALL_RADIUS);
	ball.intersect = false;
}

/* Loads PPM Files */
GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    
    /* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
    
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        // Quit if the file type is not PPM
        exit(0);
    }
    
    fscanf(fd, "%c",&c);
    
    // Ignore comments
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    
    // Retrieve dimensions and max colour values
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    /* calculate number of pixels and allocate storage for this */
    nm = n*m;
    img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);
    s=255.0/k;
    
    /* for every pixel, grab the read green and blue values, storing them in the image data array */
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    // Set return parameters, return image array
    *width = n;
    *height = m;
    *max = k;
    
    return img;
}


/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	Object Drawing methods

	Defines drawing for objects with defined mesh information
	including vertices and faces. Responsible for drawing:
		- Rectangular prisms (table and table legs, room..)

	Also defines how to calculate and draw the current player's ball trajectory,
	and how to draw players
*/

/* Rectangular prism drawing method, takes in verts and faces, and a boolean to signify which way we want the faces to face */
void drawRectPrism(float objectVertices[8][3], float objectFaces[6][4], bool insidePrism) {
	// Loop through faces, drawing corresponding vertices in the right order
	for (int i = 0; i < 6; i++) {
		// For each face, we have a list of 4 vertices to draw
		int firstVert  = objectFaces[i][0]; 
		int secondVert = objectFaces[i][1];
		int thirdVert  = objectFaces[i][2];
		int fourthVert = objectFaces[i][3];

		// Index values for all vertex normal calculations; three indices correspond to the 3 faces that touch the vertex e.g. [0]{0,1,2} would signify 
		// that the firstVertex connects to faces with normals face_normals[0,1, and 2]
		int norm_inds[4][3] = { {0, 0, 0}, 		// First vertex attaches these three faces
								{0, 0, 0}, 		// Second vertex attaches these three faces
								{0, 0, 0}, 		// Third vertex attaches these three faces
								{0, 0, 0} };	// Fourth vertex attaches these three faces
																							
		float magnitude = 0; 	// Used to normalize the vertex vector normals

		// The values of the vertex normals depend on which face we are drawing
		if (insidePrism) {	// i.e. drawing room - we are inside the room, so our normals are calculated relative to a different vertex order
			switch (i) {
				// We are drawing face with vertex ordering 1, 2, 3, 0
				case 0:
					// First vertex (1) connects front (0), left (3), top of room (4), 
					// Second vertex (2) connects right (2), front (0), top (4)
					// Third vertex (3) connects bottom(5), front (0), right (2)
					// Fourth vertex (0) connects bottom(5), front (0), left (3)
					// Face normals have indices: front = 0, back = 1, right = 2, left = 3, top = 4, bottom = 5
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;	// Vertex 1 (left,top,front)
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;	// Vertex 2 (front,top,right)
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;	// Vertex 3 (front,bottom,right)
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;	// Vertex 0 (left,bottom,front)
					break;

				// We are drawing face with vertex ordering 6, 5, 4, 7
				case 1:
					norm_inds[0][0] =  2; 	norm_inds[0][1] =  4;	norm_inds[0][2] =  1;	// Vertex 6 (right,top,back)
					norm_inds[1][0] =  3; 	norm_inds[1][1] =  4;	norm_inds[1][2] =  1;	// Vertex 5 (left,top,back)
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  3;	norm_inds[2][2] =  1;	// Vertex 4 (bottom,left,back)
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  2;	norm_inds[3][2] =  1;	// Vertex 7 (bottom,right,back)
					break;

				// We are drawing face with vertex ordering 2, 6, 7, 3
				case 2:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  4;	norm_inds[0][2] =  2; 	// Vertex 2 (front,top,right)
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  4;	norm_inds[1][2] =  1;	// Vertex 6 (right,top,back)
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  2;	norm_inds[2][2] =  1;	// Vertex 7 (bottom,right,back)
					norm_inds[3][0] =  0; 	norm_inds[3][1] =  5;	norm_inds[3][2] =  2;	// Vertex 3 (front,bottom,right)
					break;

				// We are drawing face with vertex ordering 5, 1, 0, 4
				case 3:
					norm_inds[0][0] =  3; 	norm_inds[0][1] =  4;	norm_inds[0][2] =  1;	// Vertex 5 (left,top,back)
					norm_inds[1][0] =  3; 	norm_inds[1][1] =  4;	norm_inds[1][2] =  0;	// Vertex 1 (left,top,front)
					norm_inds[2][0] =  3; 	norm_inds[2][1] =  5;	norm_inds[2][2] =  0;	// Vertex 0 (left,bottom,front)
					norm_inds[3][0] =  3; 	norm_inds[3][1] =  5;	norm_inds[3][2] =  1;	// Vertex 4 (left,bottom,back)
					break;

				// We are drawing face with vertex ordering 6, 2, 1, 5
				case 4:
					norm_inds[0][0] =  2; 	norm_inds[0][1] =  4;	norm_inds[0][2] =  1;	// Vertex 6 (right,top,back)
					norm_inds[1][0] =  0; 	norm_inds[1][1] =  4;	norm_inds[1][2] =  2;	// Vertex 2 (front,top,right)
					norm_inds[2][0] =  3; 	norm_inds[2][1] =  4;	norm_inds[2][2] =  0;	// Vertex 1 (left,top,front)
					norm_inds[3][0] =  3; 	norm_inds[3][1] =  4;	norm_inds[3][2] =  1;	// Vertex 5 (left,top,back)
					break;

				// We are drawing face with vertex ordering 3, 7, 4, 0
				case 5:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  5;	norm_inds[0][2] =  2;	// Vertex 3 (front,bottom,right)
					norm_inds[1][0] =  5; 	norm_inds[1][1] =  2;	norm_inds[1][2] =  1;	// Vertex 7 (bottom,right,back)
					norm_inds[2][0] =  3; 	norm_inds[2][1] =  5;	norm_inds[2][2] =  1;	// Vertex 4 (left,bottom,back)
					norm_inds[3][0] =  3; 	norm_inds[3][1] =  5;	norm_inds[3][2] =  0;	// Vertex 0 (left,bottom,front)
					break;
			}
		// Drawing an outside-facing rectangular prism (i.e. table)
		} else {
			switch (i) {          
				case 0:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;	
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;
					break;

				case 1:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;
					break;

				case 2:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;
					break;

				case 3:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;
					break;

				case 4:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;
					break;

				case 5:
					norm_inds[0][0] =  0; 	norm_inds[0][1] =  3;	norm_inds[0][2] =  4;
					norm_inds[1][0] =  2; 	norm_inds[1][1] =  0;	norm_inds[1][2] =  4;
					norm_inds[2][0] =  5; 	norm_inds[2][1] =  0;	norm_inds[2][2] =  2;
					norm_inds[3][0] =  5; 	norm_inds[3][1] =  0;	norm_inds[3][2] =  3;
					break;
			}
		}

		// The first vertex uses the appropriate index values and computes the corresponding normal
		float firstVertNormal[]  = {rect_prism_face_normals[norm_inds[0][0]][0]+rect_prism_face_normals[norm_inds[0][1]][0]+rect_prism_face_normals[norm_inds[0][2]][0],
							rect_prism_face_normals[norm_inds[0][0]][1]+rect_prism_face_normals[norm_inds[0][1]][1]+rect_prism_face_normals[norm_inds[0][2]][1],
							rect_prism_face_normals[norm_inds[0][0]][2]+rect_prism_face_normals[norm_inds[0][1]][2]+rect_prism_face_normals[norm_inds[0][2]][2]};
		magnitude = sqrt(pow(firstVertNormal[0],2) + pow(firstVertNormal[1],2) + pow(firstVertNormal[2],2));
		firstVertNormal[0] /= magnitude;	firstVertNormal[1] /= magnitude;	firstVertNormal[2] /= magnitude;

		// The second vertex uses the appropriate index values and computes the corresponding normal
		float secondVertNormal[] = {rect_prism_face_normals[norm_inds[1][0]][0]+rect_prism_face_normals[norm_inds[1][1]][0]+rect_prism_face_normals[norm_inds[1][2]][0],
							rect_prism_face_normals[norm_inds[1][0]][1]+rect_prism_face_normals[norm_inds[1][1]][1]+rect_prism_face_normals[norm_inds[1][2]][1],
							rect_prism_face_normals[norm_inds[1][0]][2]+rect_prism_face_normals[norm_inds[1][1]][2]+rect_prism_face_normals[norm_inds[1][2]][2]};
		magnitude = sqrt(pow(secondVertNormal[0],2) + pow(secondVertNormal[1],2) + pow(secondVertNormal[2],2));
		secondVertNormal[0] /= magnitude;	secondVertNormal[1] /= magnitude;	secondVertNormal[2] /= magnitude;

		// The third vertex uses the appropriate index values and computes the corresponding normal
		float thirdVertNormal[]  = {rect_prism_face_normals[norm_inds[2][0]][0]+rect_prism_face_normals[norm_inds[2][1]][0]+rect_prism_face_normals[norm_inds[2][0]][0],
							rect_prism_face_normals[norm_inds[2][0]][1]+rect_prism_face_normals[norm_inds[2][1]][1]+rect_prism_face_normals[norm_inds[2][0]][1],
							rect_prism_face_normals[norm_inds[2][0]][2]+rect_prism_face_normals[norm_inds[2][1]][2]+rect_prism_face_normals[norm_inds[2][0]][2]};
		magnitude = sqrt(pow(thirdVertNormal[0],2) + pow(thirdVertNormal[1],2) + pow(thirdVertNormal[2],2));
		thirdVertNormal[0] /= magnitude;	thirdVertNormal[1] /= magnitude;	thirdVertNormal[2] /= magnitude;

		// The fourth vertex uses the appropriate index values and computes the corresponding normal
		float fourthVertNormal[] = {rect_prism_face_normals[norm_inds[3][0]][0]+rect_prism_face_normals[norm_inds[3][1]][0]+rect_prism_face_normals[norm_inds[3][2]][0],
							rect_prism_face_normals[norm_inds[3][0]][1]+rect_prism_face_normals[norm_inds[3][1]][1]+rect_prism_face_normals[norm_inds[3][2]][1],
							rect_prism_face_normals[norm_inds[3][0]][2]+rect_prism_face_normals[norm_inds[3][1]][2]+rect_prism_face_normals[norm_inds[3][2]][2]};
		magnitude = sqrt(pow(fourthVertNormal[0],2) + pow(fourthVertNormal[1],2) + pow(fourthVertNormal[2],2));
		fourthVertNormal[0] /= magnitude;	fourthVertNormal[1] /= magnitude;	fourthVertNormal[2] /= magnitude;


		// Draw our vertices through referencing the array of vertex coordinates and assign their previously calculated normals
		if (drawingTable) {
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, myTex[0]);
			glBegin(GL_POLYGON);
				glNormal3fv(firstVertNormal);
				glTexCoord2f(0.0, 0.0);
				glVertex3fv(objectVertices[firstVert]);

				glNormal3fv(secondVertNormal);
				glTexCoord2f(0.0, 1.0);
				glVertex3fv(objectVertices[secondVert]);

				glNormal3fv(thirdVertNormal);
				glTexCoord2f(1.0, 1.0);	
				glVertex3fv(objectVertices[thirdVert]);

				glNormal3fv(fourthVertNormal);
				glTexCoord2f(1.0, 0.0);
				glVertex3fv(objectVertices[fourthVert]);
			glEnd();
			glDisable( GL_TEXTURE_2D );
		} else if (drawingRoom) {
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, myTex[1] );
			glBegin(GL_POLYGON);
				glNormal3fv(firstVertNormal);
				glTexCoord2f(0.0, 0.0);
				glVertex3fv(objectVertices[firstVert]);

				glNormal3fv(secondVertNormal);
				glTexCoord2f(0.0, 1.0);
				glVertex3fv(objectVertices[secondVert]);

				glNormal3fv(thirdVertNormal);
				glTexCoord2f(1.0, 1.0);	
				glVertex3fv(objectVertices[thirdVert]);

				glNormal3fv(fourthVertNormal);
				glTexCoord2f(1.0, 0.0);
				glVertex3fv(objectVertices[fourthVert]);
			glEnd();
			glDisable(GL_TEXTURE_2D );
		} else {
			glBegin(GL_POLYGON);
				glNormal3fv(firstVertNormal);
				glVertex3fv(objectVertices[firstVert]);

				glNormal3fv(secondVertNormal);
				glVertex3fv(objectVertices[secondVert]);

				glNormal3fv(thirdVertNormal);
				glVertex3fv(objectVertices[thirdVert]);

				glNormal3fv(fourthVertNormal);
				glVertex3fv(objectVertices[fourthVert]);
			glEnd();
		}
	}
}


/* Used to draw a groupof 4 rectangular prisms (i.e. the 4 table legs) */
void drawRectPrismQuadGroup(float groupVertices[32][3], float groupFaces[24][4]) {
	// Iterate through the group, drawing faces as we go
	for (int i = 0 ; i < 24; i++) {
		// Find the face we need to draw
		int firstVert = groupFaces[i][0]; 
		int secondVert = groupFaces[i][1];
		int thirdVert = groupFaces[i][2];
		int fourthVert = groupFaces[i][3];
		
		if (drawingTable) {
			// Enable and bind the appropriate texture for the table
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, myTex[0]);
			
			// Draw the corresponding face
			glBegin(GL_POLYGON);
				glTexCoord2f(0.0, 0.0);
				glVertex3fv(groupVertices[firstVert]);
				glVertex3fv(groupVertices[secondVert]);
				glTexCoord2f(1.0, 1.0);
				glVertex3fv(groupVertices[thirdVert]);				//**********************************************
				glTexCoord2f(1.0, 0.0);
				glVertex3fv(groupVertices[fourthVert]);
			glEnd();
			glDisable( GL_TEXTURE_2D );
		}
		else{
			glVertex3fv(groupVertices[firstVert]);
			glVertex3fv(groupVertices[secondVert]);
			glVertex3fv(groupVertices[thirdVert]);
			glVertex3fv(groupVertices[fourthVert]);	
		}
	}
}


/* Used to draw players inside the room */
void drawPlayer(int playerNum) {
	// Use matrix hierarchies to appropriately construct LCS for the players
	glPushMatrix();
		glScalef(playerScale, playerScale, playerScale);	// Scale the entire player system by a defined constant

		glPushMatrix();
			// Move to the corresponding side of the table based on which player we are drawing 
			if (playerNum == 1) {
				glTranslatef(0, 0, -1 * (TABLE_DEPTH / 2) - PLAYER_TABLE_DIST);	// Player 1 is at the -z end of the table
			} else if (playerNum == 2) {
				glTranslatef(0, 0, (TABLE_DEPTH / 2) + PLAYER_TABLE_DIST);		// Player 2 is at the +z end of the table
			}

			glPushMatrix();
				switchMaterial(obsidian.id);

				/* Note that for certain body members it does not matter which player we are drawing, as legs and arms are symmetrical about y axis */
				// Draw first leg
				glPushMatrix();
					// We draw a cube, scale it to be more of a leg-shape, then translate
					glTranslatef(-75, 250, 0);
					glScalef(1, 5, 1);				// Legs are 500 units tall, 100 units wide, 100 units deep
					glutSolidCube(100);
				glPopMatrix();

				// Draw second leg
				glPushMatrix();
					// We draw a cube, scale it to be more of a leg-shape, then translate
					glTranslatef(75, 250, 0);
					glScalef(1, 5, 1);				// Legs are 500 units tall, 100 units wide, 100 units deep
					glutSolidCube(100);
				glPopMatrix();

				// Draw body
				glPushMatrix();
					// First player will have a red shirt, second player will have a green shirt
					if (playerNum == 1) {
						switchMaterial(redrubber.id);
					} else if (playerNum == 2) {
						switchMaterial(greenrubber.id);
					}

					// We draw a cube, scale it to be more of a bodyshape, then translate
					glTranslatef(0, 700, 0);
					glScalef(2.5, 4, 1);			// Body is 400 units tall, 250 units wide, 100 units deep
					glutSolidCube(100);
				glPopMatrix();

				glPushMatrix();
					// Draw left shoulder
					glPushMatrix();
						// We draw a cube, scale it to be more of a shoulder shape, then translate
						glTranslatef(165, 850, 0);
						glScalef(0.8, 1, 1);

						if((cupsRemaining('f') < cupsRemaining('s') && playerNum == 2) || (cupsRemaining('s') < cupsRemaining('f') && playerNum==1)){
							glRotatef(rotateAmt,1,0,0);
						}
						// Shoulders are 100 units tall, 80 units wide, 100 units deep
						glutSolidCube(100);

						switchMaterial(pearl.id);

						// Draw left arm
						glPushMatrix();
							// We draw a cube, scale it to be more of an arm-shape, then translate
							glTranslatef(0, -150, 0);
							glScalef(0.8, 2, 1);			// Arms are 200 units tall, 80 units wide, 100 units deep

							glutSolidCube(100);
						glPopMatrix();
					glPopMatrix();

					// Draw right shoulder
					glPushMatrix();

						// We draw a cube, scale it to be more of a shoulder shape, then translate
						if (playerNum == 1) {
							switchMaterial(redrubber.id);
						} else if (playerNum == 2) {
							switchMaterial(greenrubber.id);
						}
						glTranslatef(-165, 850, 0);
						glScalef(0.8, 1, 1);			// Shoulders are 100 units tall, 80 units wide, 100 units deep

						if ((cupsRemaining('f') < cupsRemaining('s') && playerNum == 2) || (cupsRemaining('s') < cupsRemaining('f') && playerNum==1)) {
							glRotatef(-1*rotateAmt,1,0,0);
						}
						glutSolidCube(100);

						switchMaterial(pearl.id);

						// Draw right arm
						glPushMatrix();
							// We draw a cube, scale it to be more of an arm-shape, then translate
							glTranslatef(0, -150, 0);
							glScalef(0.8, 2, 1);			// Arms are 200 units tall, 80 units wide, 100 units deep

							glutSolidCube(100);
						glPopMatrix();

					glPopMatrix();
				
					if ((cupsRemaining('f') < cupsRemaining('s') && playerNum == 2) || (cupsRemaining('s') < cupsRemaining('f') && playerNum==1)) {
						rotations++;
						if (rotateAmt < 360){	
							rotateAmt += 5;
						} else {
							rotateAmt = 0;
						}
					} else {
						rotations = 0;
					}
				

				glPopMatrix();

				// Draw head
				glPushMatrix();
					// Create a LCS for the members of the head
					glTranslatef(0, 975, 0);
					if((cupsRemaining('f') < cupsRemaining('s') && playerNum == 2) || (cupsRemaining('s') < cupsRemaining('f') && playerNum==1))
						glRotatef(rotateAmt,0,1,0);		// Spin the head around the y-axis
					else if((cupsRemaining('f') < cupsRemaining('s') && playerNum == 2) || (cupsRemaining('s') < cupsRemaining('f') && playerNum==1))
						glRotatef(-1 * rotateAmt,0,1,0);	// Spin the head around the y-axis the other way

					// Hair
					glPushMatrix();
						switchMaterial(obsidian.id);

						// Translate up slightly to have player's hair positioned properly
						glTranslatef(0,87.5,0);
						glScalef(1.5,0.25,1);		// Hair is 25 units tall, 150 units wide, 100 units deep
						glutSolidCube(100);
					glPopMatrix();

					// Left eye
					glPushMatrix();
						if (playerNum == 1)
							glTranslatef(-35,35,50);
						else
							glTranslatef(35,35,-50);

						// Pupil
						glPushMatrix();
							switchMaterial(obsidian.id);

							if (playerNum == 1) 
								glTranslatef(0,0,10);
							else
								glTranslatef(0,0,-10);

							glutSolidSphere(15,100,100);
						glPopMatrix();

						switchMaterial(pearl.id);

						glutSolidSphere(20,100,100);
					glPopMatrix();

					// Right eye
					glPushMatrix();
						if (playerNum == 1)
							glTranslatef(35,35,50);
						else
							glTranslatef(-35,35,-50);

						// Pupil
						glPushMatrix();
							switchMaterial(obsidian.id);

							if (playerNum == 1) 
								glTranslatef(0,0,10);
							else
								glTranslatef(0,0,-10);

							glutSolidSphere(15,100,100);
						glPopMatrix();

						switchMaterial(pearl.id);

						glutSolidSphere(20,100,100);
					glPopMatrix();

					// Nose
					glPushMatrix();
						switchMaterial(redplastic.id);

						if (playerNum == 1)
							glTranslatef(0, 10, 50);
						else
							glTranslatef(0,10,-50);

						glutSolidSphere(15,100,100);
					glPopMatrix();

					// Mouth
					glPushMatrix();
						if (playerNum == 1)
							glTranslatef(0, -40, 5);
						else
							glTranslatef(0,-40, -5);

						switchMaterial(obsidian.id);

						glScalef(0.86,0.28,1.0);
						glutSolidCube(100);
					glPopMatrix();

					switchMaterial(pearl.id);

					glScalef(1.5, 1.5, 1);			// Head is 150 units tall x 150 units wide, 100 units deep
					glutSolidCube(100);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

/* Used for drawing the game cups at a given table X,Z. Each cup has an inside and an outside, method adjusts radius for this */ 
void drawCups(float x,float z, bool insideOfCup){
	float norm1[3];		// Face normals
    float norm2[3];
    float normV1[3];	// Vertex normals
    float normV2[3];
    
	glPushMatrix();
		glTranslatef(x,TABLE_HEIGHT,z); 	// Position of cup
    	float angle = 0.0; 					// Starting angle
   		float angleInc = 0.1; 				// How much to increase angle every iteration
   		float radius = 0;
    	
    	// If we're drawing the inside of the cup, we decrease the radius so that we can see it
    	if (!insideOfCup)
    		radius = CUP_RADIUS; 
    	else 
    		radius = 0.98 * CUP_RADIUS;

    	float height = CUP_HEIGHT; 
    
    	glBegin(GL_QUAD_STRIP); //created the cylinder using quad strips
    		angle = 0.0;

    		//as long as the angle is less than a full circle (2*pi) plus a bit extra to make sure full circle
        	while( angle < 2* PI +0.3) { 
	        	x = radius * cos(angle); //the x value
	            z = radius * sin(angle); //the y value

	            //calculate the first face
	            norm1[0] = x-0; 
				norm1[1]=height-0;
				norm1[2] = z-0;

				//normalize
				float length = sqrt(norm1[0]*norm1[0]+norm1[1]*norm1[1]+norm1[2]*norm1[2]);
				norm1[0] = norm1[0] / length;
				norm1[1] = norm1[1] / length;
				norm1[2] = norm1[2] / length;
	            
	            //calculate next face
	            norm2[0] = x-0;
				norm2[1]=0-0;
				norm2[2] = z-0;

				//normalize
				length = sqrt(norm2[0]*norm2[0]+norm2[1]*norm2[1]+norm2[2]*norm2[2]);
				norm2[0] = norm2[0] / length;
				norm2[1] = norm2[1] / length;
				norm2[2] = norm2[2] / length;

				//calculate the vertex normal
				normV1[0] = (norm1[0] + norm2[0]);
				normV1[1] = (norm1[1] + norm2[1]);
				normV1[2] = (norm1[2] + norm2[2]);
				
				//normalize
				length = sqrt(normV1[0]*normV1[0]+normV1[1]*normV1[1]+normV1[2]*normV1[2]);
				normV1[0] = (norm1[0] + norm2[0])/length;
				normV1[1] = (norm1[1] + norm2[1])/length;
				normV1[2] = (norm1[2] + norm2[2])/length;
				
				glNormal3f(normV1[0],normV1[1],normV1[2]);
				glVertex3f(x, height , z); 	//draw vertex
	            glVertex3f(x, 0.0 , z); 	//draw vertex

	            angle += angleInc; //increase the angle for the next iteration
	        }

	    glEnd();
    glPopMatrix();
}


/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	Projectile Motion
 
	Functions to calculate the projectile motion aspects of our project
	are listed here. We can draw a trajectory for any player, etc.
 */

/* Checks impact point for a successful in-cup landing */
void checkShot(){
	for(int i=0; i<6; i++){
		if(((endPosition[0]>playerOneCupsX[i]-CUP_RADIUS && endPosition[0]<playerOneCupsX[i]+CUP_RADIUS) && 
			(endPosition[2]>playerOneCupsZ[i]-CUP_RADIUS && endPosition[2]<playerOneCupsZ[i]+CUP_RADIUS))){
			playerOneActiveCups[i] = false;
			score[0] -= 1;
		}
		if(((endPosition[0]>playerTwoCupsX[i]-CUP_RADIUS && endPosition[0]<playerTwoCupsX[i]+CUP_RADIUS) && 
			(endPosition[2]>playerTwoCupsZ[i]-CUP_RADIUS && endPosition[2]<playerTwoCupsZ[i]+CUP_RADIUS))){
			playerTwoActiveCups[i] = false;
			score[1] -= 1;
		}
	}
	
}

/* Draws a given player's trajectory */
void drawTrajectory (char player, float trajectoryVector[3], bool shooting) {
    double V;																	// Velocity
    double theta = trajectoryVector[1];											// Angle of projection (Y-axis)
    double Vy, Vz;																// Velocity components
    double startPosition[3] = {0, TABLE_HEIGHT + THROW_HEIGHT, 0};				// Initial position of the ball
    double h;																	// Max height reached by the ball
    double t;																	// Time to reach max height
    double t1;																	// Freefall time after reaching max height
    double totalTime;															// Time from launch to land
    double displacement[3] = {0, 0, 0};											// Displacement amounts along each axis

    // Drawing player 1's trajectory
    if (player == 'f') {
        startPosition[2] = (-1 * TABLE_DEPTH / 2) + (3 * BALL_RADIUS);
        V = POWER_CONST * firstPlayerPower;
    }
    
    // Drawing player 2's trajectory
    else {
        startPosition[2] = (TABLE_DEPTH / 2) - (3 * BALL_RADIUS);
        V = POWER_CONST * secondPlayerPower;
    }
    
    // Components, ensuring to convert to radians
    Vy = V * sin(theta*PI/180);
    Vz = V * cos(theta*PI/180);
    
    // Vertical motion
    // Vf = Vi - g*t, solve for t when Vf = 0
    t = Vy / GRAVITY;
    h = (Vy * t) - (0.5) * GRAVITY * pow(t,2) + startPosition[1];	// h = Vy*t - 1/2gt^2 + initial height = MAX HEIGHT
    
    // Free fall from max height
    // solve for t1, from h = 1/2 gt1^2
    t1 = sqrt(h / (0.5 * GRAVITY));
    totalTime = t + t1;		// Total time the projectile is in the air, so we can solve for distances now
    
    // Calculate our new position using changes along the axes
    displacement[0] = trajectoryVector[0];		// Offset by a certain amount for X axis to reflect aiming
    displacement[1] = -1 * THROW_HEIGHT;		// We land at table level
    displacement[2] = Vz * totalTime;			// Change in depth
    
    if (player != 'f')
        displacement[2] *= -1;	// Negative for player 2
    
    // Draw approximately where the user is aiming
    glPushMatrix();
   		// Reticule will show up above where the user is aiming to give them an idea about their trajectory
    	glTranslatef(startPosition[0]+displacement[0],TABLE_HEIGHT,startPosition[2]+displacement[2]);	// This is our impact point
    	endPosition[0]=displacement[0];
    	endPosition[1]=TABLE_HEIGHT;
    	endPosition[2]=startPosition[2]+displacement[2];
    
	    // Use a cone shape to signify where they are aiming
	    glPushMatrix();
		    glTranslatef(0, RETICULE_HEIGHT, 0);
		    glRotatef(90, 1, 0, 0);
		    glutSolidCone(25, 50, 100, 100);
	    glPopMatrix();
    glPopMatrix();

    if (shooting) {
    	// Get the midpoint of the table-level travel line and rotate around that point to simulate throwing the ball
    	float midPoint[3] = {(startPosition[0])/ 2, TABLE_HEIGHT, ((startPosition[2]+displacement[2]) + startPosition[2]) / 2};

    	float rotationalRadius = sqrt(pow(midPoint[0]-startPosition[0],2)+pow(midPoint[1]-startPosition[1],2)+pow(midPoint[2]-startPosition[2],2));
    	if (rotationalRadius < 0) {
    		rotationalRadius *= -1;
    	}

    	glPushMatrix();
    		if (player == 'f') {
    			glTranslatef(displacement[0], midPoint[1], midPoint[2]);
    			glRotatef(shootingAngle, 1, 0, 0);
    		} else {
    			glTranslatef(displacement[0], midPoint[1], midPoint[2]);
    			glRotatef(-1 * shootingAngle, 1, 0, 0);
    		}

    		glPushMatrix();
    			glTranslatef(0 , rotationalRadius, 0);

    		 	glutSolidSphere(BALL_RADIUS,BALL_SLICES,BALL_SLICES);
    		glPopMatrix();
    	glPopMatrix();

    	checkShot();

    	if(!paused){
    		camUp[0]=0;
			camUp[1]=1;
			camUp[2]=0;
			switchingPlayer=true;
		}
    }
}


/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	OpenGL Functions

	Defines functions for:
		- Keyboard
		- Special Keys
		- Mouse
		- Passive Mouse Motion
		- Displaying
*/


/* Keyboard Function */
void keyboard(unsigned char key, int xIn, int yIn) {
	// Switch statement for multiple key functionality
	switch (key)
	{
		//Escape can be used to go back to menu
		case 27:
			if(!paused)
				screen=0;
			break;

		// p key used to alter shot power, must be within 1 - 10
		case 'p':
			if(!paused){
				if(currentPlayer=='f'){
					// Decrease current player's shot power
					if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
						if (firstPlayerPower > 1)
							firstPlayerPower -= 1;
					} 
					// Increase current player's shot power
					else {
						if (firstPlayerPower < 10)
							firstPlayerPower += 1;
					}
				}
				else if(currentPlayer=='s'){
					// Decrease current player's shot power
					if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
						if (secondPlayerPower > 1)
							secondPlayerPower -= 1;
					} 
					// Increase current player's shot power
					else {
						if (secondPlayerPower < 10)
							secondPlayerPower += 1;
					}
				}
			}
			break;
			
		// Enable pausing throughout gameplay with capital P key
		case 'P':
			if(screen == 1) {
				if (!paused) {
					// We pause the game, and enable fog for a cool visual effect
					paused = true;
					glEnable(GL_FOG);
					glFogi(GL_FOG_MODE,GL_EXP2);
					glFogfv(GL_FOG_COLOR,fogColor);
					glFogf(GL_FOG_DENSITY,0.0005);
					glHint(GL_FOG_HINT,GL_NICEST);
					glFogf(GL_FOG_START, 1.0f);
					glFogf(GL_FOG_END, 100.0f);
				} else {
					glDisable(GL_FOG);
					paused=false;
				}
			}
			break;		
	}
}

/* Special Keys Function */
void special (int key, int x, int y) {
    
    // Used to alter ball trajectory through modification of the projection vector
    switch (key) {
            // For player one, increase x component
            // For player two, decrease x component
        case GLUT_KEY_LEFT:
       		if (!paused) {
                if (currentPlayer == 'f')
                    firstPlayerShotVec[0] += COMPONENT_INC;
                else {
                	secondPlayerShotVec[0] -= COMPONENT_INC;
                }
	        }
            break;
            
            // For player one, decrease x component
            // For player two, increase x component
        case GLUT_KEY_RIGHT:
        	if (!paused) {
                if (currentPlayer == 'f')
                    firstPlayerShotVec[0] -= COMPONENT_INC;
                else {
                	secondPlayerShotVec[0] += COMPONENT_INC;
                }
	        }
            break;
            
            // For player one and two, increase y component
        case GLUT_KEY_UP:
        	if (!paused) {
                if (currentPlayer == 'f')
                    firstPlayerShotVec[1] += COMPONENT_INC;
                else {
                	secondPlayerShotVec[1] += COMPONENT_INC;
                }
        	}
            break;
            
            // For player one and two, decrease y component
        case GLUT_KEY_DOWN:
        	if (!paused) {
                if (currentPlayer == 'f')
                    firstPlayerShotVec[1] -= COMPONENT_INC;
                else {
                	secondPlayerShotVec[1] -= COMPONENT_INC;
                }	
	            break;
        	}
    }
    glutPostRedisplay();
}

/* Mouse Click function. Manages ray picking and menu interaction. */
void mouse(int btn, int state, int x, int y){
	// Get the x and y click values
	mouseX = x;
	mouseY = WINDOW_SIZE_HEIGHT - y;

	// We only need to attempt ray picking if the game is running. Ignore input if paused.
	if (!paused) {
		switch (btn) {
			// Left click starts the ray picking
			case GLUT_LEFT_BUTTON:
				if (screen == 1) {
					// If we have a positive intersection we start the shooting process
					if (ball.intersect && state == GLUT_DOWN) {
						if (currentPlayer == 'f') {
							playerOneShooting = true;
						} else if (currentPlayer == 's') {
							playerTwoShooting = true;
						}
					}			
				} else {
					// We also use mouse clicks to interact with the menu buttons, play and quit.
					if (onPlay && state == GLUT_DOWN){
						screen=1;
					}
					else if (onQuit && state == GLUT_DOWN){
			    		printf("\nThanks for playing!\n\n**************************\n\n");
						exit(0);
						break;
					}
				}
			}
		}
	glutPostRedisplay();
}

/* Passive Mouse Motion Function CONTENT ADDED FOR RAY PICKING */
void passiveMotion(int x, int y) {
	mouseX = x;
	mouseY = WINDOW_SIZE_HEIGHT - y;
	if(mouseX>=460&&mouseX<=550&&mouseY>=630&&mouseY<=680){
		onPlay=true;
	}
	else{
		onPlay=false;	
	}
	if(mouseX>=460&&mouseX<=550&&mouseY>=230&&mouseY<=280){
		onQuit=true;	
	}
	else{
		onQuit=false;	
	}
}

/* Frame Rate Function */
void FPS(int val) {
	// ~ 60 FPS
	glutPostRedisplay();
	glutTimerFunc(17, FPS, 0);
}

/* To draw text on screen */
void setupForText() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 1000);
    glScalef(1, -1, 1);
    glTranslatef(0, -1000, 0);
    glMatrixMode(GL_MODELVIEW);
} 

/* Resetting after text */
void resetAfterText() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
} 

/* Rendering Bitmap Strings */
void renderBitmapString(float x, float y,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
		if(screen==1) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		else if(screen==0) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		else glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);;
    }
}

/* Calculate ray intersections for ball clicking */
void CalcIntersections(){
	// Construct the ray
	GLdouble R0[3], R1[3], Rd[3];
	GLdouble modelMat[16], projMat[16];
	GLint viewMat[4];

	// Populate necessary matrices from openGL matrix data
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	glGetIntegerv(GL_VIEWPORT, viewMat);

	// Calculate near and far points
	gluUnProject(mouseX, mouseY, 0.0, modelMat, projMat, viewMat, &R0[0], &R0[1], &R0[2]);
	gluUnProject(mouseX, mouseY, 1.0, modelMat, projMat, viewMat, &R1[0], &R1[1], &R1[2]);

	// Calculate our ray from R0 and R1
	Rd[0] = R1[0] - R0[0];
	Rd[1] = R1[1] - R0[1];
	Rd[2] = R1[2] - R0[2];

	// Normalize the ray
	GLdouble m = sqrt(Rd[0]*Rd[0] + Rd[1]*Rd[1] + Rd[2]*Rd[2]);
	Rd[0] /= m;
	Rd[1] /= m;
	Rd[2] /= m;

	// Calculate t using the z direction
	double t = (((double)ball.posZ) - R0[2])/Rd[2];

	// Use the t value to get x,y coordinates
	double pt[3];
	pt[0] = R0[0] + t * Rd[0];
	pt[1] = R0[1] + t * Rd[1];
	pt[2] = ball.posZ;

	// Approximate the ball's position using a cube and detect if the intersection is valid
	if(pt[0] > ball.posX - BALL_RADIUS && pt[0] < ball.posX + BALL_RADIUS &&
		pt[1] > ball.posY - BALL_RADIUS && pt[1] < ball.posY + BALL_RADIUS &&
		pt[2] > ball.posZ - BALL_RADIUS && pt[2] < ball.posZ + BALL_RADIUS){
		ball.intersect = true;
	} else{
		ball.intersect = false;
	}
}

/* Display Function */
void display() {
	
	CalcIntersections(); // Ray picking

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//--------------Menu Screen-------------
	if(screen==0){
		
		//Disable Lighting	
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

	//------------------------------------------------------------------------------------------
		//Draw Text  			
		glColor3d(1, 1, 1);
		setupForText();
		glPushMatrix();
		glLoadIdentity();
		glColor3f(1,0.84,0);
		renderBitmapString(440,250,"BEER PONG");
		
		glColor3d(1,1,1);
		renderBitmapString(475,350,"PLAY");
		renderBitmapString(475,750,"QUIT");

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		if(onPlay==true) glColor3f(0,1,0);
		else glColor3f(0.5,0.5,0.5);
		glRecti(460,320,550,360);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		if(onQuit==true) glColor3f(0,1,0);
		else glColor3f(0.5,0.5,0.5);
		glRecti(460,720,550,760);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3d(1, 1, 1);
		renderBitmapString(405,420,"Control Instructions");
		renderBitmapString(405,430,"--------------");
		renderBitmapString(415,480,"p: Increase Power");
		renderBitmapString(385,530,"alt + p: Decrease Power");
		renderBitmapString(350,580,"Arrow Keys: Modify Trajectory");
		renderBitmapString(395,630,"Click ball: Throw Ball");

		glPopMatrix();
		resetAfterText();
	//--------------------END DRAWING MENU TEXT------------------------------------------
		//Enable Lighting	
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glLightfv(GL_LIGHT0, GL_AMBIENT, amb0); 
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

		gluLookAt(0, 1300, -10, camTarget[0] , camTarget[1] , camTarget[2], camUp[0], camUp[1], camUp[2]);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);		
		
		/* Draw room and table */
		switchMaterial(pearl.id);
		
		drawingRoom=true;
		drawRectPrism(room_verts, room_faces, true);		// Room
		drawingRoom=false;
		switchMaterial(wood.id);
		
		drawingTable=true;
		drawRectPrism(table_verts, table_faces, false);		// Table
		drawRectPrismQuadGroup(leg_verts, leg_faces);	// Legs
		drawingTable=false;
		    
	}
	//--------------Game Screen-------------
	else if(screen==1){
		
		//Disable Lighting	
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

	//------------------------------------------------------------------------------------------
		//Draw Text  			
		glColor3d(0.0, 0.0, 0.0);
		setupForText();
		glPushMatrix();
		glLoadIdentity();

		// Print the remaining cups for each player in the top left corner
		char s[10];
		char r1[100]="Red Player's Cups Remaining: ";	
		char r2[100]="Green Player's Cups Remaining: ";
		int numCups1=cupsRemaining('f');
		int numCups2=cupsRemaining('s');
		sprintf(s,"%d",numCups1);
		strcat(r1,s);
		renderBitmapString(0,20,r1);
		sprintf(s,"%d",numCups2);	
		strcat(r2,s);
		renderBitmapString(0,40, r2);

		// Print the turn indicator
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		glRecti(880,0,1000,30);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(currentPlayer=='f'){	
			glColor3f(1,0,0);					
			renderBitmapString(900,20,"Red's Turn");
		}
		else if(currentPlayer=='s'){
			glColor3f(0,1,0);
			renderBitmapString(880,20,"Green's Turn");
		}

		glColor3d(0.0, 0.0, 0.0);
		renderBitmapString(0,250,"Power");

		char sc [100]="GREEN ";
		sprintf(s,"%d",greenScore);
		strcat(sc,s);
		strcat(sc," : ");
		sprintf(s,"%d",redScore);
		strcat(sc,s);
		strcat(sc," RED");	
		renderBitmapString(415,20,sc);
		
		// We draw the pause text when necessary
		if (paused){
			glColor3f(1,1,1);
			renderBitmapString(450,50,"PAUSED");
			renderBitmapString(400,70,"PRESS 'P' TO RESUME");
		}
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		glRecti(410,0,570,30);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		// Draw power indicator
		if(currentPlayer=='f'){						
			glColor3f(1,0,0);				
			glRecti(10,510,40,510-(firstPlayerPower*25));
		}
		else if(currentPlayer=='s'){
			glColor3f(0,1,0);				
			glRecti(10,510,40,510-(secondPlayerPower*25));
		}

		glPopMatrix();
		resetAfterText();
	//--------------------END DRAWING GUI TEXT------------------------------------------
		//Enable Lighting	
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glLightfv(GL_LIGHT0, GL_AMBIENT, amb0); 
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

		if (cupsRemaining('f') == 0) {
			printf("Green wins!\n");
			resetProgram();
			greenScore++;
		} else if (cupsRemaining('s') == 0) {
			printf("Red wins!\n");
			resetProgram();
			redScore++;
		}

		/*To switch players*/
		//increment camera to certain point and switch player to next and turn switching players to false
        if (switchingPlayer) {
            if (currentPlayer == 'f'){
                if (camPos[2]<((1 * ROOM_DEPTH / 2) - ROOM_DEPTH / 7)) {
                    camPos[2] += 22;
                } else{
                    currentPlayer='s';
                    switchingPlayer=false;
                }
            }
            else if (currentPlayer == 's'){
                if (camPos[2] > ((-1 * ROOM_DEPTH / 2) + ROOM_DEPTH / 7)) {
                    camPos[2] -= 22;
                } else {
                    currentPlayer='f';
                    switchingPlayer=false;
                }
            }
            if (!switchingPlayer) {
                if (currentPlayer == 'f') {
                	playerTwoShooting = false;
                    ball.posX = 0;
                    ball.posY = TABLE_HEIGHT + THROW_HEIGHT;
                    ball.posZ = (-1 * TABLE_DEPTH / 2) + (3 * BALL_RADIUS);
                    ball.intersect = false;
                } else {
                	playerOneShooting = false;
                    ball.posX = 0;
                    ball.posY = TABLE_HEIGHT + THROW_HEIGHT;
                    ball.posZ = (TABLE_DEPTH / 2) - (3 * BALL_RADIUS);
                    ball.intersect = false;
                }
            }
        }
		
		//For sway
		if(redScore >= 1 && currentPlayer == 's' && !switchingPlayer){
			swayGreen = redScore*0.02;
			if (!swayDir) {
					camUp[0] = camUp[0] + swayGreen;
					if(camUp[0] > 0.5){
						swayDir=true;
					}
			}
			else if (swayDir){
					camUp[0] = camUp[0] - swayGreen;
					if (camUp[0] < -0.5){
						swayDir=false;
					}
			}
		}
		if(greenScore >= 1 && currentPlayer == 'f' && !switchingPlayer){
			swayRed = greenScore*0.02;
			if (!swayDir) {
					camUp[0] = camUp[0] + swayRed;
					if (camUp[0] > 0.5){
						swayDir = true;
					}
			}
			else if (swayDir){
					camUp[0] = camUp[0]-swayRed;
					if (camUp[0] < -0.5) {
						swayDir = false;
					}
			}
		}
		
		gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0] , camTarget[1] , camTarget[2], camUp[0], camUp[1], camUp[2]);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

		/* Draw room and table */
		switchMaterial(pearl.id);
		
		drawingRoom = true;
		drawRectPrism(room_verts, room_faces, true);	// Room
		drawingRoom=false;
		
		switchMaterial(wood.id);
		
		drawingTable=true;
		drawRectPrism(table_verts, table_faces, false);	// Table
		drawRectPrismQuadGroup(leg_verts, leg_faces);	// Legs
		drawingTable=false;

		/* Draw cups according to which cups still remain for each player */
		for (int i = 0; i < NUM_CUPS; i++) {
			// Draw outside of cups
			switchMaterial(redplastic.id);

			if (playerOneActiveCups[i])
				drawCups(playerOneCupsX[i],playerOneCupsZ[i], false);
			if (playerTwoActiveCups[i])
				drawCups(playerTwoCupsX[i],playerTwoCupsZ[i], false);

			// Draw inside of cups
			switchMaterial(whiteplastic.id);

			if (playerOneActiveCups[i])
				drawCups(playerOneCupsX[i],playerOneCupsZ[i], true);
			if (playerTwoActiveCups[i])
				drawCups(playerTwoCupsX[i],playerTwoCupsZ[i], true);
		}

        // Draw the ball
        if (!switchingPlayer) {
        	switchMaterial(brass.id);
            
            glPushMatrix();
            glTranslatef(ball.posX,ball.posY,ball.posZ);
            glutSolidSphere(BALL_RADIUS,BALL_SLICES,BALL_SLICES);
            glPopMatrix();
        }
        
        // Draw where the player is aiming
        if (currentPlayer == 'f' && !switchingPlayer) {
            drawTrajectory('f',firstPlayerShotVec, playerOneShooting);

        } else if (currentPlayer == 's' && !switchingPlayer) {
            drawTrajectory('s',secondPlayerShotVec, playerTwoShooting);
        }

		// Draw players
		drawPlayer(1);
		drawPlayer(2);	
	}
	glutSwapBuffers();
}


/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/*
	Callback Functions, Init and Main Method

	Program entry points and initializers are defined here.
*/

/* Initialize Callback Functions */
void callBackInit() {
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passiveMotion);
	glutTimerFunc(0,FPS,0);
	glutDisplayFunc(display);
}

/* Initialize GL Components */
void init() {
	// Set background colour and colour
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);
	
    //Texture setup
    glEnable(GL_TEXTURE_2D);
    glGenTextures(2,myTex);
    
    glBindTexture(GL_TEXTURE_2D, myTex[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    image = LoadPPM("Textures/wood_texture.ppm",&width,&height,&max);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    
    glBindTexture(GL_TEXTURE_2D, myTex[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    image2 = LoadPPM("Textures/wall_texture.ppm",&width,&height,&max);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image2);
    
    glDisable(GL_TEXTURE_2D);
	
	// Set up the projection matrix and viewing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, SCREEN_WIDTH/SCREEN_HEIGHT, 1, 2500);
	
    // Ball Position
    ball.posX = 0;
    ball.posY = TABLE_HEIGHT + THROW_HEIGHT;
    ball.posZ = (-1 * TABLE_DEPTH / 2) + (3 * BALL_RADIUS);
    ball.intersect = false;
	
    // Set encapsulated material values
	obsidian.id = 0; blackrubber.id = 1; 
	brass.id = 2; pearl.id = 3;
	greenrubber.id = 4; redrubber.id = 5; 
	redplastic.id = 6; whiteplastic.id = 7; 
	wood.id = 8;

	obsidian.ambient[0] = 0.05375; obsidian.ambient[1] = 0.05; obsidian.ambient[2] = 0.06625; obsidian.ambient[3] = 1.0;
	obsidian.diffuse[0] = 0.18275; obsidian.diffuse[1] = 0.17; obsidian.diffuse[2] = 0.22525; obsidian.diffuse[3] = 1.0;
	obsidian.specular[0] = 0.332741; obsidian.specular[1] = 0.328634; obsidian.specular[2] = 0.346435; obsidian.specular[3] = 1.0;
	obsidian.shine = 128 * 0.3;

	blackrubber.ambient[0] = 0.02; blackrubber.ambient[1] = 0.02; blackrubber.ambient[2] = 0.02; blackrubber.ambient[3] = 1.0;
	blackrubber.diffuse[0] = 0.01; blackrubber.diffuse[1] = 0.01; blackrubber.diffuse[2] = 0.01; blackrubber.diffuse[3] = 1.0;
	blackrubber.specular[0] = 0.4; blackrubber.specular[1] = 0.4; blackrubber.specular[2] = 0.4; blackrubber.specular[3] = 1.0;
	blackrubber.shine = 128 * 0.078125;

	brass.ambient[0] = 0.33; brass.ambient[1] = 0.22; brass.ambient[2] = 0.03; brass.ambient[3] = 1.0;
	brass.diffuse[0] = 0.78; brass.diffuse[1] = 0.57; brass.diffuse[2] = 0.11; brass.diffuse[3] = 1.0;
	brass.specular[0] = 0.99; brass.specular[1] = 0.91; brass.specular[2] = 0.81; brass.specular[3] = 1.0;
	brass.shine = 128 * 0.21794872;

	pearl.ambient[0] = 0.25; pearl.ambient[1] = 0.20725; pearl.ambient[2] = 0.20725; pearl.ambient[3] = 1.0;
	pearl.diffuse[0] = 1.0; pearl.diffuse[1] = 0.829; pearl.diffuse[2] = 0.829; pearl.diffuse[3] = 1.0;
	pearl.specular[0] = 0.2996648; pearl.specular[1] = 0.2996648; pearl.specular[2] = 0.2996648; pearl.specular[3] = 1.0;
	pearl.shine = 128 * 0.088;

	greenrubber.ambient[0] = 0.0; greenrubber.ambient[1] = 0.05; greenrubber.ambient[2] = 0.0; greenrubber.ambient[3] = 1.0;
	greenrubber.diffuse[0] = 0.4; greenrubber.diffuse[1] = 0.5; greenrubber.diffuse[2] = 0.4; greenrubber.diffuse[3] = 1.0;
	greenrubber.specular[0] = 0.04; greenrubber.specular[1] = 0.7; greenrubber.specular[2] = 0.04; greenrubber.specular[3] = 1.0;
	greenrubber.shine = 128 * 0.078125;

	redrubber.ambient[0] = 0.05; redrubber.ambient[1] = 0.0; redrubber.ambient[2] = 0.0; redrubber.ambient[3] = 1.0;
	redrubber.diffuse[0] = 0.5; redrubber.diffuse[1] = 0.4; redrubber.diffuse[2] = 0.4; redrubber.diffuse[3] = 1.0;
	redrubber.specular[0] = 0.7; redrubber.specular[1] = 0.04; redrubber.specular[2] = 0.04; redrubber.specular[0] = 1.0;
	redrubber.shine = 128 * 0.078125;

	redplastic.ambient[0] = 0.0; redplastic.ambient[1] = 0.0; redplastic.ambient[2] = 0.0; redplastic.ambient[3] = 1.0;
	redplastic.diffuse[0] = 0.5; redplastic.diffuse[1] = 0.0; redplastic.diffuse[2] = 0.0; redplastic.diffuse[3] = 1.0;
	redplastic.specular[0] = 0.7; redplastic.specular[1] = 0.6; redplastic.specular[2] = 0.6; redplastic.specular[3] = 1.0;
	redplastic.shine = 128 * 0.25;

	whiteplastic.ambient[0] = 0.0; whiteplastic.ambient[1] = 0.0; whiteplastic.ambient[2] = 0.0; whiteplastic.ambient[3] = 1.0;
	whiteplastic.diffuse[0] = 0.55; whiteplastic.diffuse[1] = 0.55; whiteplastic.diffuse[2] = 0.55; whiteplastic.diffuse[3] = 1.0;
	whiteplastic.specular[0] = 0.70; whiteplastic.specular[1] = 0.70; whiteplastic.specular[2] = 0.70; whiteplastic.specular[3] = 1.0;
	whiteplastic.shine = 128 * 0.25;

	wood.ambient[0] = 0.25; wood.ambient[1] = 0.148; wood.ambient[2] = 0.06475; wood.ambient[3] = 1.0;
	wood.diffuse[0] = 0.4; wood.diffuse[1] = 0.2368; wood.diffuse[2] = 0.1036; wood.diffuse[3] = 1.0;
	wood.specular[0] = 0.774597; wood.specular[1] = 0.458561; wood.specular[2] = 0.200621; wood.specular[3] = 1.0;
	wood.shine = 76.8;
}

/* Main Method */
int main(int argc, char** argv) {
	// Glut initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Window Initialization
	glutInitWindowSize(WINDOW_SIZE_WIDTH,WINDOW_SIZE_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("3GC3 Beer Pong");

	// Print game instructions
	printInstructions();

	// Initialize functions and start the main loop
	callBackInit();
	glEnable(GL_DEPTH_TEST);	// Enable proper depth testing
	init();
	glutMainLoop();

	return 0;
}

