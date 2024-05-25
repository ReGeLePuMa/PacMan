#include <SPI.h>
#include <SD.h>
#include <TFT.h>

#define PIN_SD_CS 4
#define PIN_TFT_CS 10
#define PIN_DC 9
#define PIN_RST 8
#define PIN_SPEAKER 7
#define PIN_RED 5
#define PIN_BLUE 3
#define PIN_GREEN 6

#define WIDTH 128
#define HEIGHT 60
#define PACMAN 'C'
#define FOOD '*'
#define DEMON '#'

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

#define NR_FOOD 7
#define NR_ENEMIES 4
#define TFT_BLACK 0x0000

struct PacmanObject {
  int16_t x;
  int16_t y;
  bool draw;
  String symbol;
};


TFT TFTscreen = TFT(PIN_TFT_CS, PIN_DC, PIN_RST);
PImage logo;
bool imageError = false;
bool gameStarted = false;
bool gameOver = false;
bool ateAll = false;
PacmanObject food[NR_FOOD];
PacmanObject enemies[NR_ENEMIES];
PacmanObject pacMan;
int8_t score = 0;


void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  TFTscreen.initR(INITR_BLACKTAB);
  TFTscreen.setRotation(1);

  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.background(0, 0, 0);

  initSD();
  logo = TFTscreen.loadImage("/banner.bmp");
  if (!logo.isValid()) {
    Serial.println(F("ERROR"));
    imageError = true;
  }
  srand(analogRead(A1));
  intro();
}

void initBoard() {
  int8_t i;
  pacMan = { WIDTH / 2, HEIGHT / 2, true, "C" };
  for (i = 0; i < NR_FOOD; i++) {
    food[i] = {
      rand() % WIDTH,
      rand() % HEIGHT,
      true,
      "*"
    };
  }
  for (i = 0; i < NR_ENEMIES; i++) {
    enemies[i] = {
      rand() % WIDTH,
      rand() % HEIGHT,
      true,
      "#"
    };
  }
}

void initSD() {
  Serial.print(F("SD card init..."));
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println(F("ERROR"));
    return;
  }
  Serial.println(F("SUCCESS"));
}

void playFoodSound() {
  int wakkaNotes[] = { NOTE_A4, NOTE_G4 };
  int noteDurations[] = { 150, 150 };
  for (int i = 0; i < 2; i++) {
    tone(PIN_SPEAKER, wakkaNotes[i], noteDurations[i]);
    delay(noteDurations[i]);
    noTone(PIN_SPEAKER);
  }
}

void playFailedSong() {
  int melody[] = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4,
    NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4
  };

  int noteDurations[] = {
    500, 500, 500, 500, 500, 500, 1000,
    500, 500, 500, 500, 500, 500, 1000
  };

  for (int i = 0; i < 14; i++) {
    tone(PIN_SPEAKER, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.30);
    noTone(PIN_SPEAKER);
  }
}

void playPacmanIntro() {
  int tempo = 105;
  int buzzer = PIN_SPEAKER;
  int melody[] = {
    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16,
    NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
    NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,
    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_B5, 32,
    NOTE_FS5, -16, NOTE_DS5, 8, NOTE_DS5, 32, NOTE_E5, 32, NOTE_F5, 32,
    NOTE_F5, 32, NOTE_FS5, 32, NOTE_G5, 32, NOTE_G5, 32, NOTE_GS5, 32, NOTE_A5, 16, NOTE_B5, 8
  };
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    tone(buzzer, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(buzzer);
  }
}

void draw() {
  TFTscreen.fillScreen(TFT_BLACK);
  char scoreSir[11];
  int8_t i;
  sprintf(scoreSir, "Scor: %d\n", score);
  TFTscreen.textSize(1);
  TFTscreen.text(pacMan.symbol.c_str(), pacMan.x, pacMan.y);
  for (i = 0; i < NR_FOOD; i++) {
    if (food[i].draw) {
      TFTscreen.text(food[i].symbol.c_str(), food[i].x, food[i].y);
    }
  }
  for (i = 0; i < NR_ENEMIES; i++) {
    if (enemies[i].draw) {
      TFTscreen.text(enemies[i].symbol.c_str(), enemies[i].x, enemies[i].y);
    }
  }
  TFTscreen.textSize(2);
  char boundaryLine[50];
  for (i = 0; i < 50; i++) {
    boundaryLine[i] = '_';
  }
  TFTscreen.text(boundaryLine, 0, 80);
  TFTscreen.text(scoreSir, 0, 100);
}

void move(int16_t dx, int16_t dy) {
  if ((pacMan.x + dx < 0 || pacMan.x + dx > WIDTH) || (pacMan.y + dy < 0 || pacMan.y + dy > HEIGHT)) {
    return;
  }
  pacMan.x += dx;
  pacMan.y += dy;
  int8_t i;
  bool ok = false;
  for (i = 0; i < NR_FOOD; i++) {
    if (food[i].draw) {
      ok = true;
    }
    if (checkCollision(pacMan, food[i]) && food[i].draw) {
      score++;
      food[i].draw = false;
      playFoodSound();
    }
  }
  if (!ok) {
    ateAll = true;
    gameOver = true;
    return;
  }
  for (i = 0; i < NR_ENEMIES; i++) {
    if (checkCollision(pacMan, enemies[i])) {
      gameOver = true;
      ateAll = false;
      return;
    }
  }
  for (i = 0; i < NR_ENEMIES; i++) {
    int8_t j, k;
    int16_t currDistance = manhattanDistance(pacMan, enemies[i]);
    PacmanObject aux;
    for (j = -1; j <= 1; j++) {
      bool ok = true;
      for (k = -1; k <= 1; k++) {
        if (abs(j) != abs(k)) {
          aux.x = enemies[i].x + j;
          aux.y = enemies[i].y + k;
          int16_t distance = manhattanDistance(pacMan, aux);
          if (distance < currDistance) {
            enemies[i].x = aux.x;
            enemies[i].y = aux.y;
            ok = false;
            break;
          }
        }
      }
      if (!ok) {
        break;
      }
    }
  }
}

bool checkCollision(PacmanObject obj1, PacmanObject obj2) {
  return (checkRange(obj1.x, obj2.x - 2, obj2.x + 2) && checkRange(obj1.y, obj2.y - 2, obj2.y + 2) && checkRange(obj2.x, obj1.x - 2, obj1.x + 2) && checkRange(obj2.y, obj1.y - 2, obj1.y + 2));
}

int16_t manhattanDistance(PacmanObject obj1, PacmanObject obj2) {
  return (abs(obj2.x - obj1.x) + abs(obj2.y - obj1.y));
}

bool checkRange(int val, int low, int high) {
  return (val >= low && val <= high);
}

void intro() {
  if (!imageError) {
    TFTscreen.image(logo, 20, 35);
  } else {
    TFTscreen.text("Va saluta", 40, 64);
    TFTscreen.text("Andrei", 64, 90);
  }
  analogWrite(PIN_GREEN, 64);
  playPacmanIntro();
}

void loop() {
  int value = analogRead(A0);
  if (!gameStarted) {
    if (checkRange(value, 260, 330)) {
      gameStarted = true;
      gameOver = false;
      ateAll = false;
      score = 0;
      initBoard();
      analogWrite(PIN_RED, 0);
      analogWrite(PIN_BLUE, 0);
      analogWrite(PIN_GREEN, 64);
    }
  } else {
    if (!gameOver) {
      draw();
      if (checkRange(value, 120, 170)) {
        move(0, -1);
      } else if (checkRange(value, 190, 230)) {
        move(-1, 0);
      } else if (checkRange(value, 420, 500)) {
        move(1, 0);
      } else if (checkRange(value, 900, 1024)) {
        move(0, 1);
      }
    } else {
      TFTscreen.fillScreen(TFT_BLACK);
      char scoreSir[11];
      sprintf(scoreSir, "Scor: %d\n", score);
      if (ateAll) {
        TFTscreen.text("Ati castigat! :)", 15, 50);
        TFTscreen.text(scoreSir, 40, 70);
        analogWrite(PIN_GREEN, 0);
        analogWrite(PIN_BLUE, 64);
        playPacmanIntro();
      } else {
        TFTscreen.text("Ati pierdut! :(", 15, 50);
        TFTscreen.text(scoreSir, 40, 70);
        analogWrite(PIN_GREEN, 0);
        analogWrite(PIN_RED, 64);
        playFailedSong();
      }
      gameStarted = false;
    }
  }
  delay(100);
}
