

// NOTE: squares board has the following hardwired:
//  CPU_Out_A -> PA0
//  CPU_Out_B -> PA1
//  MUX_Sel0 -> PB0 | AIN0 (comparator+)
//  MUX_Sel1 -> PB1 | AIN1 (comparator-)
//  CPU_PWM_0 -> PB2 | OC0A (timer0 pwm out) | PCINT2 (pin change interrupt source)
//  CPU_PWM_1A-> PB3 | OC1A (timer1 pwmA out) | PCINT3
//  CPU_PWM_1B-> PB4 | OC1B (timer1 pwmB out) | PCINT4
//  MUX_Out -> PD0
//  MUX_Partial_AB -> PD1
//  MUX_Partial_CD -> PD2 | INT0 (external interrupt source)
//  MUX_In_A -> PD3 | INT1 (external interrupt source)
//  MUX_In_B -> PD4 | T0 (timer0 external clock)
//  MUX_In_C -> PD5 | T1 (timer1 external clock) or OC0B (timer0 pwm out++)
//  MUX_In_D -> PD6 | ICP (timer1 input capture pin)


// output wave on OC1A (PB3)


void setup() {
  pinMode(PIN_PB0, INPUT);
  pinMode(PIN_PB1, INPUT);
  pinMode(PIN_PD4, INPUT);

  digitalWrite(PIN_PD4, 1);
  pinMode(PIN_PB3, OUTPUT);

  // THINKS: maybe use 8-bit timer0 for ramp PWM, and use 16-bit timer1 with input capture for z

  // fast PWM 8bit, superfast clock
  TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
  TCNT1 = 0;
  OCR1A = 0x0080;

  // enable TIMER1_OVF interrupt (disable all timer1 interrupts and just enable ours)
  TIMSK &= ~(_BV(TOIE1) | _BV(OCIE1A) | _BV(OCIE1B) | _BV(ICIE1));
  TIMSK |= _BV(TOIE1);
}

constexpr uint16_t freq = 100;
constexpr uint8_t  maxCount = 255;
constexpr uint32_t cyclesPerSec = 8000000;
constexpr uint32_t cyclesPerLoop = 256;
constexpr uint32_t loopsPerSec = cyclesPerSec / cyclesPerLoop;
constexpr uint32_t maxU16 = 0xffff;
constexpr uint32_t loopsPerWave = loopsPerSec / freq;
constexpr uint32_t increment32 = maxU16 / loopsPerWave;
static_assert(increment32 <= 0xffff);
constexpr uint16_t increment = uint16_t(increment32);

volatile uint16_t val_8 = 0;


ISR(TIMER1_OVF_vect)
{
    val_8 += increment;
    OCR1A = byte(val_8 >> 8);
}


void loop() {

//  for(;;)
//  {
//    val_8 += increment;
//    OCR1A = byte(val_8 >> 8);
//  }

}
