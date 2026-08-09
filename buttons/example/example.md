console_t console_usb;
cli_result_t result;
cli_t cli;

driver_i2c_it_t driver_i2c;
mcp3424_t		Adc;

driver_gpio_t driver_btn1;
button_t btn1;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


void Btn1ShortPress(void *obj)
{
	Console_PrintLine(&console_usb, "***btn1 short pressed***");
}

void Btn1LongPress(void *obj)
{
	Console_PrintLine(&console_usb, "***btn1 long pressed***");
}

void Btn1ShortRelease(void *obj)
{
	Console_PrintLine(&console_usb, "***btn1 short release***");
}

void Btn1LongRelease(void *obj)
{
	Console_PrintLine(&console_usb, "***btn1 long release***");
}

void Btn1Repeat(void *obj)
{
	Console_PrintLine(&console_usb, "***btn1 repeated***");

}

void Button_ShortPressedCallback(button_t *obj);


/* Private user code ---------------------------------------------------------*/

static const button_callbacks_t callbacks =
{
    .short_pressed  = Btn1ShortPress,
    .short_released = Btn1ShortRelease,
    .long_pressed   = Btn1LongPress,
    .long_released  = Btn1LongRelease,
    .repeated       = Btn1Repeat
};


int main(void)
{

 
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USB_Device_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  
  Console_Init(&console_usb, &COMMUNICATION_USB, NULL);

  Driver_GPIO_Init(&driver_btn1, sw_GPIO_Port, sw_Pin, DRIVER_GPIO_MODE_EXTI);
  Button_Init(&btn1, &driver_btn1, ACTIVE_HIGH, REPEAT, 50, 100, 1200, 1000, 300, &callbacks, NULL);


  Console_Printf(&console_usb,
                 "Hello World!\r\n");


  while (1)
  {
	  Button_Update(&btn1, HAL_GetTick());

  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == sw_Pin)
	    {
	        Driver_GPIO_EXTI_Callback(&driver_btn1);
	    }
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    Driver_I2C_IT_TxCpltCallback(&driver_i2c, hi2c);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    Driver_I2C_IT_RxCpltCallback(&driver_i2c, hi2c);
}

