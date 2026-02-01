#include "app_defs.h"
#include "app_levels.h"
#ifdef WITH_LIGHT
#include "app_light.h"
#endif
// #include "app_gfx.h
#include "app_pixdata.h"
#include "app_statelogic.h"
#define _COMPILE_GLOBALS_
#include "app_globals.h"
#define _COMPILE_COLLISION_
#include "app_colision.h"
#define _COMPILE_WALLS_
#include "app_walls.h"
#define _COMPILE_RND_
#include "app_rnd.h"

#define init(frDelay) I(frDelay)
#define keyInput() K()
#define process() P()
#define renderFrame(input) R(input)

void putBitmap(byte *canvas, int toX, int toY, int zoom,
               const byte pix[PIXSZ2][PIXSZ2 + 1]);
void putProj(byte *canvas, int toX, int toY);
void putWall(byte *canvas, const walSect *wal);
void putFontNumber(byte *canvas, int toX, int toY, int number);

// будет помещено в таблицу ипорта из-за -Wl,--inport-undefiled
// /* extern */ void extLog(int ptr);

const char js[] = {
#ifdef APP_MIN_JS
#embed "boot.min.js"
#else
#embed "boot.js"
#endif
    , '\0' // null terminator
};

#ifdef WITH_LIGHT
byte sum255(byte a, byte b) {
  int sum = a + b;
  return sum > 255 ? 255 : sum;
}
#endif

void drawPixel4(byte *canvas, int toX, int toY, const col4 *col) {
  if (col->v[3] != 0 && toX >= 0 && toY >= 0 && toX < CANVASZS &&
      toY < CANVASZS) {
    int ofs = toX + toY * CANVASZS;
    ofs *= BPP;
    canvas[ofs + 0] = col->v[0];
    canvas[ofs + 1] = col->v[1];
    canvas[ofs + 2] = col->v[2];
    canvas[ofs + 3] = 255;
  }
}

/* I() = init() */
void I(int frDelay) {
  for (int i = 0; i < OBJCNT; i++) {
    state.obj[i].type = OTYPE_NONE;
  }
  for (int i = 0; i < PROJCNT; i++) {
    state.projctl[i].type = OTYPE_NONE;
  }

  state.frameDelay = frDelay;
  state.baseSpeed = (state.frameDelay / 16);
  state.plXDir = +1;
  // state.plXDir = 0;
  state.vpY = CANVASZS;
  state.plX = CANVASZS / 2;
  state.plY = state.vpY - CANVASZS + PIXSZ * 2 / 3;
  state.gameover = false;
  state.score = 0;
  state.timer = -1;
  //
  unpakPal(pixFont3x5);
  // unpakPal(pixVFont3x5);
  unpakPal(pixShp16x16);
  // unpakPal(pixRing16x16);
  unpakPal(pixOppo16x16);
  initTexture();
  level0(state.obj, CANVASZS);
}

/* K() = keyInput() */
void K(/*int code1, int code2*/) {
  if (state.gameover && state.timer == 0) {
    init(state.frameDelay);
  }
  state.plXDir = -state.plXDir;
}

/* P() = process() */
void P() {
  static unsigned int procFrame = 0;
  colReset();

  // timer
  if (state.gameover && state.timer > 0) {
    state.timer--;
  }

  // viewport
  state.vpY += state.baseSpeed;

  // walls
  const walSect *ws;
  int lastY = 0;
  ws = walFirst();
  while (ws != nullptr) {
    lastY = ws->y;
    if (ws->y + ws->yDist < state.vpY - CANVASZS) {
      walTailForward();
      ws = walFirst();
      continue;
    }
    colWalAdd(ws, 1);
    ws = walNext(ws);
  }
  if (lastY < state.vpY) {
    // grow up
    createSection(state.obj, CANVASZS);
  }

  // player
  if (!state.gameover) {
    if (state.vpY % CANVASZS == 0) {
      state.score += SCORE_DISTANCE;
    }
    switch (state.plXDir) {
    case +1:
      if (state.plX + state.baseSpeed <= CANVASZS - PIXSZ2) {
        state.plX += state.baseSpeed;
      }
      break;
    case -1:
      if (state.plX - state.baseSpeed >= PIXSZ2) {
        state.plX -= state.baseSpeed;
      }
      break;
    }
    state.plY += state.baseSpeed;

    o_player();

    if (procFrame % (20 * 16 / state.frameDelay) == 0) {
      objState *s;
      if (nullptr != (s = placePtr(state.projctl, PROJCNT))) {
        s->type = OTYPE_BNORM;
        s->x = state.plX;
        s->y = state.plY;
      }
    }
  }

  // score
  if (state.score > topScore) {
    topScore = state.score;
  }

  // objects
  for (int i = 0; i < OBJCNT; i++) {
    if (state.obj[i].type == OTYPE_ERING) {
      o_ering(state.obj + i);
      colObjAdd(state.obj + i, i);
    }
    /*
    switch (state.obj[i].type) {
    case OTYPE_ERING:
      o_ering(i);
      colObjAdd(state.obj + i, i);
      break;
      // case OTYPE_EBOX:
      //   o_ebox(i);
      //   colWalAdd(ws, 1);
      //   break;
    }
    */
  }

  // projectiles
  for (int i = 0; i < PROJCNT; i++) {
    if (state.projctl[i].type == OTYPE_BNORM) {
      o_prjctl_norm(state.projctl + i);
    }
    /*
    switch (state.projctl[i].type) {
    case OTYPE_BNORM:
      o_prjctl_norm(i);
      break;
      // case OTYPE_BPOWER:
      //   o_prjctl_pwr(i);
      //   break;
    }
    */
  }

  procFrame++;
  // extLog(procFrame);
}

/* R(input) = renderFrame(input) */
void R(byte *input) {
  static unsigned int frame = 0;

#ifdef WITH_LIGHT
  // setup lights
  litReset();
  for (int i = 0; i < PROJCNT; i++) {
    switch (state.projctl[i].type) {
    case OTYPE_BNORM:
      litAdd(state.projctl[i].x - PIXSZ2 + 2, state.projctl[i].y);
      litAdd(state.projctl[i].x + PIXSZ2 - 2, state.projctl[i].y);
      break;
    }
  }
#endif

  int i; //, cnt = state.vpS * state.vpS;
  const col4 *clearColor;

  // clear - optimize?
  for (int y = 0; y < CANVASZS; y++) {
    i = ((CANVASZS + state.plY / 2 - y) % CANVASZS) * CANVASZS;
    for (int x = 0; x < CANVASZS; x++) {
      if (state.gameover) {
        clearColor = &colBgGameOver;
      } else {
        clearColor = textureSky + i + x;
      }

      drawPixel4(input, x, y, clearColor);
    }
  }

  // walls
  int sum = 0;
  const walSect *ws;
  ws = walFirst();
  while (ws != nullptr) {
    if (ws->y > state.vpY) {
      break;
    }
    sum++;
    putWall(input, ws);
    ws = walNext(ws);
  }

#ifdef WITH_LIGHT
  // apply light+shade
  for (int y = 0; y < state.vpS; y++) {
    for (int x = 0; x < state.vpS; x++) {
      i = (x + y * state.vpS) * BPP;
      clearColor = litVal(x, state.vpY - y); // var reuse
      input[i + 0] = sum255(clearColor.r, input[i + 0]);
      input[i + 1] = sum255(clearColor.g, input[i + 1]);
      input[i + 2] = sum255(clearColor.b, input[i + 2]);
    }
  }
#endif

  // objects
  objState *objPtr;
  for (int i = 0; i < OBJCNT; i++) {
    objPtr = state.obj + i;
    switch (objPtr->type) {
    case OTYPE_ERING:
      putBitmap(input, objPtr->x, state.vpY - objPtr->y, objPtr->zoom,
                pixOppo16x16.pix);
      break;
      // case OTYPE_EBOX:
      //   break;
    }
  }

  // player
  int px = state.plX;
  int py = state.vpY - state.plY;
  int zoom;
  if (state.gameover) {
    if (state.timer > 0) {
      // EXPL_DIVISOR * EXPL_NSTEPS / EXPL_MULTIPLIER * 33 / state.frameDelay;
      zoom = (EXPL_NSTEPS * EXPL_DIVISOR - state.timer * EXPL_NSTEPS / 2);
      putBitmap(input, px, py, zoom, pixShp16x16.pix);
    }
  } else {
    putBitmap(input, px, py, 1, pixShp16x16.pix);
  }

  // projectiles
  for (int i = 0; i < PROJCNT; i++) {
    objPtr = state.projctl + i;
    switch (objPtr->type) {
    case OTYPE_BNORM:
      putProj(input, objPtr->x, state.vpY - objPtr->y + PIXSZ2);
      break;
      // case OTYPE_BPOWER:
      //   break;
    }
  }

  putFontNumber(input, CANVASZS - 10, 5, topScore);
  putFontNumber(input, CANVASZS - 10, 11, state.score);

  frame++;
}

void putBitmap(byte *canvas, int toX, int toY, int zoom,
               const byte pix[PIXSZ2][PIXSZ2 + 1]) {
  int fx, fy;
  const col4 *col;
  // const pixMapLine *line;
  zoom = zoom < EXPL_DIVISOR ? 1 : zoom / EXPL_DIVISOR;
  toX -= PIXSZ2 * zoom;
  toY -= PIXSZ2 * zoom;
  for (int py = 0; py < PIXSZ; py++) {
    fy = py < PIXSZ2 ? py : PIXSZ - py - 1;
    // line = pix + fy;
    for (int px = 0; px < PIXSZ; px++) {
      fx = px < PIXSZ2 ? px : PIXSZ - px - 1;
      col = globalPal + pix[fy][fx];
      drawPixel4(canvas, toX + px * zoom, toY + py * zoom, col);
    }
  }
  // putFontNumber(canvas, toX, toY - 5, zoom);
}

void putFontNumber(byte *canvas, int toX, int toY, int number) {
  // auto pal = pixFont3x5.pal;
  auto pix = pixFont3x5.pix;
  int digitX, rangeX = 0;
  col4 *col;
  do {
    digitX = (number % 10) * 3;

    for (int py = 0; py < 5; py++) {
      for (int px = 0; px < 3; px++) {
        col = globalPal + pix[py][digitX + px];
        drawPixel4(canvas, toX - rangeX + px, toY + py, col);
      }
    }

    number = number / 10;
    rangeX += 3 + 1;
  } while (number > 0);
}

void putProj(byte *canvas, int toVpX, int toVpY) {
  int x1 = toVpX - PIXSZ2 + 2;
  int x2 = toVpX + PIXSZ2 - 2;

  drawPixel4(canvas, x1, toVpY, &colProjectile);
  drawPixel4(canvas, x2, toVpY, &colProjectile);
  drawPixel4(canvas, x1, toVpY - 1, &colProjectile);
  drawPixel4(canvas, x2, toVpY - 1, &colProjectile);
}

void putWall(byte *canvas, const walSect *wal) {
  int y1 = state.vpY - wal->y - wal->yDist;
  int y2 = y1 + wal->yDist;

  if ((y1 > CANVASZS && y2 > CANVASZS) || (y1 < 0 && y2 < 0)) {
    return;
  }

  int x, y;
  int texOfs = (wal->y % CANVASZS) * CANVASZS;
  for (y = y1; y < y2; y++) {
    texOfs = (texOfs + CANVASZS) % (CANVASZS * CANVASZS);
    if (y >= CANVASZS || y < 0) {
      continue;
    }

    for (x = 0; x < CANVASZS; x++) {
      if (x < wal->xLeft || x > wal->xRight ||
          (x >= wal->mXLeft && x <= wal->mXRight)) {
        drawPixel4(canvas, x, y, textureWall + texOfs + x);
      }
    }
  }
}
