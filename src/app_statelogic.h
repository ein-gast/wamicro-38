#include "app_colision.h"
#include "app_defs.h"
#include "app_globals.h"

int gamoverTicksDelay() {
  return EXPL_DIVISOR * EXPL_NSTEPS / EXPL_MULTIPLIER * 33 / state.frameDelay;
}

void do_gameover() {
  state.timer = 2 * gamoverTicksDelay();
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
    projctl->y += state.baseSpeed;
    if (projctl->y > state.vpY) {
      projctl->type = OTYPE_NONE;
      return;
    }

    int col = collProjWithObj(projctl->x, projctl->y);
    if (col >= 0) {
      projctl->type = OTYPE_NONE;
      // state.obj[col].type = OTYPE_NONE;
      state.obj[col].zoom = EXPL_DIVISOR * 2;
      state.score += SCORE_ENEMY;
      return;
    }

  }

  int col = collWithWall(projctl->x, projctl->y, PIXSZ, PIXSZ2 / 2);
  if (col >= 0) {
    projctl->type = OTYPE_NONE;
    return;
  }
}
