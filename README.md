# WS2811-Reactive-Audio-Controller
This repo is for the development of a WS2811 Reactive Audio Controller. The exact model used can be found **[Here](https://www.amazon.com/ALITOVE-LED-Individually-Addressable-Waterproof/dp/B01AG923GI/ref=sr_1_8?crid=36HU3U82TB4BM&dchild=1&keywords=ws2811&qid=1615448300&sprefix=ws2%2Caps%2C193&sr=8-8)**


![as](https://images-na.ssl-images-amazon.com/images/I/618sPBfPe-L._AC_SL1000_.jpg )



Table of contents
=================

<!--ts-->
   * [Prerequisites](#Prerequisites)
   * [Wiring](#Wiring)
   * [How To Use](#How-To-Use)
<!--te-->

Prerequisites
============
1. Arduino
2. Lights
3. Wire
4. 12V PSU
5. 5V USB or small PSU for arduino

Wiring
============
- Connect 12v PSU to strip.
- Connect D out arduino pin to the Din on the strip and the ground pin the the same ground as the PSU
- Connect Arduino to a usb or a 5V psu 

How To Use
============

Initialize Arduino

### FastLed

```ino 
#include <FastLED.h>

#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB 
#define NUM_LEDS    50
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         60
#define FRAMES_PER_SECOND  120
```

### NeoPixle
```ino
#include <Adafruit_NeoPixel.h>

#define LED_PIN    3
#define LED_COUNT 50
#define MIC_PIN A0 // Microphone
#define N_PIXELS 50 // number of pixels in LED strand
#define N 100 // Number of samples to take each time readSamples is called
#define fadeDelay 10 // delay time for each fade amount
#define noiseLevel 2 // slope level of average mic noise without sound

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
```