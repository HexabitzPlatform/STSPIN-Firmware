/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body for motor control application
 * @attention      : Copyright (c) 2020 STMicroelectronics. All rights reserved.
 *                  Licensed under Ultimate Liberty license SLA0044.
 *                  Obtain a copy of the License at: www.st.com/SLA0044
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "motorcontrol.h"
#include "Command_APIs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mc_api.h" // Motor control API for interacting with motor functions
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
/* USER CODE BEGIN PV */
float fTargetPos = 0;               // Target position for motor movement (radians)
float movementDuration_s = 2;       // Duration of motor movement in seconds
int32_t targetDeg = 0;              // Target position in degrees
uint8_t RecvdCommand[11];           // Buffer for received command data
uint8_t counterBytesCommand = 0;    // Counter for received command bytes
comState_t comState = IDEL;         // Current communication state
float Postion;                      // Target position from received message
float DurationPosition;             // Duration for position command
float curPosition;                  // Current motor position
int16_t Speed;                      // Motor speed for speed control
int16_t Torque;                     // Motor torque for torque control
uint16_t Time;                      // Time duration for speed/torque commands
uint8_t contMode;                   // Current control mode of the motor
uint8_t FirstBytesSentMSG[4];       // First 4 bytes of message to send
uint8_t SecondBytesSentMSG[4];      // Second 4 bytes of message to send
uint8_t ReceivedMessage[11];        // Buffer for received message
uint8_t isReceivedMEG = 0;          // Flag indicating a message was received
uint8_t FirstBytesRevdMSG[4];       // First 4 bytes of received message
uint8_t SecondBytesRevdMSG[4];      // Second 4 bytes of received message
uint8_t alignmentStatus = 0;        // Status of motor encoder alignment
uint8_t retries = 0;                // Retry counter for motor alignment
const uint8_t maxRetries = 5;       // Maximum number of retries for alignment
extern int messageCounter;           // Counter for processed messages

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);      // Configure system clock
static void MX_GPIO_Init(void);     // Initialize GPIO pins
static void MX_DMA_Init(void);      // Initialize DMA controller
static void MX_ADC_Init(void);      // Initialize ADC peripheral
static void MX_TIM1_Init(void);     // Initialize TIM1 for PWM generation
static void MX_TIM3_Init(void);     // Initialize TIM3 for encoder interface
static void MX_TIM14_Init(void);    // Initialize TIM14 for auxiliary timing
static void MX_TIM16_Init(void);    // Initialize TIM16 for auxiliary PWM
static void MX_USART1_UART_Init(void); // Initialize USART1 for communication
static void MX_NVIC_Init(void);     // Initialize NVIC for interrupts
/* USER CODE BEGIN PFP */
bool isMovementComplete(void);       // Check if motor movement is complete
uint8_t ProcessReceivedMessage(uint8_t *message); // Process incoming messages
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief  Checks if the motor movement is complete
 * @retval bool - Returns true if movement is complete, false otherwise
 */
bool isMovementComplete(void) {
    return (MC_GetControlPositionStatusMotor1() == TC_READY_FOR_COMMAND);
}

/**
 * @brief  Processes received message and updates motor control parameters
 * @param  message: Pointer to the received message buffer
 * @retval uint8_t - Returns 0 on success
 */
uint8_t ProcessReceivedMessage(uint8_t *message) {
	// Extract first and second 4 bytes from the message
	FirstBytesRevdMSG[0] = message[3];
	FirstBytesRevdMSG[1] = message[4];
	FirstBytesRevdMSG[2] = message[5];
	FirstBytesRevdMSG[3] = message[6];
	SecondBytesRevdMSG[0] = message[7];
	SecondBytesRevdMSG[1] = message[8];
	SecondBytesRevdMSG[2] = message[9];
	SecondBytesRevdMSG[3] = message[10];

	// Validate message header (0x48, 0x5A)
	if (message[0] == 72 && message[1] == 90) {
		switch (message[2]) {
		case STOP_MOTOR:
			messageCounter--; // Decrement counter (purpose unclear)

			comState = STOP_MOTOR; // Set state to stop motor
			break;
		case SET_POSITION:
			messageCounter--; // Decrement counter (purpose unclear)

			MC_StartMotor1(); // Start motor
			ProcessReceivedFloat(FirstBytesRevdMSG, &Postion); // Extract position
			ProcessReceivedFloat(SecondBytesRevdMSG, &DurationPosition); // Extract duration
			comState = SET_POSITION; // Set state to position control
			break;
		case SET_SPEED:
			messageCounter--; // Decrement counter (purpose unclear)
			MC_StartMotor1(); // Start motor
			ProcessReceivedUint16(FirstBytesRevdMSG, &Time); // Extract time
			ProcessReceivedInt16(SecondBytesRevdMSG, &Speed); // Extract speed
			comState = SET_SPEED; // Set state to speed control
			break;
		case SET_TORQUE:
			messageCounter--; // Decrement counter (purpose unclear)

			MC_StartMotor1(); // Start motor
			ProcessReceivedUint16(FirstBytesRevdMSG, &Time); // Extract time
			ProcessReceivedInt16(SecondBytesRevdMSG, &Torque); // Extract torque
			comState = SET_TORQUE; // Set state to torque control
			break;
		case GET_POSITION:
			messageCounter--; // Decrement counter (purpose unclear)

			comState = GET_POSITION; // Set state to get position
			break;
		case GET_CONTROL_MODE:
			messageCounter--; // Decrement counter (purpose unclear)

			comState = GET_CONTROL_MODE; // Set state to get control mode
			break;
		default:
			break; // Ignore invalid commands
		}
	}
	return 0; // Success
}
/* USER CODE END 0 */

/**
 * @brief  Main application entry point
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/
    HAL_Init(); // Initialize HAL library

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    /* Configure system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */

    /* Initialize peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC_Init();
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_MotorControl_Init();
    MX_TIM14_Init();
    MX_TIM16_Init();
    MX_USART1_UART_Init();
    MX_NVIC_Init();

    /* USER CODE BEGIN 2 */
    LL_TIM_EnableIT_UPDATE(TIM16); // Enable TIM16 update interrupt
    LL_TIM_EnableCounter(TIM16);   // Start TIM16 counter
    LL_USART_EnableIT_RXNE(USART1); // Enable USART1 RX not empty interrupt
    LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin); // Turn on LED
    MC_StartMotor1(); // Start motor 1

    // Wait for encoder alignment
    while (MC_GetAlignmentStatusMotor1() != TC_ALIGNMENT_COMPLETED) {
        if (MC_GetOccurredFaultsMotor1() != 0) {
            MC_AcknowledgeFaultMotor1(); // Clear fault
            LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin); // Turn off LED
            HAL_Delay(500); // Delay 500ms
            MC_StartMotor1(); // Restart motor
            LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin); // Turn on LED
            HAL_Delay(500); // Delay 500ms
        }
    }

    HAL_Delay(100); // Short delay after alignment
    LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin); // Turn off LED
    comState = IDEL; // Set initial communication state to idle
    /* USER CODE END 2 */

    /* Main loop */
    while (1) {
        // Blink LED if counter messageCounter >= 5
        if (messageCounter >= 5) {
            LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin); // Turn on LED
            HAL_Delay(200); // Delay 200ms
            LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin); // Turn off LED
            HAL_Delay(200); // Delay 200ms
        }

        /* USER CODE BEGIN 3 */
        // Handle motor faults
        if (MC_GetOccurredFaultsMotor1() != 0) {
            LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin); // Turn on LED
            MC_AcknowledgeFaultMotor1(); // Clear fault
            comState = IDEL; // Reset to idle state
        } else {
            LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin); // Turn off LED
        }

        // Process communication state
        switch (comState) {
            case STOP_MOTOR:
                MC_StopMotor1(); // Stop motor
                break;
            case SET_POSITION:
                MC_ProgramPositionCommandMotor1(Postion, DurationPosition); // Set position
                break;
            case SET_SPEED:
                MC_ProgramSpeedRampMotor1(Speed, Time); // Set speed
                break;
            case SET_TORQUE:
                MC_ProgramTorqueRampMotor1(Torque, Time); // Set torque
                break;
            case GET_POSITION:
                curPosition = MC_GetCurrentPosition1(); // Get current position
                ConvertFloatTwoBytes(curPosition, FirstBytesSentMSG); // Convert to bytes
                SendMessage(GET_POSITION, FirstBytesSentMSG, FirstBytesSentMSG); // Send position
                comState = IDEL; // Reset to idle
                break;
            case GET_CONTROL_MODE:
                contMode = MC_GetControlModeMotor1(); // Get control mode
                FirstBytesSentMSG[0] = contMode; // Store mode
                SendMessage(GET_CONTROL_MODE, FirstBytesSentMSG, FirstBytesSentMSG); // Send mode
                comState = IDEL; // Reset to idle
                break;
        }
        /* USER CODE END 3 */
    }
}

/**
 * @brief  Configures the system clock to 48 MHz using PLL with HSI as source
 * @retval None
 */
void SystemClock_Config(void) {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1); // Set flash latency
    if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {
        Error_Handler(); // Handle latency error
    }

    LL_RCC_HSI_Enable(); // Enable HSI oscillator
    while (LL_RCC_HSI_IsReady() != 1); // Wait for HSI ready
    LL_RCC_HSI_SetCalibTrimming(16); // Set HSI calibration
    LL_RCC_HSI14_Enable(); // Enable HSI14 for ADC
    while (LL_RCC_HSI14_IsReady() != 1); // Wait for HSI14 ready
    LL_RCC_HSI14_SetCalibTrimming(16); // Set HSI14 calibration

    // Configure PLL: HSI/2 * 12 = 48 MHz
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);
    LL_RCC_PLL_Enable(); // Enable PLL
    while (LL_RCC_PLL_IsReady() != 1); // Wait for PLL ready

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1); // Set AHB prescaler
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1); // Set APB1 prescaler
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL); // Set PLL as system clock
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL); // Wait for system clock switch

    LL_SetSystemCoreClock(48000000); // Set core clock to 48 MHz
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) { // Update time base
        Error_Handler(); // Handle tick init error
    }

    LL_RCC_HSI14_EnableADCControl(); // Enable HSI14 for ADC
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1); // Set USART1 clock source
}

/**
 * @brief  Configures NVIC interrupts for peripherals
 * @retval None
 */
static void MX_NVIC_Init(void) {
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0); // TIM1 interrupt priority
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); // Enable TIM1 interrupt
    NVIC_SetPriority(DMA1_Channel1_IRQn, 1); // DMA1 channel 1 priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn); // Enable DMA1 channel 1 interrupt
    NVIC_SetPriority(TIM3_IRQn, 3); // TIM3 interrupt priority
    NVIC_EnableIRQ(TIM3_IRQn); // Enable TIM3 interrupt
    NVIC_SetPriority(EXTI0_1_IRQn, 3); // EXTI0_1 interrupt priority
    NVIC_EnableIRQ(EXTI0_1_IRQn); // Enable EXTI0_1 interrupt
    NVIC_SetPriority(TIM14_IRQn, 2); // TIM14 interrupt priority
    NVIC_EnableIRQ(TIM14_IRQn); // Enable TIM14 interrupt
    NVIC_SetPriority(TIM16_IRQn, 2); // TIM16 interrupt priority
    NVIC_EnableIRQ(TIM16_IRQn); // Enable TIM16 interrupt
    NVIC_SetPriority(USART1_IRQn, 3); // USART1 interrupt priority
    NVIC_EnableIRQ(USART1_IRQn); // Enable USART1 interrupt
}

/**
 * @brief  Initializes ADC for motor current and voltage sensing
 * @retval None
 */
static void MX_ADC_Init(void) {
    LL_ADC_InitTypeDef ADC_InitStruct = {0}; // ADC initialization structure
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0}; // ADC regular channel structure
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO initialization structure

    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1); // Enable ADC1 clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); // Enable GPIOA clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); // Enable GPIOB clock

    // Configure GPIO pins for ADC (PA3, PA4, PA5, PB1)
    GPIO_InitStruct.Pin = M1_CURR_AMPL_U_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(M1_CURR_AMPL_U_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_CURR_AMPL_V_Pin;
    LL_GPIO_Init(M1_CURR_AMPL_V_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_CURR_AMPL_W_Pin;
    LL_GPIO_Init(M1_CURR_AMPL_W_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_BUS_VOLTAGE_Pin;
    LL_GPIO_Init(M1_BUS_VOLTAGE_GPIO_Port, &GPIO_InitStruct);

    // Configure DMA for ADC
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

    // Configure ADC channels (3, 4, 5, 9)
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_3);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_4);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_5);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_9);

    // Configure ADC global features
    ADC_InitStruct.Clock = LL_ADC_CLOCK_ASYNC;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_LEFT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM1_TRGO;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

    LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_7CYCLES_5);
    LL_ADC_DisableIT_EOC(ADC1); // Disable end of conversion interrupt
    LL_ADC_DisableIT_EOS(ADC1); // Disable end of sequence interrupt
    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);
}

/**
 * @brief  Initializes TIM1 for PWM generation for motor control
 * @retval None
 */
static void MX_TIM1_Init(void) {
    LL_TIM_InitTypeDef TIM_InitStruct = {0}; // TIM1 initialization structure
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0}; // Output compare structure
    LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0}; // Break and dead-time structure
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO initialization structure

    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1); // Enable TIM1 clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); // Enable GPIOB clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); // Enable GPIOA clock

    // Configure GPIO pin for TIM1 break input (PB12)
    GPIO_InitStruct.Pin = M1_OCP_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
    LL_GPIO_Init(M1_OCP_GPIO_Port, &GPIO_InitStruct);

    // Configure TIM1 channels for PWM
    TIM_InitStruct.Prescaler = ((TIM_CLOCK_DIVIDER) - 1);
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_CENTER_UP;
    TIM_InitStruct.Autoreload = ((PWM_PERIOD_CYCLES) / 2);
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV2;
    TIM_InitStruct.RepetitionCounter = (REP_COUNTER);
    LL_TIM_Init(TIM1, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM1);

    // Configure PWM channels (CH1, CH2, CH3, CH4)
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH1);

    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH2);

    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH3);

    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH4);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
    TIM_OC_InitStruct.CompareValue = (((PWM_PERIOD_CYCLES) / 2) - (HTMIN));
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH4);

    LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_OC4REF); // Set trigger output
    LL_TIM_DisableMasterSlaveMode(TIM1);

    // Configure break and dead-time
    TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_ENABLE;
    TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_ENABLE;
    TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_1;
    TIM_BDTRInitStruct.DeadTime = ((DEAD_TIME_COUNTS) / 2);
    TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_ENABLE;
    TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(TIM1, &TIM_BDTRInitStruct);

    // Configure GPIO pins for PWM outputs
    GPIO_InitStruct.Pin = M1_PWM_UL_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
    LL_GPIO_Init(M1_PWM_UL_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_PWM_VL_Pin;
    LL_GPIO_Init(M1_PWM_VL_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_PWM_WL_Pin;
    LL_GPIO_Init(M1_PWM_WL_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_PWM_UH_Pin;
    LL_GPIO_Init(M1_PWM_UH_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_PWM_VH_Pin;
    LL_GPIO_Init(M1_PWM_VH_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_PWM_WH_Pin;
    LL_GPIO_Init(M1_PWM_WH_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  Initializes TIM3 for encoder interface
 * @retval None
 */
static void MX_TIM3_Init(void) {
    LL_TIM_InitTypeDef TIM_InitStruct = {0}; // TIM3 initialization structure
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO initialization structure

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); // Enable TIM3 clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); // Enable GPIOA clock

    // Configure GPIO pins for encoder (PA6, PA7)
    GPIO_InitStruct.Pin = M1_ENCODER_A_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(M1_ENCODER_A_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = M1_ENCODER_B_Pin;
    LL_GPIO_Init(M1_ENCODER_B_GPIO_Port, &GPIO_InitStruct);

    // Configure TIM3 for encoder mode
    LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X4_TI12);
    LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, M1_ENC_IC_FILTER);
    LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, M1_ENC_IC_FILTER);
    LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = M1_PULSE_NBR;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM3, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM3);
    LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM3);
}

/**
 * @brief  Initializes TIM14 for auxiliary timing
 * @retval None
 */
static void MX_TIM14_Init(void) {
    LL_TIM_InitTypeDef TIM_InitStruct = {0}; // TIM14 initialization structure
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0}; // Output compare structure

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14); // Enable TIM14 clock

    // Configure TIM14 for PWM
    TIM_InitStruct.Prescaler = 2399;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 0;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM14, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM14);

    LL_TIM_OC_EnablePreload(TIM14, LL_TIM_CHANNEL_CH1);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM14, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM14, LL_TIM_CHANNEL_CH1);
}

/**
 * @brief  Initializes TIM16 for auxiliary PWM
 * @retval None
 */
static void MX_TIM16_Init(void) {
    LL_TIM_InitTypeDef TIM_InitStruct = {0}; // TIM16 initialization structure
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0}; // Output compare structure
    LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0}; // Break and dead-time structure

    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16); // Enable TIM16 clock

    // Configure TIM16 for PWM
    TIM_InitStruct.Prescaler = 47;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 1999;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init(TIM16, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM16);

    LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(TIM16, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM16, LL_TIM_CHANNEL_CH1);

    TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
    TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
    TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
    TIM_BDTRInitStruct.DeadTime = 0;
    TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
    TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(TIM16, &TIM_BDTRInitStruct);
}

/**
 * @brief  Initializes USART1 for serial communication
 * @retval None
 */
static void MX_USART1_UART_Init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0}; // USART initialization structure
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO initialization structure

    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1); // Enable USART1 clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); // Enable GPIOA clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); // Enable GPIOB clock

    // Configure GPIO pins for USART1 (PB6: TX, PA15: RX)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure USART1
    USART_InitStruct.BaudRate = 19200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_DisableIT_CTS(USART1); // Disable CTS interrupt
    LL_USART_ConfigAsyncMode(USART1); // Configure for asynchronous mode
    LL_USART_Enable(USART1); // Enable USART1
    LL_USART_TransmitData8(USART1, 72); // Transmit initial test byte
}

/**
 * @brief  Enables DMA controller clock
 * @retval None
 */
static void MX_DMA_Init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1); // Enable DMA1 clock
}

/**
 * @brief  Initializes GPIO pins for motor control and communication
 * @retval None
 */
static void MX_GPIO_Init(void) {
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0}; // EXTI initialization structure
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO initialization structure

    // Enable GPIO clocks
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    // Initialize output pins
    LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin); // Reset LED pin
    LL_GPIO_ResetOutputPin(RS485_DE_GPIO_Port, RS485_DE_Pin); // Reset RS485 driver enable
    LL_GPIO_SetOutputPin(OC_SEL_GPIO_Port, OC_SEL_Pin); // Set overcurrent select
    LL_GPIO_SetOutputPin(OCTH_STBY2_GPIO_Port, OCTH_STBY2_Pin); // Set standby 2
    LL_GPIO_SetOutputPin(OCTH_STBY1_GPIO_Port, OCTH_STBY1_Pin); // Set standby 1

    // Configure LED pin
    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

    // Configure overcurrent select pin
    GPIO_InitStruct.Pin = OC_SEL_Pin;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(OC_SEL_GPIO_Port, &GPIO_InitStruct);

    // Configure overcurrent comparator input
    GPIO_InitStruct.Pin = OC_COMP_INT2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(OC_COMP_INT2_GPIO_Port, &GPIO_InitStruct);

    // Configure standby pins
    GPIO_InitStruct.Pin = OCTH_STBY2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(OCTH_STBY2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = OCTH_STBY1_Pin;
    LL_GPIO_Init(OCTH_STBY1_GPIO_Port, &GPIO_InitStruct);

    // Configure RS485 driver enable pin
    GPIO_InitStruct.Pin = RS485_DE_Pin;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(RS485_DE_GPIO_Port, &GPIO_InitStruct);

    // Configure EXTI for encoder Z signal
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTF, LL_SYSCFG_EXTI_LINE1);
    LL_GPIO_SetPinPull(M1_ENCODER_Z_GPIO_Port, M1_ENCODER_Z_Pin, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(M1_ENCODER_Z_GPIO_Port, M1_ENCODER_Z_Pin, LL_GPIO_MODE_INPUT);
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_1;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);
}

/**
 * @brief  Error handler function
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add custom error handling here */
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports assert_param errors
 * @param  file: Pointer to the source file name
 * @param  line: Assert error line number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add custom implementation to report file and line */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
