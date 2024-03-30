#include <LiquidCrystal.h>
// LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.cursor();
  lcd.print("Hello World!");
}

void loop() {
}
