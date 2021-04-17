#include <irrigation.h>

int Irrigation::add(int a, int b) {
    return a + b;
}

int Irrigation::sub(int a, int b) {
    return a - b;
}

int Irrigation::mul(int a, int b) {
    return a * b;
}

int Irrigation::div(int a, int b) {
    return a / b;
}

char[] Irrigation::hello() {
    return "hello";
}




Unit::Unit(int xPos, int yPos) {
  x = xPos;
  y = yPos;
}

void Unit::draw(Adafruit_BicolorMatrix &matrix){
  matrix.drawPixel(x, y, LED_GREEN);
  //matrix.writeDisplay();  // write the changes we just made to the display
}
