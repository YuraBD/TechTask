#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "led.h"


/*******************************************************************************
* Macros
*******************************************************************************/

#define GPIO_INTERRUPT_PRIORITY (7u)
#define BRIGHTNESS_NOT_CHANGED (101u)
#define PERIOD_NOT_CHANGED (0u)

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);

/*******************************************************************************
* Global Variables
*******************************************************************************/

bool data_recieved = false;
cyhal_gpio_callback_data_t gpio_btn_callback_data;
uint8_t uart_read_value;
static led_data_t led_data = {LED_BLINKING, LED_MAX_BRIGHTNESS, DEFAULT_LED_BLINK_TIMER_PERIOD/2 / 2};



uint32_t get_value_with_first(uint8_t first_v)
{
    uint8_t buffer[4];
    buffer[0] = uart_read_value;
    for (int i = 1; i < 4; i++)
    {
        cyhal_uart_getc(&cy_retarget_io_uart_obj, &uart_read_value, 0);
        buffer[i] = uart_read_value;
    }
    uint32_t value = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    return value;
}

uint32_t get_value()
{
    uint8_t buffer[4];
    buffer[0] = uart_read_value;
    for (int i = 0; i < 4; i++)
    {
        cyhal_uart_getc(&cy_retarget_io_uart_obj, &uart_read_value, 0);
        buffer[i] = uart_read_value;
    }
    uint32_t value = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    return value;
}


int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif /* #if defined (CY_DEVICE_SECURE) */

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    
    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the user button */
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    /* Configure GPIO interrupt */
    gpio_btn_callback_data.callback = gpio_interrupt_handler;
    cyhal_gpio_register_callback(CYBSP_USER_BTN,
                                 &gpio_btn_callback_data);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                                 GPIO_INTERRUPT_PRIORITY, true);

    result = initialize_led();
    if (CYRET_SUCCESS != result)
    {
        /* Halt the CPU if CapSense initialization failed */
        printf("ERROR");
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                 CY_RETARGET_IO_BAUDRATE);
    cyhal_uart_clear(&cy_retarget_io_uart_obj);


    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    for (;;)
    {
        cyhal_uart_getc(&cy_retarget_io_uart_obj, &uart_read_value, 0);
        uint32_t mode = get_value_with_first(uart_read_value);
        uint32_t brightness = get_value();
        uint32_t period = get_value();
        if (mode == 0)
        {
            led_data.state = LED_ON;
        }
        else if (mode == 1)
        {
            led_data.state = LED_OFF;
        }
        else if (mode == 2)
        {
            led_data.state = LED_BLINKING;
        }

        if (brightness != BRIGHTNESS_NOT_CHANGED)
        {
            led_data.brightness = brightness;
        }

        if (period != PERIOD_NOT_CHANGED)
        {
            led_data.blinking_period = period;
        }
        update_led_state(&led_data);
    }
}

static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    led_data.state = LED_BLINKING;
    led_data.brightness = 100;
    led_data.blinking_period = DEFAULT_LED_BLINK_TIMER_PERIOD/2 / 2;
    update_led_state(&led_data);
    if (CY_RSLT_SUCCESS != cyhal_uart_putc(&cy_retarget_io_uart_obj, 'D'))
   {
        CY_ASSERT(0);
   }
}
