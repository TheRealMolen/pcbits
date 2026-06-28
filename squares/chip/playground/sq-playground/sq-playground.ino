
// all squares boards have the following wired to ISP header
//  RESET ->  PA2 | /RESET
//  MOSI  ->  PB5
//  MISO  ->  PB6
//  SCK   ->  PB7

#define SQR_REV_A 1
#define SQR_REV_B (1 - SQR_REV_A)

#if SQR_REV_A
// squares revA board has the following hardwired:
//  CPU_Out_A -> PA0
//  CPU_Out_B -> PA1
//  MUX_Sel0 -> PB0 | PCINT0
//  MUX_Sel1 -> PB1 | PCINT1
//  CPU_PWM_0 -> PB2 | OC0A (timer0 pwm out) | PCINT2 (pin change interrupt source)
//  CPU_PWM_1A-> PB3 | OC1A (timer1 pwmA out) | PCINT3
//  [unsupported] CPU_Out_C -> PB4 | OC1B (timer1 pwmB out) | PCINT4
//  MUX_Out -> PD0
//  MUX_Partial_AB -> PD1
//  MUX_Partial_CD -> PD2 | INT0 (external interrupt source)
//  MUX_In_A -> PD3 | INT1 (external interrupt source)
//  MUX_In_B -> PD4 | T0 (timer0 external clock)
//  MUX_In_C -> PD5 | T1 (timer1 external clock) or OC0B (timer0 pwm out++)
//  MUX_In_D -> PD6 | ICP (timer1 input capture pin)

constexpr byte Pin_CPU_Out_A  = PIN_PA0;
constexpr byte Pin_CPU_Out_B  = PIN_PA1;
constexpr byte Pin_MUX_Sel0   = PIN_PB0;
constexpr byte Pin_MUX_Sel1   = PIN_PB1;
constexpr byte Pin_CPU_PWM_A  = PIN_PB2;
constexpr byte Pin_CPU_PWM_B  = PIN_PB3;
constexpr byte Pin_MUX_Out    = PIN_PD0;
constexpr byte Pin_Partial_AB = PIN_PD1;
constexpr byte Pin_Partial_CD = PIN_PD2;
constexpr byte Pin_MUX_In_A   = PIN_PD3;
constexpr byte Pin_MUX_In_B   = PIN_PD4;
constexpr byte Pin_MUX_In_C   = PIN_PD5;
constexpr byte Pin_MUX_In_D   = PIN_PD6;

// to simplify code...
constexpr byte Pin_SqrA_In = Pin_MUX_In_A;
constexpr byte Pin_SqrB_In = Pin_MUX_In_B;
constexpr byte Pin_SqrC_In = Pin_MUX_In_C;
constexpr byte Pin_SqrD_In = Pin_MUX_In_D;

#elif SQR_REV_B
// squares revB board has the following hardwired:
//  CPU_Out_A -> PA0
//  CPU_Out_B -> PA1
//  MUX_Sel0 -> PB0 | PCINT0
//  MUX_Sel1 -> PB1 | PCINT1
//  CPU_PWM_A -> PB2 | OC0A (timer0 pwm out) | PCINT2 (pin change interrupt source)
//  CPU_PWM_B -> PB3 | OC1A (timer1 pwmA out) | PCINT3
//  MUX_Out -> PB4 | OC1B (timer1 pwmB out) | PCINT4
//  CPU_Out_C -> PD0
//  MUX_Partial_AB -> PD1
//  MUX_Partial_CD -> PD2 | INT0 (external interrupt source)
//  SqrA_Out -> PD3 | INT1 (external interrupt source)
//  SqrB_Out -> PD4 | T0 (timer0 external clock)
//  SqrC_Out -> PD5 | T1 (timer1 external clock) or OC0B (timer0 pwm out++)
//  SqrD_Out -> PD6 | ICP (timer1 input capture pin)

constexpr byte Pin_CPU_Out_A  = PIN_PA0;
constexpr byte Pin_CPU_Out_B  = PIN_PA1;
constexpr byte Pin_MUX_Sel0   = PIN_PB0;
constexpr byte Pin_MUX_Sel1   = PIN_PB1;
constexpr byte Pin_CPU_PWM_A  = PIN_PB2;
constexpr byte Pin_CPU_PWM_B  = PIN_PB3;
constexpr byte Pin_MUX_Out    = PIN_PB4;  // NOTE: swapped from revA vvv
constexpr byte Pin_CPU_Out_C  = PIN_PD0;  // NOTE: swapped from revA ^^^
constexpr byte Pin_Partial_AB = PIN_PD1;
constexpr byte Pin_Partial_CD = PIN_PD2;
constexpr byte Pin_SqrA_In    = PIN_PD3;
constexpr byte Pin_SqrB_In    = PIN_PD4;
constexpr byte Pin_SqrC_In    = PIN_PD5;
constexpr byte Pin_SqrD_In    = PIN_PD6;

constexpr byte MskD_CPU_Out_C   = 1 << Pin_CPU_Out_C;

#else
#error unknown board revision
#endif


// CPU_PWM_A :: TIMER0 / PWM0 for percussive envelope, triggered by MUX_Partial_CD (INT0)
// CPU_PWM_B :: TIMER1 / PWM1A for sawtooth tracking freq of SqrC, hardsynced to SqrD
// CPU_Out_A :: SqrA xor SqrB
// CPU_Out_B :: SqrC xor SqrD
// CPU_Out_C :: MUX_Sel1 ? (SqrA xor SqrC) : (SqrB xor SqrC)




void setup()
{
  // start with everything hi-z to avoid freaking anything out
  DDRA = 0;
  PORTA = 0;
  DDRB = 0;
  PORTB = 0;
  DDRD = 0;
  PORTD = 0;
  
  pinMode(Pin_CPU_Out_A, OUTPUT);
  pinMode(Pin_CPU_Out_B, OUTPUT);
#if !SQR_REV_A
  pinMode(Pin_CPU_Out_C, OUTPUT);
#endif
  pinMode(Pin_CPU_PWM_A, OUTPUT);
  pinMode(Pin_CPU_PWM_B, OUTPUT);
  pinMode(Pin_MUX_Out, INPUT);
  pinMode(Pin_MUX_Sel0, INPUT);
  pinMode(Pin_MUX_Sel1, INPUT);

  // THINKS: maybe use 8-bit timer0 for ramp PWM, and use 16-bit timer1 with input capture for z

  // TIMER1 for PWM_B
  // fast PWM 8bit, superfast clock
  TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
  TCNT1 = 0;
  OCR1A = 0x0080;

  // enable TIMER1_OVF interrupt (disable all timer1 interrupts and just enable ours)
  TIMSK &= ~(_BV(TOIE1) | _BV(OCIE1A) | _BV(OCIE1B) | _BV(ICIE1));
  TIMSK |= _BV(TOIE1);

  // testing: enable PCINT0 interrupt
  PCMSK = 1;  // PCINT0 is bit 0
  GIMSK = _BV(PCIE);
  digitalWrite(Pin_CPU_PWM_A, 1);
}

constexpr uint16_t freq = 100;
constexpr uint32_t cyclesPerSec = 8000000;
constexpr uint32_t cyclesPerLoop = 256;
constexpr uint32_t loopsPerSec = cyclesPerSec / cyclesPerLoop;
constexpr uint32_t maxU16 = 0xffff;
constexpr uint32_t loopsPerWave = loopsPerSec / freq;
constexpr uint32_t increment32 = maxU16 / loopsPerWave;
static_assert(increment32 <= 0xffff);
static_assert(increment32 > 0);

uint16_t increment = uint16_t(increment32);

volatile uint16_t pwm_saw_val_fp8 = 0;

//
//ISR(TIMER0_OVF_vect)
//{
//    val_8 += increment;
//    OCR0A = byte(val_8 >> 8);
//}
ISR(TIMER1_OVF_vect)
{
    pwm_saw_val_fp8 += increment;
    OCR1A = byte(pwm_saw_val_fp8 >> 8);
}

bool bbb = false;
ISR(PCINT_vect)
{
  if (digitalRead(Pin_MUX_Sel0))
    bbb = !bbb;
  digitalWrite(Pin_CPU_PWM_A, bbb);
}


void loop()
{
  // update CPU_Out_x as fast as we can
  // CPU_Out_A :: SqrA xor SqrB
  // CPU_Out_B :: SqrC xor SqrD
  // [revB+ only] CPU_Out_C :: MUX_Sel1 ? (SqrA xor SqrC) : (SqrB xor SqrC)
  
//  const byte pd = PORTD;
//  const bool cpu_a = 1 & ((pd >> Pin_SqrA_In) ^ (pd >> Pin_SqrB_In));
//  const bool cpu_b = 1 & ((pd >> Pin_SqrC_In) ^ (pd >> Pin_SqrD_In));
//  PORTA = cpu_a | (cpu_b << 1);

  const bool sqrA = digitalRead(Pin_SqrA_In);
  const bool sqrB = digitalRead(Pin_SqrB_In);
  digitalWrite(Pin_CPU_Out_A, sqrA ^ sqrB);
  const bool sqrC = digitalRead(Pin_SqrC_In);
  const bool sqrD = digitalRead(Pin_SqrD_In);
  digitalWrite(Pin_CPU_Out_B, sqrC ^ sqrD);

#if !SQR_REV_A
  const byte pb = PORTB;
  const bool mux_sel1 = 1 & (pb >> Pin_MUX_Sel1);
  
  const bool cpu_c = (mux_sel1 ? ((pd >> Pin_SqrA_In) & (pd >> Pin_SqrC_In)) : ((pd >> Pin_SqrB_In) & (pd >> Pin_SqrD_In)));
  PORTD = (pd & ~MskD_CPU_Out_C) | (cpu_c << Pin_CPU_Out_C);
#endif
}
