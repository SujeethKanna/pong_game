#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

// ---------------- Display Pins (Wokwi Standard) ----------------
#define TFT_DC 2
#define TFT_CS 15
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int SCORE_AREA_WIDTH = 120;
const int SCORE_AREA_HEIGHT = 20;

// Buttons
#define BTN_LEFT 5
#define BTN_RIGHT 17

bool leftButtonPressed = false, rightButtonPressed = false;
unsigned long lastUpdate = 0;
int score = 0;
const unsigned long interval = 30; // update every 50ms //20fps

class Ball
{
    public:
    float X, Y, speedX, speedY;
    int color, size;

    Ball(float x=0, float y=SCREEN_HEIGHT/2, float s_x=2, float s_y=0, int sz=6, int c = ILI9341_CYAN)
    {
        X = x;
        Y = y;
        speedX = s_x;
        speedY = s_y;
        size = sz;
        color = c;
    }

    void erase()
    {
        tft.fillCircle(X, Y, size, ILI9341_BLACK);
    }

    void draw()
    {
       tft.fillCircle(X, Y, size, color);
    }

    void update()
    {
        X += speedX;
        Y += speedY;
    }
};

class Paddle
{
    public:
    float speed, X, width, Y, thickness;
    Paddle(float p = SCREEN_WIDTH/2, float w = 80)
    {
        speed = 10;
        X = p;
        width = w;
        thickness = 10;
        Y = SCREEN_HEIGHT - thickness;
    }
    void draw()
    {
        tft.fillRect(X, Y, width, thickness, ILI9341_GREEN);
    }
    void erase()
    {
        tft.fillRect(X, Y, width, thickness, ILI9341_BLACK);
    }
    void update() 
    {
        if (leftButtonPressed && X > 0) {
            X -= speed;
        }
        if (rightButtonPressed && X < SCREEN_WIDTH - width) {
            X += speed;
        }
    }
};

class PongGame
{
    public:
    Ball ball;
    Paddle paddle;
    float ball_speed, paddle_speed;

    PongGame(float b_v=5, float p_v=5): 
            ball_speed(b_v),
            paddle_speed(p_v),
            ball(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, b_v, b_v, 7),
            paddle()
    {
        
    }

    void reset()
    {
        ball.erase();
        ball.X = SCREEN_WIDTH/2;
        ball.Y = SCREEN_HEIGHT/2;
        ball.speedX = -ball.speedX;
        ball.speedY = -ball.speedY;
        score = 0;
        tft.setCursor(0,0);
        tft.fillRect(0,0,SCORE_AREA_WIDTH,SCORE_AREA_HEIGHT,ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.print("SCORE: ");
        tft.print(score);
        ball.update();
    }

    void update()
    {
        ball.erase();
        paddle.erase();

        ball.update();
        paddle.update();

        // Wall collisions
        if (ball.X <= ball.size || ball.X >= SCREEN_WIDTH - ball.size) ball.speedX = -ball.speedX;

        if (ball.Y - SCORE_AREA_HEIGHT <= ball.size) ball.speedY = -ball.speedY;

        //Paddle Collision
        if (ball.Y + ball.size >= paddle.Y && ball.Y - ball.size <= paddle.thickness + paddle.Y && 
            ball.X + ball.size >= paddle.X && ball.X - ball.size <= paddle.X + paddle.width) 

        {
            float prev_y = (ball.Y - ball.speedY) + ball.size; 
            /*Using the previous coordinates of the ball,
              checking whether the ball is going to hit the top
              or the sides of the paddle and 
              forcing the coordinates out of the paddle to avoid velcro effect.
            */  
            if(prev_y <= paddle.Y){
                ball.speedY = -ball.speedY;
                ball.Y = paddle.Y - ball.size - 1; 
            }
            
            else {
                float prev_x = (ball.X - ball.speedX); 
                
                if (prev_x + ball.size <= paddle.X) {
                    ball.speedX = -ball.speedX;      
                    ball.X = paddle.X - ball.size - 1; 
                } 
                else {
                    ball.speedX = -ball.speedX;          
                    ball.X = paddle.X + paddle.width + 1;  
                }

            }

            score += 5;
            tft.setCursor(0,0);
            tft.fillRect(0,0,SCORE_AREA_WIDTH,SCORE_AREA_HEIGHT, ILI9341_BLACK);
            tft.setTextColor(ILI9341_WHITE);
            tft.setTextSize(2);
            tft.print("SCORE: ");
            tft.print(score);
        }

        //Paddle missed
        if (ball.Y > SCREEN_HEIGHT)
            reset();
        
        ball.draw();
        paddle.draw();
    }
};

PongGame pong(7,5);

void setup()
{
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    tft.setCursor(0,0);
    tft.fillRect(0,0,SCORE_AREA_WIDTH,SCORE_AREA_HEIGHT, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.print("SCORE: ");
    tft.print(score);
}

void loop()
{
    if (!digitalRead(BTN_LEFT))
        leftButtonPressed = true;
    if (!digitalRead(BTN_RIGHT))
        rightButtonPressed = true;
    
    unsigned long now = millis();

    if (now - lastUpdate >= interval)
    {
        pong.update();
        leftButtonPressed = false;
        rightButtonPressed = false;
        lastUpdate = now;
    }
}
