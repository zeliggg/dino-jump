#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int switchState = 0;
int switchFirstPress = 0;

int buzzerPin = 9;
int jumpButton = 2;
int j;
int score = 0;
unsigned long t;
unsigned long t1;
unsigned long t2;
unsigned long t3;

int objectDelay = 300; // basic cactus moving speed
int objectDecrease = 20; // decreases delay

bool dinoUp = false; // Position state of dino

byte cactus[8] = { // Cactus character
  0b00101,
  0b10101,
  0b10101,
  0b10101,
  0b10110,
  0b01100,
  0b00100,
  0b00100,
};

byte dino[8] = { // Dino character
  0b00000,
  0b00111,
  0b00101,
  0b10111,
  0b11100,
  0b11111,
  0b01101,
  0b01100,
};

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(7, cactus);
  lcd.createChar(6, dino);
  pinMode(buzzerPin, OUTPUT);
  pinMode(jumpButton, INPUT_PULLUP);
  j = 15;
  t = millis();
  t1 = millis();
  t2 = millis();
  t3 = millis();
  Serial.begin(9600);

  // Show the start screen and wait for button press
  switchFirstPress = digitalRead(jumpButton);
  while (switchFirstPress == HIGH) {
    for (int i = 15; i >= 0; i--) {
      lcd.setCursor(i, 1);
      lcd.print("PRESS TO START");
      lcd.setCursor(0, 0);
      lcd.print("DINO GAME 1.0");
      lcd.setCursor(14, 0);
      lcd.write(6); // Display dino on start screen
      delay(400);
      lcd.clear();
      switchFirstPress = digitalRead(jumpButton);
      if (switchFirstPress == LOW) {
        break;
      }
    }
  }
  delay(300);
}

void loop() {
  switchState = digitalRead(jumpButton);

  if ((millis() - t) >= objectDelay) { // Delay on cactus move
    t = millis();
    lcd.clear();
    
    // Display cactus moving position
    lcd.setCursor(j, 1);
    lcd.write(7); 
    j = j - 1;
    if (j == -1) {
      j = 15;
    }
    
    // Display score
    lcd.setCursor(6, 0);
    lcd.print("Score:");
    lcd.print(score);

    // Display dino at the correct row
    if (dinoUp) {
      lcd.setCursor(0, 0); // Dino jumps to top row
      lcd.write(6);
    } else {
      lcd.setCursor(0, 1); // Dino stays at bottom row
      lcd.write(6);
    }
  }

  // Check if the button is pressed for jump
  if (switchState == LOW && dinoUp == false) {
    // Dino jumps
    dinoUp = true;
  } else if (switchState == HIGH && dinoUp == true) {
    // Dino lands immediately when the button is released
    dinoUp = false;
  }

  // Check collision and game over logic
  if (((millis() - t3) >= 1000) && (j == 0)) { // Adjusted to check collision
    t3 = millis();
    if (dinoUp) {
      tone(buzzerPin, 700, 100);
      score++;
      if (score % 5 == 0) {
        objectDelay -= objectDecrease;
        if (objectDelay <= 100) { // Max speed limit
          objectDecrease = 0;
        }
      }
    } else { // Game over
      lcd.clear();
      lcd.setCursor(0, 0);
      tone(buzzerPin, 700, 100);
      lcd.print("   GAME OVER!   ");
      lcd.setCursor(0, 1);
      lcd.print("Push Button");
      score = 0;
      j = 15;
      objectDelay = 300; // Reset speed

      // Wait for button press to restart
      while (digitalRead(jumpButton) == HIGH) {
        // Wait for button press
      }
      delay(500);
    }
  }
}