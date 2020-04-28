
/*
 * Snake game project on a LOL shield module for
 * Arduino Uno, controlled via Serial input.
 */

#include "Charliplexing.h"
#include "Myfont.h"
#include "Arduino.h"
#include <elapsedMillis.h>
#define MAX_X 14 // Screen width
#define MAX_Y 9 // Screen height

// Screen matrix
uint8_t matrix[MAX_X][MAX_Y] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1}
};

uint8_t food[2] = {0}; // Coordinates of food
int button_dir = 'D'; // User input direction

// Struct containing coordinates and directions of every snake pixel
struct body {
  byte x;
  byte y;
  int dir;
};

body snake[(MAX_Y - 2) * (MAX_X - 2) - 1]; // Static struct array of a snake
int len = 1; // Snake length

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  LedSign::Init(); // LOL shield initializer
  
  // Snake head starts at (1, 1) and moves to the right
  snake[0].x = 1;
  snake[0].y = 1;
  snake[0].dir = 'D';
  matrix[snake[0].x][snake[0].y] = 1; // Sets snake position in matrix
  
  // Sets screen LEDs one by one according to the matrix
  for(uint8_t i = 0; i < MAX_X; i++)
    for(uint8_t j = 0; j < MAX_Y; j++)
      LedSign::Set(i, j, matrix[i][j]);
}

void loop() {
  /*
   * Checks if there's food on screen
   * Program is made so that it sets X coordinate
   * of food to 0 because it can never be 0 by itself
   */
  if(!food[0]) {
    generateFood();
  }
  
  refresh(1000); // 1 second delay

  // If snake head is on food, lenght grows and food is 0
  if(snake[0].x == food[0] && snake[0].y == food[1]) {
    len++;
    food[0] = 0;
  }

  // Snake tail is turned off
  matrix[snake[len - 1].x][snake[len - 1].y] = 0;

  // All snake coordinates are shifted
  for(int i = len - 1; i > 0; i--)
    snake[i] = snake[i - 1];
  
  // Change head coordinates
  moveHead();

  // Check if dead
  if(snake[0].x == 13 || snake[0].x == 0 || snake[0].y == 0 || snake[0].y == 8) gameover();
  for(int i = 1; i < len; i++)
    if(snake[0].x == snake[i].x && snake[0].y == snake[i].y) gameover();
}

/*
 * This function is used instead of delay.
 * It refreshes screen according to the matrix and
 * waits for user input for given amount of time.
 */

void refresh(uint32_t wait) {
  for(uint8_t i = 1; i < MAX_X - 1; i++)
    for(uint8_t j = 1; j < MAX_Y - 1; j++)
      LedSign::Set(i, j, matrix[i][j]);
  
  elapsedMillis counter;
  while(counter < wait) {
    while(Serial.available()) { // If there's input
      char temp = Serial.read(); // Read one byte
      if(temp != '\n') {
        // Doesn't allow for snake to turn 180
        if(temp == 'W' && button_dir == 'S') break;
        if(temp == 'S' && button_dir == 'W') break;
        if(temp == 'A' && button_dir == 'D') break;
        if(temp == 'D' && button_dir == 'A') break;
        // If it's not newline and is acceptable letterh, save it
        if(temp == 'W' || temp == 'S' || temp == 'A' || temp == 'D') button_dir = temp;
      }
      else break;
    }
  }
}

/*
 * Generates food at random coordinates until it
 * finds coordinates that are not used by the snake
 */

void generateFood() {
  do {
    food[0] = random(1, 13);
    food[1] = random(1, 8);
  } while(matrix[food[0]][food[1]]);
  matrix[food[0]][food[1]] = 1;
}

/*
 * Changes coordinates of snake's head according to
 * it's direction and sets a new direction based on
 * user's last input.
 */

void moveHead() {
  switch(snake[0].dir) {
    case 'W':
      snake[0].y--;
      break;
    case 'S':
      snake[0].y++;
      break;
    case 'A':
      snake[0].x--;
      break;
    case 'D':
      snake[0].x++;
      break;
  }
  snake[0].dir = button_dir;
  matrix[snake[0].x][snake[0].y] = 1;
}

/*
 * If the game is over, this function prints out
 * lenght by converting it's digits in a string
 * and it loops forever.
 */

void gameover() {
  unsigned char text[] = "Game over";
  unsigned char score[11] = "Score: ";

  // Converting lenght digit by digit and putting
  // it in score[] array for printing.
  score[9] = len % 10 + '0';
  len /= 10;
  score[8] = len % 10 + '0';
  len /= 10;
  score[7] = len % 10 + '0';
  score[10] = '\0';
  
  while(true) {
    
    // Functions from Myfont.h library
    // They take string size and string itself and
    // print it out as a scrolling text on the screen
    Myfont::Banner(10, score);
    Myfont::Banner(9, text);
  }
}
