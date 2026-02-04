#include "SignalManager.h"

void SignalManager::initialize()
{
  setRegisters();
  setPins();
}

void SignalManager::setPins()
{
  pinMode(VOLTAGE_MEASUREMENT_PIN, INPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  analogWrite(MOTOR_PWM_PIN, MOTOR_PWM_VALUE);
  pinMode(MOTOR_ENABLE_FORWARD_PIN, OUTPUT);
  digitalWrite(MOTOR_ENABLE_FORWARD_PIN, LOW);
  pinMode(MOTOR_ENABLE_BACKWARD_PIN, OUTPUT);
  digitalWrite(MOTOR_ENABLE_BACKWARD_PIN, LOW);

  pinMode(FREQUENCY_SET_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);

  pinMode(ENC_SW_PIN, INPUT_PULLUP);
}

void SignalManager::setRegisters()
{
  pmc_enable_periph_clk(ID_ADC);
  
  ADC->ADC_MR = ADC_MR_PRESCAL(4);
  ADC->ADC_MR |= ADC_MR_STARTUP_SUT64;
  ADC->ADC_MR |= ADC_MR_TRACKTIM(15);
  ADC->ADC_MR |= ADC_MR_TRANSFER(1);
  
  ADC->ADC_CHER = ADC_CHER_CH7;
  
  // Enable ADC interrupt
  ADC->ADC_IER = ADC_IER_DRDY;
  NVIC_EnableIRQ(ADC_IRQn);
  
  Serial.println("ADC initialized with interrupts");
  
  // === TIMER SETUP (TC0 Channel 0) ===
  // Timer will trigger ADC at controlled rate
  pmc_enable_periph_clk(ID_TC0);
  
  TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
  
  // TIMER_CLOCK1 = MCK/2 = 42MHz
  // For 50kHz sampling: RC = 42MHz / 100kHz = 420
  uint32_t rc = 420;  // 100kHz sampling rate
  TC_SetRC(TC0, 0, rc);
  
  // Enable timer interrupt
  TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  TC0->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(TC0_IRQn);
  
  Serial.print("Timer configured for ~");
  Serial.print(42000000.0 / rc / 1000.0, 1);
  Serial.println(" kHz sampling");
  
  pmc_enable_periph_clk(PWM_INTERFACE_ID);
  
  PWMC_ConfigureClocks(42000000, 0, VARIANT_MCK); 
  PWMC_ConfigureChannel(PWM, 7, PWM_CMR_CPRE_CLKA, 0, 0);
  PWMC_SetPeriod(PWM, 7, topValue_);
  PWMC_SetDutyCycle(PWM, 7, 0);
  PWMC_EnableChannel(PWM, 7);
  TC_Start(TC0, 0);
  delay(100);
}

void SignalManager::setUpOperation(DataHolder &data)
{
  switch(data.mode)
  {
    case DataHolder::Modes::AC:
      setFrequency(data.desiredFrequency);
      setVoltage(data.desiredVoltage, data.handler);
      data.desiredVoltage = fabs(data.desiredVoltage);
    break;
    case DataHolder::Modes::DC:
      setVoltage(fabs(data.desiredVoltage), data.handler);
      if (data.desiredVoltage < 0.0f)
        setDutyCycle(100);
      else
        setDutyCycle(0);
    break;
    case DataHolder::Modes::VCCS:
      setVoltage(data.desiredVoltage, data.handler);
      if (data.desiredVoltage < 0.0f)
        setDutyCycle(0);
      else
        setDutyCycle(100);
    break;
    default:
    break;
  }
}

void SignalManager::stopOperation(DataHolder &data)
{
  setVoltage(0, data.handler);
  setDutyCycle(0);
  digitalWrite(GREEN_LED_PIN, LOW);
  startTime_ = 0;
}

void SignalManager::operateMotor(uint64_t currentMs, bool &enableMotor, const float &wireLength, const uint8_t &motorEnablePin, bool &blockSpiTransfer)
{
  if (currentMs - motorLastActivationTime_ < MOTOR_TOGGLE_PREVENTION_PERIOD)
    return;
  if (motorStartTime_ == 0)
    motorStartTime_ = currentMs;

  if ((currentMs - motorStartTime_) <= wireLength * 20)
  {
    blockSpiTransfer = true;
    digitalWrite(motorEnablePin, HIGH);
  }
  else
  {
    blockSpiTransfer = false;
    digitalWrite(motorEnablePin, LOW);
    enableMotor = false;
    motorStartTime_ = 0;
    motorLastActivationTime_ = currentMs;
  }
}

void SignalManager::setVoltage(const float &voltage, ControllersHandler *handler)
{
  handler->dac.fastWriteA(constrain(voltage * 819, 0, 4095));
}

void SignalManager::setFrequency(uint16_t frequency, const uint8_t &dutyCycle)
{
// Pin 6 = PC24 = PWM Channel 7
  
  // Enable PWM peripheral clock
  pmc_enable_periph_clk(ID_PWM);
  
  // Configure Pin 6 (PC24) for PWM (Peripheral B)
  PIO_Configure(PIOC, PIO_PERIPH_B, PIO_PB24, PIO_DEFAULT);
  
  uint32_t channel = 7;

  frequency = constrain(frequency, 10, 1000);

  const uint32_t prescalers[][3] = {
    {320, 4, 2},
    {160, 8, 3},
    {80, 16, 4},
    {40, 32, 5},
    {20, 64, 6},
    {10, 256, 8}
  };
  
  uint32_t prescaler_ = 256;
  uint32_t prea = 8;
  
  for (int i = 0; i < 6; i++) {
    if (frequency >= prescalers[i][0]) {
      prescaler_ = prescalers[i][1];
      prea = prescalers[i][2];
      break;
    }
  }

  pwmPeriod_ = (84000000 / prescaler_) / frequency;
  
  // Configure PWM clock A
  PWM->PWM_CLK = PWM_CLK_PREA(prea) | PWM_CLK_DIVA(1);
  
  // Disable channel first
  PWM->PWM_DIS = (1 << channel);
  
  // Wait for channel to be disabled
  while ((PWM->PWM_SR & (1 << channel)) != 0);
  
  // Configure channel to use clock A
  PWM->PWM_CH_NUM[channel].PWM_CMR = PWM_CMR_CPRE_CLKA;
  
  // Set period
  PWM->PWM_CH_NUM[channel].PWM_CPRD = pwmPeriod_;
  
  // Enable the channel
  PWM->PWM_ENA = (1 << channel);
  
  // Set initial duty cycle
  setDutyCycle(dutyCycle);
}

void SignalManager::setDutyCycle(uint8_t percentage)
{
  uint32_t channel = 7;

  percentage = constrain(percentage, 0, 100);

  uint32_t duty = (pwmPeriod_ * percentage) / 100;

  PWM->PWM_CH_NUM[channel].PWM_CDTYUPD = duty;
}

void SignalManager::indicateLedStatus(DataHolder &data, const uint64_t &currentMs)
{
  static bool ledOffPowerOn = false;

  if(!data.operating && ledOffPowerOn)
    stopLedTimer_ = currentMs;

  if (data.waiting)
  {
    digitalWrite(GREEN_LED_PIN, HIGH);
    blink(currentMs);
  }
  else if (data.operating && !startTime_)
  {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    ledOffPowerOn = true;
    startTime_ = currentMs;
  }
  else if(!data.operating && ledOffPowerOn && currentMs - stopLedTimer_ <= offLedTimeout_)
  {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledOffPowerOn = false;
    startTime_ = 0;
  }
  else if(!data.operating && currentMs - stopLedTimer_ > offLedTimeout_)
  {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

void SignalManager::blink(const uint64_t &currentMs)
{
  if (currentMs - stopLedTimer_ <= blinkInterval_)
    digitalWrite(RED_LED_PIN, HIGH);
  else if (currentMs - stopLedTimer_ - blinkInterval_ <= blinkInterval_)
    digitalWrite(RED_LED_PIN, LOW);
  else
    stopLedTimer_ = currentMs;
}

bool SignalManager::handleEncoder(DataHolder *data, int32_t newPosition, const uint64_t &currentMs)
{
  if(!digitalRead(ENC_SW_PIN) && (currentMs - lastEncoderUseTime_) > encoderLockTime_)
  {
    data->useButton = true;
    lastEncoderUseTime_ = currentMs;
    return true;
  }
  else
  {
    updatePosition(data, newPosition);
    return true;
  }
  return false;
}

void SignalManager::updatePosition(DataHolder *data, int32_t newPosition)
{
  if (newPosition % 4 == 0 && newPosition != encoderPosition_)
  {
    data->switchButton = newPosition > encoderPosition_ ? BUTTON_NEXT : BUTTON_PREV;
    encoderPosition_ = newPosition;
  }
}
