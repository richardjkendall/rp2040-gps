struct Colour {
  int red;
  int green;
  int blue;
};

void setup_pwm();
void set_led1(struct Colour led);
void fade_between(struct Colour from, struct Colour to, int steps, int step);

