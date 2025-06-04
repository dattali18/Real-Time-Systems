#include "PARAL25.H"
#include "TASK25.H"
#include "SMARTS25.H"

unsigned getTimerClocks() {
  // Gets the remaining clocks of the timer register

  unsigned clocks;
  /* latch counter #0 */
  outportb(0x43, 0x00);
  /* read counter #0 low byte */
  clocks = inportb(0x40);
  /* read counter #0 high byte */
  clocks += inportb(0x40) << 8;
  return clocks;
}
