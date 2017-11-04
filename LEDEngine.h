
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

#include <FastLED.h>

#define NUM_LEDS 60
#define DATA_PIN 4


void flipLeds(CRGB *leds, size_t start, size_t end){
  for(size_t i=0; i < (end-start)/2;i++){
    CRGB tmp = leds[start+i];
    leds[start+i] = leds[end-i-1];
    leds[end-i-1] = tmp;
  }
}

enum class LEDMode { color, rainbow };

class LEDEngine{ 
  public:
  
    enum MODE { 
      COLOR,
      RAINBOW
      };
    
    LEDEngine() {};
  
    void setMode(MODE mode) {
      this->mode = mode;    
    };
  
    void setColor(CRGB color) {
      this->color = color;
    };
  
  
    void setup(){
      FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);    
      //FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 
      FastLED.setBrightness(255);
      FastLED.setTemperature( Tungsten100W );
    };

    void setBrightness(uint8_t br){
      this->brightness = br;
    }
  
    void update(){
      if(mode == COLOR){
        for(int i=0; i< NUM_LEDS; i++){
          leds[i] = color;
        }
      }else{
        uint8_t beatA = beatsin8(7, 0, 255);                        // Starting hue
        uint8_t beatB = beatsin8(5, 0, 255);
        fill_rainbow(leds, NUM_LEDS, (beatA+beatB)/2, 8); // Use FastLED's fill_rainbow routine.
        flipLeds(leds, NUM_LEDS/2, NUM_LEDS);
        uint8_t brightness = beatsin8(9,150,255);
        for(size_t i=0; i<NUM_LEDS; i++){
          leds[i] %= brightness;
        }
      }
      FastLED.setBrightness(brightness);
      FastLED.show();
    };

  private:
    CRGB leds[NUM_LEDS];
    MODE mode = LEDEngine::RAINBOW;
    CRGB color = CRGB(255,255,255);
    uint8_t brightness = 255;
};
