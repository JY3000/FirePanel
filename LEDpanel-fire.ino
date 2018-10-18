#include <FastLED.h>

/* NeoPixel Shield data pin is always 6. Change for other boards */
#define CONTROL_PIN 0

/* Board shape and size configuration. Sheild is 8x5, 40 pixels */
#define HEIGHT 48 // number of rows
#define WIDTH 48 // number of columns
#define NUM_LEDS HEIGHT*WIDTH //number of leds

CRGB leds[NUM_LEDS]; //gives all leds RGB pixels
CRGBPalette16 gPal; //the color palette is defined in gPal variable

void setup() {
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN>(leds, NUM_LEDS); // tells library there are neopixels in pin (CONTROL_PIN) which was defined above.


  
  /* Set a black-body radiation palette
     This comes from FastLED */
gPal = CRGBPalette16( CRGB::Black, CRGB::Red); //defines color pallete  
  /* Clear display before starting */
  FastLED.clear(); //clears leds and wipes local array of data
  FastLED.show(); //updates to current led colors
  FastLED.delay(1500); // Sanity before start
}

/* Refresh rate. Higher makes for flickerier
   Recommend small values for small displays */
#define FPS 17
#define FPS_DELAY 1000/FPS

void loop() {
  random16_add_entropy( random() ); // We chew a lot of entropy
  
  Fireplace(); //name of code which is defined below

  FastLED.show(); //updates to current led colors
  FastLED.delay(FPS_DELAY); //delay
}

/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) */
#define COOLING 20  

/* How hot is "hot"? Increase for brighter fire */
#define HOT 90
#define MAXHOT HOT*HEIGHT

void Fireplace () {
  static unsigned int spark[WIDTH]; // the bottom row of leds
  CRGB stack[WIDTH][HEIGHT];        // leds above the bottom row
 
  // sets the values for the bottom row of leds
  for( int i = 0; i < WIDTH; i++) {
    if (spark[i] < HOT ) {
      int base = HOT * 2;
      spark[i] = random16( base, MAXHOT ); //
    }
  }
  
  // lowers the heat of all the bottom leds
  for( int i = 0; i < WIDTH; i++) {
    spark[i] = qsub8( spark[i], random8(0, COOLING) ); //subtracts a random number from each spark value
  }
  
  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for( int i = 0; i < WIDTH; i++) { //iiterates up the width
    unsigned int heat = constrain(spark[i], HOT/2, MAXHOT);
    for( int j = HEIGHT-1; j >= 0; j--) { // iterates down the height
      /* Calculate the color on the palette from how hot this
         pixel is */
      
      byte index = constrain(heat, 0, HOT);
      stack[i][j] = ColorFromPalette( gPal, index );
      
      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0,HOT);
      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
      else heat -= drop; //subtracts the drop value from heat
 
      heat = constrain(heat, 0, MAXHOT);
    }
  }
  
  // 4. map stacks to led array
  for( int i = 0; i < WIDTH; i++) {
  for( int j = 0; j < HEIGHT; j++) {
     leds[(j*WIDTH) + i] = stack[i][j]; //final step, puts new led value onto fire.
  }
  }
  
}
