#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/**
 * How to adjust pwm frequency:
 * https://forum.arduino.cc/t/varying-the-pwm-frequency-for-timer-0-or-timer-2/16679/4
 */
#define _configureTimer(REGISTER, SETTING) (void)(REGISTER = (REGISTER & 0b11111000) | (SETTING))
#define configureTimerPins_5_6(SETTING) _configureTimer(TCCR0B, SETTING)
#define configureTimerPins_9_10(SETTING) _configureTimer(TCCR1B, SETTING)
#define configureTimerPins_11_3(SETTING) _configureTimer(TCCR2B, SETTING)

#define VOLTS_MAX 5.0
#define AIN_VIN A0
#define AOUT_PWM1 5
#define AOUT_PWM2 6

typedef enum BridgebridgeControlEnum {
  BRIDGE_COAST,
  BRIDGE_REVERSE,
  BRIDGE_FORWARD,
  BRIDGE_BRAKE,
} BridgebridgeControlEnum;

// 0 - 255, 0 = no PWM, 255 = max duty cycle, 64 = quarter duty cycle
typedef struct PinStruct {
  int pwm1DutyOut;
  int pwm2DutyOut;
  int analogIn;
} PinStruct;

static BridgebridgeControlEnum bridgeControl = BRIDGE_BRAKE;
static PinStruct pins;

static struct {
  int pinNumber; int inOrOut; int *io;
} pinConfig[] = {
  {AOUT_PWM1, OUTPUT, &pins.pwm1DutyOut},
  {AOUT_PWM2, OUTPUT, &pins.pwm2DutyOut},
  {A0, INPUT, &pins.analogIn}
};

void setup() {
  Serial.begin(9600);

  // load pin table
  for (int i; i < ARRAY_SIZE(pinConfig); i++) {
    pinMode(pinConfig[i].pinNumber, pinConfig[i].inOrOut);
  }

  /**
   * Set PWM Frequency
   * 0x01 - 62500
   * 0x02 - 7812.5
   * 0x03 - 976.5625   <--DEFAULT
   * 0x04 - 244.140625
   * 0x05 - 61.03515625
   */
  configureTimerPins_5_6(0x01);
}

void loop() {
  // read all IOs
  for (int i = 0; i < ARRAY_SIZE(pinConfig); i++) {
    if (pinConfig[i].inOrOut == INPUT) {
      *pinConfig[i].io = analogRead(pinConfig[i].pinNumber);
    }
  }

  // read if -1 (0xFF is -1 for an int8 but 255 for an int4). Nonblocking read.
  int character = Serial.peek();
  if (character != 255) {
    character = Serial.read();
    switch (character) {
    case 'x':
      bridgeControl = BRIDGE_COAST;
      break;
    case 's':
      bridgeControl = BRIDGE_REVERSE;
      break;
    case 'w':
      bridgeControl = BRIDGE_FORWARD;
      break;
    case 'b':
      bridgeControl = BRIDGE_BRAKE;
      break;
    }

    Serial.println(pins.analogIn / 1024.0f * 5.0f, DEC);
  }

  // scaling: 1-1024 to 0-255
  float convert = pins.analogIn / 4.0f;
  switch (bridgeControl) {
    // drv8871.pdf:8 shows HIs and LOs
  case BRIDGE_COAST:
    pins.pwm1DutyOut = 0;
    pins.pwm2DutyOut = 0;
    break;
  case BRIDGE_REVERSE:
    pins.pwm1DutyOut = 0;
    pins.pwm2DutyOut = convert;
    break;
  case BRIDGE_FORWARD:
    pins.pwm1DutyOut = convert;
    pins.pwm2DutyOut = 0;
    break;
  case BRIDGE_BRAKE:
    pins.pwm1DutyOut = convert;
    pins.pwm2DutyOut = convert;
    break;
  }

  // write all IOs
  for (int i = 0; i < ARRAY_SIZE(pinConfig); i++) {
    if (pinConfig[i].inOrOut == OUTPUT) {
      analogWrite(pinConfig[i].pinNumber, *pinConfig[i].io);
    }
  }

  delay(50000);
}
