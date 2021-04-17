#ifndef IRRIGATION_H
#define IRRIGATION_H

class Irrigation {
  public:
    Irrigation() {}
    int add (int a, int b);
    int sub (int a, int b);
    int mul (int a, int b);
    int div (int a, int b);
};

#endif


#ifndef HEADER_MYCLASS
#define HEADER_MYCLASS

class Unit {
  public:
   Unit(int xPos, int yPos);
   int x;
   int y;
   int hp;
   int color;
   void draw(Adafruit_BicolorMatrix &matrix);
};

#endif
