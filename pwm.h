struct Led {
  int redpin;
  int greenpin;
  int bluepin;
};

struct Colour {
  int red;
  int green;
  int blue;
};

struct Pattern {
  int steps;
  int stepdelay;
  struct Colour led1colours[10];
  struct Colour led2colours[10];
};

void setup_pwm(struct Led led, int limit);
void set_led(struct Colour colour, struct Led led, int limit);
void fade_between(struct Led led, struct Colour from, struct Colour to, int steps, int step, int limit);

