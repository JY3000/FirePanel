#include <FastLED.h>
# include <SPI.h>
# include <SdFat.h>
# include <SFEMP3Shield.h>

/* NeoPixel Shield data pin is always 6. Change for other boards */
#define CONTROL_PIN1 1                                // Turning on pin 1; code will be sent through pin

#define CONTROL_PIN2 2                                // Turning on pin 2; code will be sent through pin
#define CONTROL_PIN3 3                                // Turning on pin 3; code will be sent through pin

/* Board shape a nd size configuration. Sheild is 8x5, 40 pixels */
#define HEIGHT 16                                     // number of leds "height" wise, but is technically the width of our fire
#define WIDTH 32                                      // number of leds "width" wise, but is technically the height of our fire
#define NUM_LEDS HEIGHT*WIDTH                         // Total # of leds
/*Setting these values to integer allows us to create code that will change the brightness and height of fire gradually*/
int BRIGHTNESS = 10;                                  // set initial brightness value and making it a variable instead of a constant
int count = 0 ;                                       // set initial counting time and making it a variable instead of a constant
int StartTime = 0 ;                                   // set initial start time and making it a variable instead of a constant
int EndTime = 0 ;                                     // set initial end time and making it a variable instead of a constant
int ElapsedTime = 0 ;                                 // set initial elapsed time and making it a variable instead of a constant
int HOT = 360 ;                                       // set initial "HOT" value and making it a variable instead of a constant; best start is 360

SdFat sd;                                             //create object to handle SD functions
SFEMP3Shield MP3player;                               // Create Mp3 library object
// These variables used in MP3 initialization to set up
uint8_t volume = 0;                             // MP3 Player volume 0=max, 255=lowest(off)
const uint16_t monoMode = 1;                          // Mono setting 0=off, 3=max
int triggerpin = 4;                                   // set input pin for MP3 File

CRGB leds[NUM_LEDS];                                  // How many leds to give palette to
CRGBPalette16 gPal;                                   // Color is chosen to a palette aalready in the FastLED library (color palette "gpal" defined below)

void setup() {
  // MP3 Trigger:
{
  pinMode (triggerpin, INPUT);
}
  //LEDs:
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN1>(leds, NUM_LEDS);
  
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN2>(leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN3>(leds, NUM_LEDS);

  /*FastLED.setBrightness(BRIGHTNESS);*/
  
  /* Set a black-body radiation palette
     This comes from FastLED */
  gPal = HeatColors_p;                                // Color palete variable is defined here
  
  /* Clear display before starting */
  FastLED.clear();                                    // Clears the leds and display
  FastLED.show();                                     // updates all controllers with color palette
  FastLED.delay(1500);                                // Sanity before start; gives a delay before beginning program
}

/* Refresh rate. Higher makes for flickerier
   Recommend small values for small displays */
#define FPS 25                                        // orignially 17; this value will change speed at 
#define FPS_DELAY 1000/FPS                            // delay between each frame

void loop() {
  random16_add_entropy( random() );                   // Add entropy to random number generator; we use a lot of it
StartTime = millis() ; 
if (BRIGHTNESS <= 60 && millis() % 100 == 0)
{
  BRIGHTNESS++;
}

  FastLED.setBrightness(BRIGHTNESS);                  // Sets the brightness of panel

 StartTime = millis() ;                               // the start time is equal to the # of seconds since the program started
if (HOT >= 270 && millis() % 100 == 0)
{
  HOT--;                                              // Change value as needed; this is the value at which the height will stop increasing
}

  Fireplace();
EndTime = millis();                                   // end time is equal to the # of seconds since the program started
ElapsedTime = EndTime - StartTime ;                   // elapsed time is equal to the difference of the time elapsed (EndTime) and the start time

  FastLED.show();                                     // updates all controllers with the new variable values
  FastLED.delay(FPS_DELAY);                           // delay before starting program

}

/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) */
#define COOLING 30                                  //changes ratio of orange, red, and white light

/* How hot is "hot"? Increase for brighter fire */
// #define HOT 285 //range from 250 to 300 produces the increase gradient of fire height that we want
// start at 360 and decrease hot to 285
#define MAXHOT HOT*HEIGHT

void Fireplace () {
  static unsigned int spark[WIDTH];                 // base heat
  CRGB stack[WIDTH][HEIGHT];                        // stacks that are cooler
 
  // 1. Generate sparks to re-heat
  for( int i = 0; i < WIDTH; i++) {                 // sets "i" as an variable starting at 0, if i is maller than the width value, increass it by 1
    if (spark[i] < HOT ) {                          // 
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
      if (drop > heat) heat = 0;                    // avoid wrap-arounds from going "negative"; if the drop is greater than the heat value, heat will be equal to 0
      else heat -= drop;                            // or else heat will heat = heat minus drop
 
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

// MP3 Shield:
void MP3() {
  {
    MP3player.setVolume(volume, volume); //sets volume with 1st and 2nd parameters the left and right channels respectively
    digitalWrite(4, LOW); // turns pin 4 on
    // if loop to increase volume as time goes on
    if (millis() % 100 == 0)
    {
      volume++;
    }
  }
MP3();
}
