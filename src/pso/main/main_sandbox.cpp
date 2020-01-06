
#include <ncurses.h>
#include <iostream>

int main()
{
  //printf("\033c");
  int c = 0;
  while(1)
  {
    c = 0;
    switch((c=getch())) {
    case KEY_UP:
      break;
    case KEY_DOWN:
      break;
    case KEY_LEFT:
      break;
    case KEY_RIGHT:
      break;
    default:
      break;
    }
  }
}
