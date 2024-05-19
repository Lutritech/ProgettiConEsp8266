/* https://www.lutritech.it/come-realizzare-un-micro-pc-con-esp8266/

  Micro Computer con Pong giocabile con Esp8266
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin dei pulsanti
#define UP_BUTTON D3
#define DOWN_BUTTON D4
#define UP_BUTTON_op D6
#define DOWN_BUTTON_op D5


const unsigned long PADDLE_RATE = 33;
const unsigned long BALL_RATE = 16;
const uint8_t PADDLE_HEIGHT = 24;

// Impostazioni del display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
//#define SSD1306_NO_SPLASH in libraries\Adafruit_SSD1306\Adafruit_SSD1306.h

//SSD1306 SDA, SCL pins
#define OLED_RESET -1  // -1 reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void drawCourt();

uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;

unsigned long paddle_update;

const uint8_t player_2X = 12;
uint8_t player_2y = 16;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;

int MAX_SCORE = 8;
int PLAYER2_SCORE = 0;
int PLAYER_SCORE = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  unsigned long start = millis();

  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON_op, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_op, INPUT_PULLUP);
  digitalWrite(UP_BUTTON, 1);
  digitalWrite(DOWN_BUTTON, 1);
  digitalWrite(UP_BUTTON_op, 1);
  digitalWrite(DOWN_BUTTON_op, 1);
  display.clearDisplay();
  drawCourt();

  display.setCursor(58, 4);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print(PLAYER2_SCORE);
  display.print(":");
  display.print(PLAYER_SCORE);

  while (millis() - start < 2000)
    ;


  
  display.display();
  
  ball_update = millis();
  paddle_update = ball_update;
}

void loop() {

  bool update = false;
  unsigned long time = millis();

  static bool up_state = false;
  static bool down_state = false;
  static bool up_state_op = false;
  static bool down_state_op = false;

  up_state_op |= (digitalRead(UP_BUTTON_op) == LOW);
  down_state_op |= (digitalRead(DOWN_BUTTON_op) == LOW);

  //up_state |= (digitalRead(UP_BUTTON) == LOW);
  //down_state |= (digitalRead(DOWN_BUTTON) == LOW);
  up_state = up_state_op;
  down_state = down_state_op;

  if (time > ball_update) {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;
    uint8_t new_2x = ball_x + ball_dir_x;
    uint8_t new_2y = ball_y + ball_dir_y;


    // Check if the ball hit the vertical walls
    if (new_x == 0)  //Player1 prende un punto
    {
      display.clearDisplay();
      drawCourt();
      PLAYER_SCORE++;
      display.setCursor(58, 4);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.print(PLAYER2_SCORE);
      display.print(":");
      display.print(PLAYER_SCORE);
      display.display();
      if (PLAYER_SCORE == MAX_SCORE) {
        finish();
        display.display();
        delay(4000);
        ESP.reset();
      }
    }
    // Check if the ball hit the vertical walls
    if (new_x == 127)  //Player2 prende un punto
    {
      display.clearDisplay();
      drawCourt();
      PLAYER2_SCORE++;
      display.setCursor(58, 4);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.print(PLAYER2_SCORE);
      display.print(":");
      display.print(PLAYER_SCORE);
      display.display();
      if (PLAYER2_SCORE == MAX_SCORE) {
        finish2();
        display.display();
        delay(4000);
        ESP.reset();
      }
    }

    // Check if the ball hit the horizontal walls.
    if (new_y == 0 || new_y == 63) {
      ball_dir_y = -ball_dir_y;

      new_y += ball_dir_y + ball_dir_y;
    }



    // Check if the ball hit the player2 paddle
    if (new_2x == player_2X
        && new_2y >= player_2y
        && new_2y <= player_2y + PADDLE_HEIGHT) {
      ball_dir_x = -ball_dir_x;
      new_2x += ball_dir_x + ball_dir_x;
    }
    if (new_x == 0 || new_x == 127) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if the ball hit the player paddle
    if (new_x == PLAYER_X
        && new_y >= player_y
        && new_y <= player_y + PADDLE_HEIGHT) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    display.drawPixel(ball_x, ball_y, BLACK);
    display.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;


    ball_update += BALL_RATE;

    update = true;
  }



  if (time > paddle_update) {
    paddle_update += PADDLE_RATE;

    // Player2 racchetta
    display.drawFastVLine(player_2X, player_2y, PADDLE_HEIGHT, BLACK);
    if (up_state_op) {
      player_2y -= 1;
    }
    if (down_state_op) {
      player_2y += 1;
    }
    up_state_op = down_state_op = false;
    if (player_2y < 1) player_2y = 1;
    if (player_2y + PADDLE_HEIGHT > 63) player_2y = 63 - PADDLE_HEIGHT;
    display.drawFastVLine(player_2X, player_2y, PADDLE_HEIGHT, WHITE);


    // Player1 racchetta
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
    if (up_state) {
      player_y -= 1;
    }
    if (down_state) {
      player_y += 1;
    }
    up_state = down_state = false;
    if (player_y < 1) player_y = 1;
    if (player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

    update = true;
  }

  if (update) {
    display.display();
  }
}

void drawCourt() {
  display.drawRect(0, 0, 128, 64, WHITE);
}
int finish() {
  ball_dir_x = 0;
  ball_dir_y = 0;

  display.fillRoundRect(0, 0, 128, 64, 2, WHITE);

  display.setCursor(15, 4);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.print("Player1    Vince");
  return 1;
}
int finish2() {
  ball_dir_x = 0;
  ball_dir_y = 0;

  display.fillRoundRect(0, 0, 128, 64, 2, WHITE);

  display.setCursor(15, 4);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.print("Player2    Vince");
  return 2;
}
