#include "font.h"

int main()
{
    Init_Font();
    Clean_Area(0,0,300,100,0x0000ff00);
    Display_characterX(0,0,"ÄãºÃ",0x00ff0000,2);
    UnInit_Font();
    return 0;
}
