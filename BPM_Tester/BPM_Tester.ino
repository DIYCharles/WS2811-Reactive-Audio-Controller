
#include <FastLED.h>
/* 
Fucking around with some BPM stuff. Most of this is ripped straight from
the FastLED DemoReel100 example. Future improvements include interfacing with a simple 
analog microphone to dynamically determine BPM.
*/

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//START of PIN definition
#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    50
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         60
#define FRAMES_PER_SECOND  120
//END

//START of sensor definition
// Global Variables
int Bias = 521;                           //Nominally half of the 1023 range = 512.  Adjust as necessary
int NoiseThreshold = 6;                   //Increase to minimize false triggers with silence or background noise
int Analog;                               // The analog reading (with the bias subtracted-out)
int Max;                                  // The waveform peak over a short period of time
int Average = 60;                              // A "sampled" moving average of Max values used as the LED-on threshold (not the true-average)

unsigned long ReadStartTime;             //millis() used to set time
int SampleTime = 50;                     //Sample for short time to find waveform peak
unsigned long ArrayStartTime;            //millis() again
int ArrayTime = 1000;                    //Update array once per second

const int ArraySize = 20;                // 20 values in the moving-average array
int ArrayIndex = 0;                      // Index/pointer
int AverageArray[ArraySize];             // Saves a sample of 20 Max values for averaging

//Function prototype
int UpdateAverage(int Max);               //UpdateAverage() function prototype

//END

void setup() {
  Serial.begin(9600);
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //initialize the array with zeros
  for(int i = 0; i < 20; i++)
    AverageArray[i] = 0; 

}
uint8_t gHue = 0; // rotating "base color" used for the rainbow.

void loop() {
  Max = 0;                                  //Initilize/reset before running while() loop
  ReadStartTime = millis();                 //Save/update loop-starting time

  // Find waveform peak loop ==================================================================================
  // Takes readings in a "fast loop" to find the waveform peak.
  // At 20Hz a half-cycle is 25mS, so 25ms is the minimum time to find the peak (at deep bass frequencies).
  // A longer sample of about 100mS may work better with incancescant lamps (allowing the light to reach full-brightness). 
 
  while (millis() - ReadStartTime < SampleTime)
  {
    Analog = abs(analogRead(A0) - Bias);                 // Read, take out the 2.5V bias/offset, make positive.

    if (Analog > Max)                                    //Find Max before exiting loop
    {
      Max = Analog;
    }
  }// End waveform peak loop ==================================================================================

  //TURN THE LED ON/OFF ===================================================================================
  if (Average < NoiseThreshold) 
  {            // Make sure LED is off with silence or background noise
    Average = NoiseThreshold;         
  }
  if (millis() - ArrayStartTime > ArrayTime)
  {// Time to update array?
    Average = UpdateAverage(Max);            // Call the UpdateAverage() function once per second
  }
  bpm(Analog);
   // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

}

void bpm(uint8_t dynamicBPM)
{
  Serial.print (" BPM = ");
  Serial.println(dynamicBPM);
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = dynamicBPM;
  CRGBPalette16 palette = RainbowColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*0.1), beat-gHue+(i*10));
  }
}

// Takes-in latest Max reading, updates the array, returns the average
int UpdateAverage(int Max)
{
  int Sum = 0;    // Inialize/reset Sum before calculating average
  AverageArray[ArrayIndex] = Max;                //Update one array element with the latest Max

  for(int i = 0; i < 20; i++)                    //Sum-up the data in the array
  {
    Sum = Sum + AverageArray[i]; 
  }

  Average = Sum/20;                              // Find average of the saved Max values

  ArrayIndex++;                                  //Next index for next time
  if (ArrayIndex > 19)
    ArrayIndex = 0;                              //Back to the beginning of the circuar buffer

  //Print information (Optional, once per second for testing/debugging
 
  ArrayStartTime = millis();                     //New Array Start Time 
 
  return Average;       
}
