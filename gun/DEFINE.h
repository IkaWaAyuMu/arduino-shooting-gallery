// GAMEPLAY SETUP

#define AMMO_COUNT    20    // Ammo count each magazine
#define RELOAD_TIME   500  // Reload time (in ms)
#define FIRE_RATE     100   // Fire rate (in ms)
#define VISUAL_DELAY  25    // Visual display speed (in ms)
#define ISAUTO        false // Is this automatic


// ------------------------------------------------------------------
// COMPONENTS DEFINITION
// BUTTONS
#define FIRE_BUTTON   D6    // Fire button
#define RELOAD_BUTTON D7    // Reload Button


// INFRALED TRANSMITTER
#define IRLED D5
#define VISUAL_PIN  D3      // Visual indicator pin

// TM1637
#define TM1637_CLK    D2     // CLK pin
#define TM1637_DIO    D1     // DIO pin
#define TM1637_DIG    4      // Digit counts
#define TM1637_DELAY  100    // Transtions Delay (in ms)
// ------------------------------------------------------------------
// TIMER INTERRUPT

#define USING_TIM_DIV1    false           
#define USING_TIM_DIV16   false
#define USING_TIM_DIV256  true

// ------------------------------------------------------------------
// TM1637
const uint8_t NUM_DIGIT[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
};

const uint8_t DASH = 0b01000000;  // -
// ------------------------------------------------------------------