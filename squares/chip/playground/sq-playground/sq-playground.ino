
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

constexpr byte BitD_CPU_Out_C = 0;
constexpr byte MskD_CPU_Out_C = 1 << BitD_CPU_Out_C;

#else
#error unknown board revision
#endif

constexpr byte BitD_SqrA_In = 3;
constexpr byte BitD_SqrB_In = 4;
constexpr byte BitD_SqrC_In = 5;
constexpr byte BitD_SqrD_In = 6;
constexpr byte MskD_SqrA_In = 1 << BitD_SqrA_In;
constexpr byte MskD_SqrB_In = 1 << BitD_SqrB_In;
constexpr byte MskD_SqrC_In = 1 << BitD_SqrC_In;
constexpr byte MskD_SqrD_In = 1 << BitD_SqrD_In;

static_assert(Pin_MUX_Sel1 == PIN_PB1);
constexpr byte BitB_MUX_Sel1 = 1;
constexpr byte MskB_MUX_Sel1 = 1 << BitB_MUX_Sel1;



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


  // CPU_PWM_A :: TIMER0 / PWM0 for percussive envelope, triggered by MUX_Partial_CD (INT0)
  TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(WGM01) | _BV(WGM00);
  TCCR0B = _BV(CS00);
  TCNT0 = 0;
  OCR0A = 0x00;
  
  // CPU_PWM_B :: TIMER1 / PWM1A for sawtooth tracking freq of SqrC, hardsynced to SqrD
  // fast PWM 8bit, superfast clock
  TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
  TCNT1 = 0;
  OCR1A = 0x0080;

  byte sreg = SREG;
  cli();
  
  // enable TIMER0_OVF & TIMER1_OVF interrupts
  TIMSK = _BV(TOIE0) |_BV(TOIE1);

//  // enable int0 interrupt to trigger env
//  MCUCR &= 0x0f;
//  MCUCR |= _BV(ISC01) | _BV(ISC00);
//  GIMSK = _BV(INT0);
  // enable pcint1 interrupt to trigger env
  PCMSK = 1 << BitB_MUX_Sel1;
  GIMSK = _BV(PCIE);

  SREG = sreg;
}

constexpr uint16_t freq = 100;
constexpr uint32_t cyclesPerSec = 8000000;
constexpr uint32_t cyclesPerLoop = 256;
constexpr uint32_t loopsPerSec = cyclesPerSec / cyclesPerLoop;
constexpr uint32_t maxU16 = 0xffff;
constexpr uint32_t loopsPerWave = loopsPerSec / freq;
constexpr uint32_t increment32 = maxU16 / loopsPerWave;
static_assert(increment32 <= 0xffff);
static_assert(increment32 > 0);   // == 209.5 for 100Hz

uint16_t pwm_saw_inc_fx8 = uint16_t(increment32);

volatile uint16_t pwm_saw_val_fx8 = 0;
volatile uint32_t pwm_accum_saw_val_fx8 = 0;

volatile byte pwm_env_skip_count = 0;
volatile byte pwm_env_phase = 0;
volatile byte pwm_env_thresh = 0;

ISR(TIMER0_OVF_vect)
{
    if (pwm_env_skip_count > 0)
    {
        --pwm_env_skip_count;
        return;
    }
    pwm_env_skip_count = 50;

    switch (pwm_env_phase)
    {
      case 0:
        pwm_env_thresh += 16;
        if (pwm_env_thresh > 190)
          pwm_env_phase = 1;
        break;

      case 1:
        pwm_env_thresh += 3;
        if (pwm_env_thresh > 240)
          pwm_env_phase = 2;
        break;

      default:
        if (pwm_env_thresh < 0xff)
          pwm_env_thresh += 1;
    }
 
    OCR0A = pwm_env_thresh;
}

static inline void trigger_env()
{
    pwm_env_thresh = 0;
    pwm_env_phase = 0;
    TCNT0 = 0;
}

ISR(TIMER1_OVF_vect)
{
  uint16_t val_fx8 = pwm_saw_val_fx8;
  val_fx8 += pwm_saw_inc_fx8;
  
  // if we wrapped, make sure to accumulate the 0xffff total increment we just used
  if (val_fx8 < pwm_saw_inc_fx8)
    pwm_accum_saw_val_fx8 += 0xffff;
    
  OCR1A = byte(val_fx8 >> 8);
  pwm_saw_val_fx8 = val_fx8;
}


// fired by PCINT1 (MUX_Sel1) changing
static byte lastPortB = 0;
ISR(PCINT_vect)
{
  if (PINB & MskB_MUX_Sel1)
    trigger_env();
}

// fired by Pin_Partial_CD changing
// trigger percussive envelope
ISR(INT0_vect)
{
    trigger_env();
}


bool oldSqrC = false;
bool oldSqrD = false;

void loop()
{
  // update CPU_Out_x as fast as we can
  // CPU_Out_A :: SqrA xor SqrB
  // CPU_Out_B :: SqrC xor SqrD
  // [revB+ only] CPU_Out_C :: MUX_Sel1 ? (SqrA xor SqrC) : (SqrB xor SqrC)
  
  const byte pd = PIND;
  const bool cpu_a = 1 & ((pd >> BitD_SqrA_In) ^ (pd >> BitD_SqrB_In));
  const bool cpu_b = 1 & ((pd >> BitD_SqrC_In) ^ (pd >> BitD_SqrD_In));
  PORTA = cpu_a | (cpu_b << 1); //  note that PA2 is nRESET, but as that's set to be an input, this is fine

#if !SQR_REV_A
  const bool mux_sel1 = PINB & _BV(BitB_MUX_Sel1);
  const bool cpu_c = (mux_sel1 ? ((pd >> BitD_SqrA_In) & (pd >> BitD_SqrC_In)) : ((pd >> BitD_SqrB_In) & (pd >> BitD_SqrD_In)));
  if (cpu_c)
    PORTD |= BitD_CPU_Out_C;
  else
    PORTD &= ~BitD_CPU_Out_C;
#endif

  // apply hardsync for PWM_B
  const bool sqrD = (pd & _BV(BitD_SqrD_In)) != 0;
  if (sqrD && !oldSqrD)
  {
    byte sreg = SREG;
    cli();
    
    // make sure to remember any increment we used so we can manage tuning
    pwm_accum_saw_val_fx8 += pwm_saw_val_fx8;
    pwm_saw_val_fx8 = 0;
    
    SREG = sreg;
  }
  oldSqrD = sqrD;

  // track tuning for PWM_B against sqrC
  const bool sqrC = (pd & _BV(BitD_SqrC_In)) != 0;
  if (sqrC != oldSqrC)
  {
    // ideally, pwm_accum_saw_val_fx8+pwm_saw_val_fx8 == 0xffff at this moment
    // adjust our increment to try and make that happen
    byte sreg = SREG;
    cli();
    int32_t error = (pwm_accum_saw_val_fx8 + pwm_saw_val_fx8);
    pwm_accum_saw_val_fx8 = 0;
    SREG = sreg;

    error -= 0xffff;

    if (error > 0 && pwm_saw_inc_fx8 > 1)
    {
      // increment is too high, so we adjust down
      uint16_t d_inc = 1;
      if ((error >> 8) > 100)
        d_inc = 20;
      else if ((error >> 8) > 10)
        d_inc = 4;
      else if ((error >> 8) > 3)
        d_inc = 2;

      sreg = SREG;
      cli();
      pwm_saw_inc_fx8 -= d_inc;
      SREG = sreg;
    }
    else if (error < 0 && pwm_saw_inc_fx8 < 0xfff8)
    {
      // increment is too low, so adjust up
      error = -error;
      uint16_t d_inc = 1;
      if ((error >> 8) > 100)
        d_inc = 20;
      else if ((error >> 8) > 10)
        d_inc = 4;
      else if ((error >> 8) > 3)
        d_inc = 2;
      
      sreg = SREG;
      cli();
      pwm_saw_inc_fx8 += d_inc;
      SREG = sreg;
    }
  }
  oldSqrC = sqrC;
}
