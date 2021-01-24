#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>

int main() {
  fixedpt a = fixedpt_rconst(-1.2);
  fixedpt b = fixedpt_rconst(8388607);
  fixedpt c = fixedpt_rconst(5.6/1.2);
  printf("a = %d   b = %d  \n",a,b);
  printf("%x  %d  %d\n",fixedpt_rconst(-1.2),fixedpt_rconst(1.2),fixedpt_abs(fixedpt_rconst(-1.2)));
  printf("res a  floor = %d  ceil = %d\n",fixedpt_floor(a),fixedpt_ceil(a));
  printf("res b  floor = %d  ceil = %d\n",fixedpt_floor(b),fixedpt_ceil(b));
  assert(fixedpt_abs(fixedpt_floor(a)-fixedpt_ceil(a)) == 256);
  assert(fixedpt_abs(fixedpt_floor(b)-fixedpt_ceil(b)) == 256);
  assert(fixedpt_abs(fixedpt_floor(c)-fixedpt_ceil(c)) == 256);
  return 0;
}
