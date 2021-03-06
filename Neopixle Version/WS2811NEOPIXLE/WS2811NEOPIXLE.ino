#include <Adafruit_NeoPixel.h>

#define LED_PIN    3
#define LED_COUNT 50
#define MIC_PIN A0 // Microphone
#define N_PIXELS 50 // number of pixels in LED strand
#define N 100 // Number of samples to take each time readSamples is called
#define fadeDelay 10 // delay time for each fade amount
#define noiseLevel 2 // slope level of average mic noise without sound

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

int samples[N]; // storage for a sample collection set
int periodFactor = 0; // keep track of number of ms for period calculation
int t1 = -1; // times of slope > 100 detected.
int T; // period between times scaled to milliseconds
int slope; // the slope of two collected data sample points
byte periodChanged = 0;

void setup() {
  strip.begin();
  ledsOff();
  delay(500);
  displayColor(Wheel(100));
  strip.show();
  delay(500);
}

void loop() {
  readSamples();
}

void readSamples() {
  for(int i=0; i<N; i++) {
    samples[i] = analogRead(0);
    if(i>0) {
      slope = samples[i] - samples[i-1];
    }
    else {
      slope = samples[i] - samples[N-1];
    }
    // Check if Slope greater than noiseLevel - sound that is not at noise level detected
    if(abs(slope) > noiseLevel) {
      if(slope < 0) {
        calculatePeriod(i);
        if(periodChanged == 1) {
          displayColor(getColor(T));
        }
      }
    }
    else {
      ledsOff();
    }
    periodFactor += 1;
    delay(1);
  }
}

void calculatePeriod(int i) {
  if(t1 == -1) {
    // t1 has not been set
    t1 = i;
  }
  else {
    // t1 was set so calc period
    int period = periodFactor*(i - t1);
    periodChanged = T==period ? 0 : 1;
    T = period;
    //Serial.println(T);
    // reset t1 to new i value
    t1 = i;
    periodFactor = 0;
  }
}

uint32_t getColor(int period) {
  if(period == -1)
    return Wheel(0);
  else if(period > 400)
    return Wheel(5);
  else
    return Wheel(map(-1*period, -400, -1, 50, 255));
}

void fadeOut()
{
  for(int i=0; i<5; i++) {
    strip.setBrightness(110 - i*20);
    strip.show(); // Update strip
    delay(fadeDelay);
    periodFactor +=fadeDelay;
  }
}

void fadeIn() {
  strip.setBrightness(100);
  strip.show(); // Update strip
  // fade color in
  for(int i=0; i<5; i++) {
    //strip.setBrightness(20*i + 30);
    //strip.show(); // Update strip
    delay(fadeDelay);
    periodFactor+=fadeDelay;
  }
}

void ledsOff() {
  fadeOut();
  for(int i=0; i<N_PIXELS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
}

void displayColor(uint32_t color) {
  for(int i=0; i<N_PIXELS; i++) {
    strip.setPixelColor(i, color);
  }
  fadeIn();
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
