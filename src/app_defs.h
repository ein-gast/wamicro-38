#pragma once

typedef unsigned char byte;

// типы объектов
const byte OTYPE_NONE = 0, //
    OTYPE_ERING = 1,       // противник №1
    // OTYPE_EBOX = 2,        // противник №2
    // OTYPE_PWPN = 101,      // буст оружия
    // OTYPE_PSHLD = 102,     // буст защиты
    OTYPE_BNORM = 201 // снаряд нормальный
    // OTYPE_BPOWER = 202    // снаряд усиленный
    ;

// размер растра
#define PIXSZ (16)
#define PIXSZ2 (PIXSZ / 2)
// размер экрана
#define CANVASZS (150)
// сколько байт в пикселе пиксбуфера
#define BPP (4)

// темп разлёта осколков
#define EXPL_DIVISOR (50)
#define EXPL_MULTIPLIER (11)
#define EXPL_NSTEPS (8)

// начисление очков за события
#define SCORE_DISTANCE 1
#define SCORE_ENEMY 50

//
typedef struct {
  int type;
  int x, y;
  int zoom;
  // int light_val;
} objState;

typedef struct {
  union {
    byte v[3];
    struct {
      byte r, g, b;
    };
  };
} col3;

typedef struct {
  union {
    byte v[4];
    struct {
      byte r, g, b, a;
    };
  };
} col4;

typedef col4 pal256[256];
//typedef byte pal256[256][4];

// стандартная полу-строка растра 
typedef byte pixMapLine[PIXSZ2 + 1];

#ifdef WITH_LIGHT
typedef struct {
  int x, y;
  int val;
  byte r, g, b;
} lightState;
#endif

/*
               mXRigth
  xLeft   mxLeft  |    xRight
      V      V    V     V
  ----|      |----|     |----

mXRigth = mxLeft = -1
  xLeft              xRight
      V               V
  ----|               |----
*/
typedef struct {
  int y;
  int yDist;
  int xLeft;
  int xRight;
  int mXLeft;
  int mXRight;
  int index;
} walSect;

// размеры пулов объектов
#define PROJCNT (20)         // "выстрелы"
#define LITCNT (PROJCNT * 2) // источники света
#define WALCNT (20)          // секции стен
#define OBJCNT (WALCNT * 20) // противники и поверапы

// макросы-хелперы
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// состояние игры
typedef struct {
  // viewport size and pos
  int vpY /*, vpS*/;
  // player
  int plY, plX, plXDir;

  // gamover timer
  int timer;
  // score
  int score;
  
  // game over flag
  bool gameover;

  // scene objects
  objState obj[OBJCNT];
  objState projctl[PROJCNT];
} gameState;
