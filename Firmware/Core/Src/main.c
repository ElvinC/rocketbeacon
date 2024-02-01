/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

CRC_HandleTypeDef hcrc;

SUBGHZ_HandleTypeDef hsubghz;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_SUBGHZ_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */

void SetStandbyXOSC();
void SetPacketTypeLora();
void SetPacketTypeFSK();
uint32_t ComputeRfFreq(double frequencyMhz);
void SetRfFreq(uint32_t rfFreq);
void SetPaLowPower();
void SetPa22dB();
void SetTxPower(int8_t powerdBm);
void SetContinuousWave();
void SetTxInfinitePreamble();
void SetTx(uint32_t timeout);
void SetRx(uint32_t timeout);
void SetModulationParamsLora(const uint8_t params[4]);
void SetModulationParamsFSK(uint32_t bitrate, uint8_t pulseshape, uint8_t bandwidth, uint32_t freq_dev);
void SetPacketParamsLora(uint16_t preamble_length, bool header_fixed, uint8_t payload_length, bool crc_enabled, bool invert_iq);
void FSKBeep(int8_t powerdBm, uint32_t toneHz, uint32_t lengthMs);
void CWBeep(int8_t powerdBm, uint32_t lengthMs);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// standard frequencies. Note: zero indexed.
const double LPD433[69] = {
    433.075, 433.100, 433.125, 433.150, 433.175, 433.200, 433.225, 433.250, 433.275, 433.300, // 1-10
    433.325, 433.350, 433.375, 433.400, 433.425, 433.450, 433.475, 433.500, 433.525, 433.550, // 11-20
    433.575, 433.600, 433.625, 433.650, 433.675, 433.700, 433.725, 433.750, 433.775, 433.800, // 21-30
    433.825, 433.850, 433.875, 433.900, 433.925, 433.950, 433.975, 434.000, 434.025, 434.050, // 31-40
    434.075, 434.100, 434.125, 434.150, 434.175, 434.200, 434.225, 434.250, 434.275, 434.300, // 41-50
    434.325, 434.350, 434.375, 434.400, 434.425, 434.450, 434.475, 434.500, 434.525, 434.550, // 51-60
    434.575, 434.600, 434.625, 434.650, 434.675, 434.700, 434.725, 434.750, 434.775           // 61-69
};

const double PMR446[16] = {
    446.00625, 446.01875, 446.03125, 446.04375, 446.05625, // 1-5
    446.06875, 446.08125, 446.09375, 446.10625, 446.11875, // 6-10
    446.13125, 446.14375, 446.15625, 446.16875, 446.18125, // 11-15
    446.19375 // 16
};

const double FRS[22] = {
    462.5625, 462.5875, 462.6125, 462.6375, 462.6625, 462.6875, 462.7125, 467.5625, 467.5875, 467.6125, // 1-10
    467.6375, 467.6625, 467.6875, 467.7125, 462.5500, 462.5750, 462.6000, 462.6250, 462.6500, 462.6750, // 11-20
    462.7000, 462.7250 // 21-22
};


// letter to morse based on ASCII characters.
// right-terminated by a "1". 1 is dah, 0 is dit.
const uint8_t morse_chars[] = {
    0b11111111,       // Special code for SPACE
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000110,       // - Hyphen sign
    0b10000000,       // N/A
    0b10010100,       // "/" Slash
    0b11111100,       // "0"
    0b01111100,       // "1"
    0b00111100,       // "2"
    0b00011100,       // "3"
    0b00001100,       // "4"
    0b00000100,       // "5"
    0b10000100,       // "6"
    0b11000100,       // "7"
    0b11100100,       // "8"
    0b11110100,       // "9"
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10001100,       // "=" BT prosign/Equal sign
    0b10000000,       // N/A
    0b00110010,       // "?" Question mark
    0b10000000,       // N/A
    0b01100000,       // "A"
    0b10001000,       // "B"
    0b10101000,       // "C"
    0b10010000,       // "D"
    0b01000000,       // "E"
    0b00101000,       // "F"
    0b11010000,       // "G"
    0b00001000,       // "H"
    0b00100000,       // "I"
    0b01111000,       // "J"
    0b10110000,       // "K"
    0b01001000,       // "L"
    0b11100000,       // "M"
    0b10100000,       // "N"
    0b11110000,       // "O"
    0b01101000,       // "P"
    0b11011000,       // "Q"
    0b01010000,       // "R"
    0b00010000,       // "S"
    0b11000000,       // "T"
    0b00110000,       // "U"
    0b00011000,       // "V"
    0b01110000,       // "W"
    0b10011000,       // "X"
    0b10111000,       // "Y"
    0b11001000,       // "Z"
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b10000000,       // N/A
    0b01100000,       // "a"
    0b10001000,       // "b"
    0b10101000,       // "c"
    0b10010000,       // "d"
    0b01000000,       // "e"
    0b00101000,       // "f"
    0b11010000,       // "g"
    0b00001000,       // "h"
    0b00100000,       // "i"
    0b01111000,       // "j"
    0b10110000,       // "k"
    0b01001000,       // "l"
    0b11100000,       // "m"
    0b10100000,       // "n"
    0b11110000,       // "o"
    0b01101000,       // "p"
    0b11011000,       // "q"
    0b01010000,       // "r"
    0b00010000,       // "s"
    0b11000000,       // "t"
    0b00110000,       // "u"
    0b00011000,       // "v"
    0b01110000,       // "w"
    0b10011000,       // "x"
    0b10111000,       // "y"
    0b11001000        // "z"
};

void LED_on() {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}
void LED_off() {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

// https://en.wikipedia.org/wiki/Morse_code#/media/File:International_Morse_Code.svg
uint32_t morse_unit_ms = 70;
int8_t morse_power = 10;

void play_morse_char(uint8_t ascii_letter, bool use_cw) {
    uint8_t morse_code = 0b11111111;
    if (ascii_letter > 31 && ascii_letter < 123) {
        morse_code = morse_chars[ascii_letter - 32];
    }

    // space
    if (morse_code == 0b11111111) {
        if (use_cw) {
            HAL_Delay(morse_unit_ms);
        } else {
            //FSKBeep(morse_power, 750, morse_unit_ms);
            HAL_Delay(morse_unit_ms);
        }
        return;
    }
    uint8_t terminatelen = 0;
    for (uint8_t idx = 0; idx < 8; idx++) {
        if (morse_code & (1 << idx)) {
            terminatelen = idx;
            break;
        }
    }

    for (uint8_t i = 7; i > terminatelen; i--) {
    	LED_on();
        if (morse_code & (1 << i)) {
            // make dat
            //printf("-");
            if (use_cw) {
                CWBeep(morse_power, morse_unit_ms * 3);
            } else {
                FSKBeep(morse_power, 400, morse_unit_ms * 3);
            }
        } else {
            // make dit
            //printf(".");
            if (use_cw) {
                CWBeep(morse_power, morse_unit_ms);
            } else {
                FSKBeep(morse_power, 400, morse_unit_ms);
            }
        }
        LED_off();

        // Make delay.
        if (use_cw) {
            HAL_Delay(morse_unit_ms);
        } else {
            HAL_Delay(morse_unit_ms);
            //CWBeep(morse_power, morse_unit_ms);
        }
    }
}

void play_morse_word(uint8_t* letters, uint8_t len, bool use_cw) {
    for (uint8_t i = 0; i < len; i++) {
        play_morse_char(letters[i], use_cw);

        // Space between letters
        if (use_cw) {
            HAL_Delay(morse_unit_ms * 3);
        } else {
            //CWBeep(morse_power, morse_unit_ms);
            HAL_Delay(morse_unit_ms * 3);
        }
    }
}


void rx_test(bool is_tx) {
  LED_on();
  HAL_Delay(100);
  LED_off();
  SetStandbyXOSC();
  HAL_Delay(1);
  SetPacketTypeLora();
  HAL_Delay(1);
  SetRfFreq(ComputeRfFreq(433.250));

  //SetPaLowPower(); // For powers up to 14 dBm
  SetPa22dB(); // Allows powers up to 22 dBm
  HAL_Delay(1);
  SetTxPower(-9);
  HAL_Delay(1);
  uint8_t LORA_SF12_BW62_CR45[4] = {0x0C, 0x03, 0x01, 0x00};
  SetModulationParamsLora(LORA_SF12_BW62_CR45);
  HAL_Delay(1);

  SetPacketParamsLora(4, true, 4, true, false); // Send 4 bytes
  HAL_Delay(1);
  uint8_t buffer[4] = {0x00, 0x00, 0x00, 0x00};
  HAL_SUBGHZ_WriteBuffer(&hsubghz, 0, buffer, 4);


  while (1) {
      if (is_tx) {
          buffer[0] = 0x04; // Led ON
          HAL_SUBGHZ_WriteBuffer(&hsubghz, 0, buffer, 4);
          HAL_Delay(1);
          SetTx(0);
          LED_on();
          HAL_Delay(1000);

      } else {
          SetRx(0);
          HAL_Delay(100);
          HAL_SUBGHZ_ReadBuffer(&hsubghz, 0, buffer, 4);
          if (buffer[0] == 0x04) {
              LED_on();

          } else {
              LED_off();
          }
      }
  }

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_SUBGHZ_Init();
  MX_USART2_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  // ==========================================
  //      START CHANGING SETTINGS HERE
  // ==========================================

  // Frequency setting in MHz
  // Can be a "standard" frequency, e.g. center_freq = LPD433[20-1]; sets it to LPD433 channel 20 (zero indexed) = 433.550 MHz
  rx_test(true);
  double center_freq = 433.225;

  // max power in dBm, valid values between -9 and 22;
  // If using coin cell batteries, values above 16 dBm are not recommended without testing due to current limitations.
  // Alternatively use external LiPo power
  int maxPower = 10;

  double freq_correction = 0.99999539941; // For tuning frequency

  bool CallsignTF = false;
  uint8_t callsign[] = "nocall";

  int CallsignPeriod = 300; // seconds

  // Continuous Wave (CW) settings
  bool CWbeep = false;
  int CWbeepcount = 4;
  bool CWHigh2Low = true; //if true, start with highest power beep, decrease from there
  int CWbeepOffset = 150; //Hertz
  int CWbeepIndLength = 20; //milliseconds
  int CWbeepGapLength = 20; //milliseconds


  // Frequency-shift keying (FSK) settings. Can be heard with FM receivers
  bool FSKbeep = true;
  int FSKbeepcount = 3;
  bool FSKHigh2Low = false; //if true, start with highest power beep, decrease from there. When true, it maximizes the power of the highest power beep, but makes it harder to read a visible signal indicator on later beeps.
  int FSKbeepIndLength = 250; //milliseconds
  int FSKbeepGapLength = 50; //milliseconds
  bool CustomFSKtones = false;
  int CustomFSKfrequencies[] = {320, 400, 480, 640}; // Must match the length exactly!

  int Period = 2000; //milliseconds
  int StartupWait = 5000; // initial start. A few seconds to allow coin cell batteries to recover in case of brownout resets.

  // ==========================================
  //      STOP CHANGING SETTINGS HERE
  // ==========================================


  //EE_Status ee_status = EE_OK;
  LED_on();
  HAL_Delay(StartupWait);
  LED_off();
  SetStandbyXOSC();
  HAL_Delay(1);
  SetPacketTypeLora();
  HAL_Delay(1);

  //SetPaLowPower(); // For powers up to 14 dBm
  SetPa22dB(); // Allows powers up to 22 dBm
  HAL_Delay(1);
  SetTxPower(-9);
  HAL_Delay(1);

  SetPacketTypeFSK();

  SetModulationParamsFSK(2000,    0x09,     0x1E,      2500);


  //  int FSKtones[12] = {400, 350, 300, 250, 200, 150, 1600, 2000, 2400, 3200, 4000, 4800};
  int FSKtones[FSKbeepcount];
  SetRfFreq(ComputeRfFreq(center_freq * freq_correction));

  memset(FSKtones, 0, sizeof(FSKtones));
  for(int i=0; i<FSKbeepcount; i++){
      int base = floor(i/3);
      int mplr = 1<<base;
      FSKtones[i] = 320*(1 + 0.25*(i-3*floor(i/3)))*mplr;
  }
  if (CustomFSKtones){
      memcpy(FSKtones, CustomFSKfrequencies, sizeof(FSKtones));
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /*
  while (1) {
      // Emulate comspec beacon at 40 mW
      LED_on();
      CWBeep(16, 50);
      LED_off();
      HAL_Delay(1000);
  }*/

  int loopCounter = floor(CallsignPeriod * 1000 / Period);

  int gap = Period;
  if(FSKbeep) {
	  gap = gap-(FSKbeepIndLength * FSKbeepcount);
	  gap = gap-(FSKbeepGapLength * (FSKbeepcount-1));
  }
  if(CWbeep) {
	  gap = gap-(CWbeepIndLength * CWbeepcount);
	  gap = gap-(CWbeepGapLength * (CWbeepcount - 1));
  }
  if (CWbeep && FSKbeep) {
	  gap = gap /2;
  }

  int FSKTXpwrs[FSKbeepcount];
  memset(FSKTXpwrs, 0, sizeof(FSKTXpwrs));
  if(FSKbeep){
	  int stepsize = 0;
	  if (FSKbeepcount > 1){
		  stepsize = (31 - 22 + maxPower)/(FSKbeepcount - 1);
	  }
	  for(int i=0; i<FSKbeepcount; i++){
		 FSKTXpwrs[i] = maxPower - stepsize * i;
	  }
  }
  int CWTXpwrs[CWbeepcount];
  memset(CWTXpwrs, 0, sizeof(CWTXpwrs));
  if(CWbeep){
	  int stepsize = 0;
	  if (CWbeepcount > 1){
		  stepsize = (31 - 22 + maxPower)/(CWbeepcount - 1);
	  }
	  for(int i=0; i<CWbeepcount; i++){
		 CWTXpwrs[i] = maxPower - stepsize * i;
	  }
  }

  while (1)
  {
	  if(CallsignTF)
	  {
		  play_morse_word(callsign, sizeof(callsign)-1, false);
	  }

      LED_off();

      HAL_Delay(gap);
      for (int i=0; i<loopCounter-1; i++)
      {
    	  SetRfFreq(ComputeRfFreq(center_freq * freq_correction));
    	  // FSK beeps
    	  if(FSKbeep){
    		  if(FSKHigh2Low){
    			  for (int j=0; j<FSKbeepcount; j++){
    				  LED_on();
    				  FSKBeep(FSKTXpwrs[j], FSKtones[j], FSKbeepIndLength);
    				  LED_off();
    				  HAL_Delay(FSKbeepGapLength);
    			  }
    		  }
    		  else {
    			  for (int j=0; j<FSKbeepcount; j++){
    				  LED_on();
    				  FSKBeep(FSKTXpwrs[FSKbeepcount-1-j], FSKtones[j], FSKbeepIndLength);
    				  LED_off();
    				  HAL_Delay(FSKbeepGapLength);
    			  }
    		  }
    		  HAL_Delay(gap);
    	  }
    	  if(CWbeep){
    		  if(CWHigh2Low){
    			  for (int j=0; j<CWbeepcount; j++){
    				  LED_on();
    				  SetRfFreq(ComputeRfFreq((center_freq + 0.000001*CWbeepOffset*j) * freq_correction));
    				  CWBeep(CWTXpwrs[j], CWbeepIndLength);
    				  LED_off();
    				  HAL_Delay(CWbeepGapLength);
    			  }
    		  }
    		  else{
    			  for (int j=0; j<CWbeepcount; j++){
    				  LED_on();
    				  SetRfFreq(ComputeRfFreq((center_freq + 0.000001*CWbeepOffset*j) * freq_correction));
    				  CWBeep(CWTXpwrs[CWbeepcount-1-j], CWbeepIndLength);
    				  LED_off();
    				  HAL_Delay(CWbeepGapLength);
    			  }
    		  }
    		  HAL_Delay(gap);
    	  }
    	  // CW beeps
/*    	  LED_on();
          FSKBeep(-9, 400, 150);
          LED_off();

          HAL_Delay(100);

          LED_on();
          FSKBeep(7, 500, 150);
          LED_off();

          HAL_Delay(100);

          LED_on();
          FSKBeep(22, 600, 200);
          LED_off();

          HAL_Delay(1000);

          LED_on();
          CWBeep(22, 25);
          LED_off();
          HAL_Delay(25);
          SetRfFreq(ComputeRfFreq((center_freq + 0.0002) * freq_correction));
          LED_on();
          CWBeep(11, 25);
          LED_off();
          HAL_Delay(25);
          SetRfFreq(ComputeRfFreq((center_freq + 0.0004) * freq_correction));
          LED_on();
          CWBeep(1, 25);
          LED_off();
          HAL_Delay(25);
          SetRfFreq(ComputeRfFreq((center_freq + 0.0006) * freq_correction));
          LED_on();
          CWBeep(-9, 25);
          LED_off();

          HAL_Delay(2000);
*/
      }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV16;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the HSE Prescaler
  */
  __HAL_RCC_HSE_DIV2_ENABLE();
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief SUBGHZ Initialization Function
  * @param None
  * @retval None
  */
static void MX_SUBGHZ_Init(void)
{

  /* USER CODE BEGIN SUBGHZ_Init 0 */

  /* USER CODE END SUBGHZ_Init 0 */

  /* USER CODE BEGIN SUBGHZ_Init 1 */

  /* USER CODE END SUBGHZ_Init 1 */
  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_8;
  if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SUBGHZ_Init 2 */

  /* USER CODE END SUBGHZ_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CONF_440_Pin CONF_868_Pin */
  GPIO_InitStruct.Pin = CONF_440_Pin|CONF_868_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT_Pin */
  GPIO_InitStruct.Pin = BOOT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void SetStandbyXOSC() {
    uint8_t txbuf[2] = {0x80, 0x01};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetPacketTypeLora() {
    uint8_t txbuf[2] = {0x8A, 0x01};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetPacketTypeFSK() {
    uint8_t txbuf[2] = {0x8A, 0x00};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

uint32_t ComputeRfFreq(double frequencyMhz) {
    return (uint32_t)(frequencyMhz * 1048576L); //2^25/(32e6)
}

void SetRfFreq(uint32_t rfFreq) {
    uint8_t txbuf[5] = {0x86, (rfFreq & 0xFF000000) >> 24, (rfFreq & 0x00FF0000) >> 16, (rfFreq & 0x0000FF00) >> 8, rfFreq & 0x000000FF};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetPaLowPower() {
    // set Pa to 14 dB.
    uint8_t txbuf[5] = {0x95, 0x02, 0x02, 0x00, 0x01};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetPa22dB() {
    // set Pa to the highest 22 dBm
    uint8_t txbuf[5] = {0x95, 0x04, 0x07, 0x00, 0x01};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetTxPower(int8_t powerdBm) {
    // Between -9 and 22
    int8_t power = powerdBm < -9 ? -9 : ((powerdBm > 22) ? 22 : powerdBm);
    uint8_t txbuf[3] = {0x8E, (uint8_t) power, 0x02};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetContinuousWave() {
    uint8_t txbuf[1] = {0xD1};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf, 0);
}

void SetTxInfinitePreamble() {
    uint8_t txbuf[1] = {0xD2};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf, 0);
}

void SetTx(uint32_t timeout) {
    // Timeout * 15.625 µs
    uint8_t txbuf[4] = {0x83, (timeout & 0x00FF0000) >> 16, (timeout & 0x0000FF00) >> 8, timeout & 0x000000FF};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetRx(uint32_t timeout) {
    // Timeout * 15.625 µs
    // 0x000000 No timeout. Rx Single mode
    // 0xFFFFFF Rx Continuous mode. The device remains in RX mode until the host sends a command to change the operation mode
    uint8_t txbuf[4] = {0x82, (timeout & 0x00FF0000) >> 16, (timeout & 0x0000FF00) >> 8, timeout & 0x000000FF};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetModulationParamsLora(const uint8_t params[4]) {
    uint8_t txbuf[5] = {0x8B, params[0], params[1], params[2], params[3]};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetModulationParamsFSK(uint32_t bitrate, uint8_t pulseshape, uint8_t bandwidth, uint32_t freq_dev) {
    uint32_t BR = 32 * 32e6 / bitrate;
    uint32_t fdev = (uint32_t) (freq_dev * 1.048576L); // 2^25/32e6 = 1.048576
    uint8_t txbuf[9] = {0x8B, (BR & 0x00FF0000) >> 16, (BR & 0x0000FF00) >> 8, BR & 0x000000FF, pulseshape, bandwidth, (fdev & 0x00FF0000) >> 16, (fdev & 0x0000FF00) >> 8, fdev & 0x000000FF};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}

void SetPacketParamsLora(uint16_t preamble_length, bool header_fixed, uint8_t payload_length, bool crc_enabled, bool invert_iq) {
    uint8_t txbuf[7] = {0x8C, (uint8_t)((preamble_length >> 8) & 0xFF), (uint8_t)(preamble_length & 0xFF),
                        (uint8_t) header_fixed, payload_length, (uint8_t) crc_enabled, (uint8_t) invert_iq};

    HAL_SUBGHZ_ExecSetCmd(&hsubghz, txbuf[0], txbuf+1, sizeof(txbuf)-1);
}
/*
void WriteBuffer(uint8_t offset, uint8_t *data, uint8_t len) {
    HAL_SUBGHZ_WriteBuffer(&hsubghz, offset, data, len);
}

void ReadBuffer(uint8_t offset, uint8_t *data, uint8_t len) {
    HAL_SUBGHZ_ReadBuffer(&hsubghz, offset, data, len);
}
*/
void FSKBeep(int8_t powerdBm, uint32_t toneHz, uint32_t lengthMs) {
    // assume in standbyXOSC already.
    HAL_Delay(1);
    SetTxPower(powerdBm);
    SetModulationParamsFSK(toneHz*2,    0x09,     0x1E,      2500);
    HAL_Delay(5);
    SetTxInfinitePreamble();
    HAL_Delay(lengthMs);
    SetStandbyXOSC();
    HAL_Delay(5);
}

void CWBeep(int8_t powerdBm, uint32_t lengthMs) {
    HAL_Delay(1);
    SetTxPower(powerdBm);
    HAL_Delay(5);
    SetContinuousWave();
    HAL_Delay(lengthMs);
    SetStandbyXOSC();
    HAL_Delay(5);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
