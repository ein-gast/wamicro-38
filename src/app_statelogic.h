#include "app_colision.h"
#include "app_defs.h"
#include "app_globals.h"

void do_gameover() {
  state.timer = EXPL_DIVISOR * EXPL_NSTEPS / EXPL_MULTIPLIER;
  state.gameover = true;
}

void o_player() {
  int col;
  // col = collWithObj(state.plX, state.plY);
  // if (col >= 0) {
  //   state.obj[col].type = OTYPE_NONE;
  //   do_gameover();
  //   return;
  // }
  col = collWithWall(state.plX, state.plY, PIXSZ, PIXSZ2);
  if (col >= 0) {
    do_gameover();
    return;
  }
}

void o_ering(objState *obj) {
  if (obj->zoom > 1) {
    if ((obj->zoom += EXPL_MULTIPLIER) > EXPL_DIVISOR * EXPL_NSTEPS) {
      obj->type = OTYPE_NONE;
    }
    return;
  }
  if (obj->y < state.vpY - CANVASZS - PIXSZ) {
    obj->type = OTYPE_NONE;
    return;
  }
}

void o_prjctl_norm(objState *projctl) {
  for (int k = 0; k < 3; k++) {
    (projctl->y)++;
    if (projctl->y > state.vpY) {
      projctl->type = OTYPE_NONE;
      return;
    }
  }

  int col = collWithObj(projctl->x, projctl->y);
  if (col >= 0) {
    projctl->type = OTYPE_NONE;
    // state.obj[col].type = OTYPE_NONE;
    state.obj[col].zoom = EXPL_DIVISOR * 2;
    state.score += SCORE_ENEMY;
    return;
  }

  col = collWithWall(projctl->x, projctl->y, PIXSZ, PIXSZ2 / 2);
  if (col >= 0) {
    projctl->type = OTYPE_NONE;
    return;
  }
}
