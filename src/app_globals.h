#include "app_defs.h"

// глобальная палитра
extern pal256 globalPal;
// глобалдьное состояние игры
extern gameState state;
// рекорд
extern unsigned int topScore;

// базовые цвета
extern const col4 colBgGamePlay;
extern const col4 colBgGameOver;
extern const col4 colWall;
extern const col4 colStar;
extern const col4 colProjectile;

#ifdef _COMPILE_GLOBALS_
// глобальная палитра
pal256 globalPal;
// глобалдьное состояние игры
gameState state;
// рекорд
unsigned int topScore = 0;

// базовые цвета
const col4 colBgGamePlay = {10, 10, 10, 255};
const col4 colBgGameOver = {120, 10, 10, 255};
const col4 colWall = {100, 100, 100, 255};
const col4 colStar = {220, 220, 220, 255};
const col4 colProjectile = {0, 255, 0, 255};

#endif
