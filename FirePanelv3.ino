#include <FastLED.h>

/* NeoPixel Shield data pin is always 6. Change for other boards */
#define CONTROL_PIN1 1
#define CONTROL_PIN2 2
#define CONTROL_PIN3 3

/* Board shape and size configuration. Sheild is 8x5, 40 pixels */
#define HEIGHT 16
#define WIDTH 16
#define NUM_LEDS HEIGHT*WIDTH
int BRIGHTNESS = 15;
int count = 0 ;
int StartTime = 0 ; 
int EndTime = 0 ; 
int ElapsedTime = 0 ; 

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

void setup() {
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN1>(leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN2>(leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN3>(leds, NUM_LEDS);
 
  /*FastLED.setBrightness(BRIGHTNESS);*/
  
  /* Set a black-body radiation palette
     This comes from FastLED */
  gPal = HeatColors_p; 
  
  /* Clear display before starting */
  FastLED.clear();
  FastLED.show();
  FastLED.delay(1500); // Sanity before start
}

/* Refresh rate. Higher makes for flickerier
   Recommend small values for small displays */
#define FPS 17
#define FPS_DELAY 1000/FPS

void loop() {
  random16_add_entropy( random() ); // We chew a lot of entropy
StartTime = millis() ; 
if ((BRIGHTNESS < 60) && (millis() % 100 == 0)) {
  BRIGHTNESS++;
}
  FastLED.setBrightness(BRIGHTNESS);
  
  Fireplace();
EndTime = millis();
ElapsedTime = EndTime - StartTime ;

  FastLED.show();
  FastLED.delay(FPS_DELAY); //

}

/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) */
#define COOLING 30  

/* How hot is "hot"? Increase for brighter fire */
#define HOT 300
#define MAXHOT HOT*HEIGHT

void Fireplace () {
  static unsigned int spark[WIDTH]; // base heat
  CRGB stack[WIDTH][HEIGHT];        // stacks that are cooler
 
  // 1. Generate sparks to re-heat
  for( int i = 0; i < WIDTH; i++) {
    if (spark[i] < HOT ) {
      int base = HOT * 2;
      spark[i] = random16( base, MAXHOT );
    }
  }
  
  // 2. Cool all the sparks
  for( int i = 0; i < WIDTH; i++) {
    spark[i] = qsub8( spark[i], random8(0, COOLING) );
  }
  
  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for( int i = 0; i < WIDTH; i++) {
    unsigned int heat = constrain(spark[i], HOT/2, MAXHOT);
    for( int j = HEIGHT-1; j >= 0; j--) {
      /* Calculate the color on the palette from how hot this
         pixel is */
      byte index = constrain(heat, 0, HOT);
      stack[i][j] = ColorFromPalette( gPal, index );
      
      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0,HOT);
      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
      else heat -= drop;
 
      heat = constrain(heat, 0, MAXHOT);
    }
  }
  
  // 4. map stacks to led array
  for( int i = 0; i < WIDTH; i++) {
  for( int j = 0; j < HEIGHT; j++) {
     leds[(j*WIDTH) + i] = stack[i][j];
  }
  }
  for( int count = 0; count++;);
}
