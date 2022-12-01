/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcdtp.h"
#include "xpt2046.h"
#include "UI.h"
#include "UI_buttons.h"
#include "rtc.h"
#include "dht11.h"
#include "printf.h"
#include "timer.h"
#include "pet.h"
#include "buzzer.h"
#include "alarm.h"
#include "esp8266.h"
#include "bsp_usart.h"
#include "ring_buffer.h"
#include "wifi_info.h"
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
ADC_HandleTypeDef hadc1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FSMC_Init(void);
void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * Variables Initializations:
 * DHT11_data -> Receive data from DHT11
 * Coordinate -> Receive touch coordinate of LCD
 * real_time  -> Receive data from rtc
 *
 * mode, mode_new, render_done -> Flow control for UI
 */
DHT11_datastruct DHT11_data = {0}; // read data by calling -  DHT11_ReadData(&DHT11_data);
strType_XPT2046_Coordinate Coordinate = {0}; //Coordinate of LCD
TimeStamp real_time = {0}; //read real time data by calling - get_TimeStamp(&real_time);
uint32_t lastupdate_raw,  lastdrink_raw = 0;
uint32_t next = 0; //Next drink schedule time
int tilnext = 0; // time till next drink
uint32_t exertime = 0;//Time Set By User (Volatile)
uint32_t exertime_fixed = 0;//Time Set By User (Fixed)
uint32_t exertimer = 0;//Target time of exercise alarm
int sec = 0;


/*
 * Status Variables
 * petStats -> Current Image Of Pet
 * DHT11_SCHEDULE_FLAG -> Read DHT11 Data when Flag = 1
 */
const unsigned char * petStats = normal;
uint8_t DHT11_SCHEDULE_FLAG = 1;
uint8_t USART_GET_TIME_FLAG = 0;
uint8_t USART_WATER_FLAG = 0; //Upload drink water info
uint8_t USART_EXERCISE_FLAG = 0;//Upload exercise info
uint8_t ALARM_TIMES_UP_RENDER_FLAG = 0;
uint8_t EXER_TIMER_SET_FLAG = 0;
uint8_t ADC_DARKMODE_TOGGLE = 0;
char USART_DATE_BUFFER[15];
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
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  	/*
  	 * Module Initializations:
  	 * rtc.c
  	 * XPT2046.c
  	 * LCD
  	 * ADC (Photoresistor)
  	 * ESP8266
  	 */
	RTC_Init(&hrtc);
	macXPT2046_CS_DISABLE();
	LCD_INIT();
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	uint32_t value = HAL_ADC_GetValue(&hadc1);
	DEBUG_USART_Config();
	extern uint8_t esp8266_step_flag;
	esp8266_step_flag = 0;
	Ringbuf_init();
	RTC_Get();
	get_TimeStamp(&real_time);
	sec = real_time.rsec;
	//Flow control of UI
	uint8_t mode = 0; //Current Mode: Mode 0 = Home, Mode 1 = Drink Water, Mode 3 = Pet
	uint8_t mode_new = 0; //To Determine Whether A Mode is Updated
	uint8_t render_done=0;//Set To 1 Whenever Screen Need to Update
	uint8_t pet_update=0; //Set To 1 Whenever Pet Image Need to Update


	//Calibration of TouchPad
	while( ! XPT2046_Touch_Calibrate () );
	alarm_update_next();
	alarm_update_last();
	exertimer = RTC_raw();
	DHT11_ReadData(&DHT11_data);

	/*
	 * Scheduling Event
	 * Tim3: Water Alarm
	 * Tim5: Update Temperature/ Weather Data
	 */
	TIMER_INIT();
	timer_min(1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	
  while (1)
  {
	  XPT2046_Get_TouchedPoint(&Coordinate,
	  			&strXPT2046_TouchPara);
	  HAL_ADC_Start(&hadc1);
	  if (real_time.rsec % 5==0 && ADC_DARKMODE_TOGGLE){
		  value = HAL_ADC_GetValue(&hadc1);
	  }
	  //!!Only For UI that is changing every moment, Just For Testing, Need Refactoring Later
	  if(mode==0) {
		  RTC_Get();
		  UI_Home_Display_Date(real_time.ryear, real_time.rmon, real_time.rday);
		  UI_Home_Display_Time(real_time.rhour, real_time.rmin, real_time.rsec);
	  }
	  else if (mode == 3){
		  RTC_Get();
		  UI_Stats_Update();
	  }
	  else if (mode == 6){
	  		  UI_Set_Update();
	  	  }
	  get_TimeStamp(&real_time);
		sec = real_time.rsec;
	  if (petStats != sleep1 && petStats != sleep2 && mode==0 && next > RTC_raw() && !(exertimer <= RTC_raw() && EXER_TIMER_SET_FLAG)){
		  if (DHT11_data.temp_int > 27){
			  if (real_time.rsec % 2 == 0){
				  petStats = hot1;
			  }
			  else {
				  petStats = hot2;
			  }
			  pet_update = 1;
		  }
		  else if (DHT11_data.temp_int < 24){
			  if (real_time.rsec % 2 == 0){
				  petStats = cold1;
			  }
			  else {
				  petStats = cold2;
			  }
			  pet_update = 1;
		  }
		  else {
			  petStats = normal;
			  pet_update = 1;
		  }
	  }
	  do {
		  //Home Buttons
		  if(mode==0){
			  if(Check_touchkey(&home_drink_water,&Coordinate)) {
				  alarm_release(); mode_new = 1; USART_WATER_FLAG = 1;
				  if(EXER_TIMER_SET_FLAG && exertimer<= RTC_raw()) EXER_TIMER_SET_FLAG = 0;
				  break;
			  }
			  if(Check_touchkey(&home_dark_mode,&Coordinate) && !ADC_DARKMODE_TOGGLE) {mode_new = 2; break;}
			  if(value > 1000 && (petStats != sleep1 && petStats != sleep2  && petStats != sleep_water) && ADC_DARKMODE_TOGGLE){mode_new = 2; break;}
			  if(value < 1000 && (petStats == sleep1 || petStats == sleep2  || petStats == sleep_water) && ADC_DARKMODE_TOGGLE){mode_new = 2; break;}
			  if(Check_touchkey(&home_pet,&Coordinate)) {pet_update = 1;	if (petStats != sleep1 && petStats != sleep2  && petStats != sleep_water) {petStats = happy1;}; break;}
			  if(Check_touchkey(&home_stats,&Coordinate)) {mode_new = 3; break;}
			  if(Check_touchkey(&home_config,&Coordinate)) {mode_new = 4; break;}
			  if (Check_touchkey(&home_set, &Coordinate)) {
			  					mode_new = 6;
			  					break;
			  				}
			  if (petStats == sleep1 && sec %2 == 0){
			  		  		petStats = sleep2;
			  		  		pet_update = 1;
			  		  }
			  else if (petStats == sleep2 && sec %2 == 1){
			  		  		petStats = sleep1;
			  		  		pet_update = 1;
			  		  }
		  }
		  //Other Buttons In Other Screen
		  else if (mode==3){//Statistics
			  if(Check_touchkey(&stats_home,&Coordinate)) {mode_new = 0; break;}
		  }else if (mode==4){//Configuration
			  if(Check_touchkey(&config_home,&Coordinate)) {mode_new = 0; break;}
			  if(Check_touchkey(&config_set_time,&Coordinate)) {mode_new = 5; USART_GET_TIME_FLAG = 1; break;}
			  if(Check_touchkey(&config_adc,&Coordinate)) {
				  mode_new = 0;
				  if(!ADC_DARKMODE_TOGGLE) ADC_DARKMODE_TOGGLE = 1;
				  else ADC_DARKMODE_TOGGLE = 0;
				  break;
			  }
		  }else if (mode==5){
			  if(Check_touchkey(&time_set_back,&Coordinate)) {
				  mode_new = 4;
				  USART_GET_TIME_FLAG = 0;
				  break;
			  }
		  }
		  else if (mode==6){
			  if (Check_touchkey(&stats_home, &Coordinate)) {
			  					mode_new = 0;
			  					break;
			  				}
			  				if (Check_touchkey(&plus_hour, &Coordinate)) {
			  					exertime += 3600;
			  					HAL_Delay(100);
			  					break;
			  				} else if (Check_touchkey(&minus_hour, &Coordinate)) {
			  					if (exertime > 3600) {
			  						exertime -= 3600;
			  					} else {
			  						exertime = 0;
			  					}
			  					HAL_Delay(100);
			  					break;
			  				} else if (Check_touchkey(&plus_min, &Coordinate)) {
			  					exertime += 60;
			  					HAL_Delay(100);
			  					break;
			  				} else if (Check_touchkey(&minus_min, &Coordinate)) {
			  					if (exertime > 60) {
			  						exertime -= 60;
			  					} else {
			  						exertime = 0;
			  					}
			  					HAL_Delay(100);
			  					break;
			  				} else if (Check_touchkey(&set_set, &Coordinate)) {
			  					if(exertime!=0){
			  						exertime_fixed = exertime;
									exertimer = RTC_raw() + exertime;
									EXER_TIMER_SET_FLAG = 1;
									mode_new = 0;
			  					}
			  					break;
			  				}
		  		  }


	  } while (0);

	  //Reset Coordinates
	  XPT2046_Reset_TouchPoint(&Coordinate);

	  if(next<= RTC_raw() || (exertimer<= RTC_raw() && EXER_TIMER_SET_FLAG)){ //Water/ Exercise Alarm
		  if(mode != 1) { //Not Drinking Water
			  if(!ALARM_TIMES_UP_RENDER_FLAG){ //Alarm Event that do only once
				  Beep_set(63999, 563, 281);//2 Hz
				  Beep_start();
				  pet_update = 1;
				  mode_new = 0;
				  USART_GET_TIME_FLAG = 0; //Prevent Get Time Flag causing Other ESP8266 functions get stuck
				  if((exertimer<= RTC_raw() && EXER_TIMER_SET_FLAG)){
					  Beep_set(63999, 1125, 562);//1 Hz
					  Beep_start();
					  USART_EXERCISE_FLAG = 1; //Start Sending Exercise Data
				  }
			  }
			  alarm_times_up();
		  }else{//In drinking Mode, Prevent Retriggering Alarm Just after drinking
			  if((exertimer<= RTC_raw() && EXER_TIMER_SET_FLAG)){
				  EXER_TIMER_SET_FLAG = 0;
			  }
		  }
	  }

	  if(mode != mode_new){
		  mode = mode_new;
		  render_done = 0;
	  }

	  //Render Pet Image If Updated
	  if(pet_update){
		  UI_Home_Display_Pet(60,70,petStats);
		  pet_update = 0;
	  }

	  //Flag Enables Every 30 Seconds
	  if(DHT11_SCHEDULE_FLAG){
		  DHT11_ReadData(&DHT11_data);
		  DHT11_SCHEDULE_FLAG = 0;
		  if(mode==0) UI_Home_Display_DHT11();
	  }



	  //Read Buffer when flag on
	  if(USART_GET_TIME_FLAG && mode==5){
		  if(USART_GET_TIME_FLAG==1) {esp8266_step_flag = 0; USART_GET_TIME_FLAG = 2;}
		  esp8266_get_time(); //Do The Get Time Procedures

		  if (esp8266_step_flag == 9){// Reading Done
			  char* t = USART_DATE_BUFFER;
			  uint16_t dt[6];//yearmonth, day, hour, min, sec
			  sscanf(t, "%04d%02d%02d%02d%02d%02d", &dt[0], &dt[1],&dt[2],&dt[3],&dt[4],&dt[5]);


			  //Update RTC Success
			  if(RTC_Set(dt[0],dt[1],dt[2],dt[3],dt[4],dt[5])==0){

				  //Update Flag And UI
				  USART_GET_TIME_FLAG = 0;
				  LCD_Clear(0, 100, 250,150);
				  LCD_DrawString(30, 100, "Done...");
				  alarm_update_last();
				  alarm_update_next();
	  	  	  }
		  }
	  }

	  //Upload Exercise Data
	  if(USART_EXERCISE_FLAG && !USART_GET_TIME_FLAG){
		  if(USART_EXERCISE_FLAG==1) {esp8266_step_flag = 0; USART_EXERCISE_FLAG=2;} //Reset Step Flag
		  esp8266_update_exercise();
	  }

	  //Upload drink water data
	  if(USART_WATER_FLAG && !USART_GET_TIME_FLAG && !USART_EXERCISE_FLAG){
		  if(USART_WATER_FLAG==1) {esp8266_step_flag = 0;USART_WATER_FLAG=2;} //Reset Step Flag
		  esp8266_update_water();
	  }


	  //Render LCD If Enter New Mode
	  Render(&mode_new, &render_done,petStats);


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */
  __HAL_RCC_PWR_CLK_ENABLE();//Save Time when power source disconnected (BKP)
  HAL_PWR_EnableBkUpAccess();//Enable Write Permission
  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  //Check If First Time Connect To Power Source
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)!= 0x5050){//Default Value should be 0xffff
	  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x5050);
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  }
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 63999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1125;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 300;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 63999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 33749;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 63999;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 33749;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
  __HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 DHT11_Pin PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|DHT11_Pin|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : K2_Pin */
  GPIO_InitStruct.Pin = K2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(K2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB5 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /** Disconnect NADV
  */

  __HAL_AFIO_FSMCNADV_DISCONNECTED();

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
