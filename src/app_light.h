#include "app_defs.h"

lightState lights[LITCNT];
int lightPtr = 0;

int litAdd(int x, int y, int val, const col4 col) {
  if (lightPtr > LITCNT) {
    return 2;
  }
  lights[lightPtr].x = x;
  lights[lightPtr].y = y;
  lights[lightPtr].val = val;
  lights[lightPtr].c = col;
  lightPtr++;
  return 0;
}

void litReset() { lightPtr = 0; }

byte cap255(float val) {
  if (val > 255.0) {
    return 255;
  } else {
    return (byte)val;
  }
}

col4 litVal(int x, int y) {
  col4 col, res = {0, 0, 0, 0};
  if (!lightPtr) {
    return res;
  }
  float r = 0, g = 0, b = 0, val;
  float dist;
  for (int i = 0; i < lightPtr; i++) {
    val = lights[i].val;
    col = lights[i].c;
    dist = (x - lights[i].x) * (x - lights[i].x) +
           (y - lights[i].y) * (y - lights[i].y);
    if (dist < 1.0)
      dist = 1.0;
    r += col.r / dist * val;
    g += col.g / dist * val;
    b += col.b / dist * val;
  }
  const float lwr = 2.0;
  res.r = cap255(r / lwr);
  res.g = cap255(g / lwr);
  res.b = cap255(b / lwr);
  return res;
}