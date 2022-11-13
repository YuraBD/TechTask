/*******************************************************************************
* Header files includes
*******************************************************************************/
#include "cybsp.h"
#include "cyhal.h"
#include "led.h"

/*******************************************************************************
* Global constants
*******************************************************************************/
#define PWM_BRIGHTNESS_LED_FREQ_HZ    (1000000lu)  /* in Hz */
#define GET_DUTY_CYCLE(x)    (100 - x)

/*******************************************************************************
* Global constants
*******************************************************************************/
led_state_t led_state_cur = LED_OFF;
cyhal_pwm_t pwm_led_brightness;
cyhal_timer_t led_blink_timer;
bool timer_interrupt_flag = false;


void timer_init(void);
static void isr_timer(void *callback_arg, cyhal_timer_event_t event);


void update_led_state(led_data_t *ledData)
{
    if ((led_state_cur == LED_OFF) && (ledData->state == LED_ON))
    {
        led_state_cur = LED_ON;
    }
    else if ((led_state_cur == LED_OFF) && (ledData->state == LED_BLINKING))
	{
    	led_state_cur = LED_BLINKING;
	}
    else if ((led_state_cur == LED_ON) && (ledData->state == LED_OFF))
    {
        cyhal_pwm_stop(&pwm_led_brightness);
        led_state_cur = LED_OFF;
    }
    else if ((led_state_cur == LED_ON) && (ledData->state == LED_BLINKING))
    {
    	led_state_cur = LED_BLINKING;
    }
    else if ((led_state_cur == LED_BLINKING) && (ledData->state == LED_ON))
    {
    	cyhal_timer_stop(&led_blink_timer);
    	cyhal_timer_reset(&led_blink_timer);
    	timer_interrupt_flag = false;
    	led_state_cur = LED_ON;
    }
    else if ((led_state_cur == LED_BLINKING) && (ledData->state == LED_OFF))
    {
    	cyhal_timer_stop(&led_blink_timer);
    	cyhal_timer_reset(&led_blink_timer);
    	cyhal_pwm_stop(&pwm_led_brightness);
    	timer_interrupt_flag = false;
    	led_state_cur = LED_OFF;
    }
    else
    {
    }

    if ((LED_ON == led_state_cur) || (LED_BLINKING == led_state_cur))
    {
        cyhal_pwm_start(&pwm_led_brightness);
        uint32_t brightness = (ledData->brightness < LED_MIN_BRIGHTNESS) ? LED_MIN_BRIGHTNESS : ledData->brightness;

        /* Drive the LED with brightness */
        cyhal_pwm_set_duty_cycle(&pwm_led_brightness, GET_DUTY_CYCLE(brightness),
        						 PWM_BRIGHTNESS_LED_FREQ_HZ);
    }

    if (LED_BLINKING == led_state_cur)
    {
    	const cyhal_timer_cfg_t led_blink_timer_cfg =
    	{
		   .compare_value = 0,                 /* Timer compare value, not used */
		   .period = ledData->blinking_period,   /* Defines the timer period */
		   .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
		   .is_compare = false,                /* Don't use compare mode */
		   .is_continuous = true,              /* Run timer indefinitely */
		   .value = 0                          /* Initial value of counter */
    	};

    	cyhal_timer_configure(&led_blink_timer, &led_blink_timer_cfg);

    	cyhal_timer_start(&led_blink_timer);
    }
}

cy_rslt_t initialize_led(void)
{
    cy_rslt_t rslt;

    timer_init();
    cyhal_timer_start(&led_blink_timer);

    rslt = cyhal_pwm_init(&pwm_led_brightness, CYBSP_USER_LED, NULL);

    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_pwm_set_duty_cycle(&pwm_led_brightness,
                                        GET_DUTY_CYCLE(LED_MAX_BRIGHTNESS),
										PWM_BRIGHTNESS_LED_FREQ_HZ);
        if (CY_RSLT_SUCCESS == rslt)
        {
            rslt = cyhal_pwm_start(&pwm_led_brightness);
        }

    }

    if (CY_RSLT_SUCCESS == rslt)
    {
        led_state_cur = LED_BLINKING;
    }

    return rslt;
}


void timer_init(void)
{
   cy_rslt_t result;

   const cyhal_timer_cfg_t led_blink_timer_cfg =
   {
       .compare_value = 0,                 /* Timer compare value, not used */
       .period = DEFAULT_LED_BLINK_TIMER_PERIOD/2 / 2,   /* Defines the timer period */
       .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
       .is_compare = false,                /* Don't use compare mode */
       .is_continuous = true,              /* Run timer indefinitely */
       .value = 0                          /* Initial value of counter */
   };

   /* Initialize the timer object. Does not use input pin ('pin' is NC) and
    * does not use a pre-configured clock source ('clk' is NULL). */
   result = cyhal_timer_init(&led_blink_timer, NC, NULL);

   /* timer init failed. Stop program execution */
   if (result != CY_RSLT_SUCCESS)
   {
       CY_ASSERT(0);
   }

   /* Configure timer period and operation mode such as count direction,
      duration */
   cyhal_timer_configure(&led_blink_timer, &led_blink_timer_cfg);

   /* Set the frequency of timer's clock source */
   cyhal_timer_set_frequency(&led_blink_timer, DEFAULT_LED_BLINK_TIMER_CLOCK_HZ);

   /* Assign the ISR to execute on timer interrupt */
   cyhal_timer_register_callback(&led_blink_timer, isr_timer, NULL);

   /* Set the event on which timer interrupt occurs and enable it */
   cyhal_timer_enable_event(&led_blink_timer, CYHAL_TIMER_IRQ_TERMINAL_COUNT,
                             7, true);
}


static void isr_timer(void *callback_arg, cyhal_timer_event_t event)
{
   (void) callback_arg;
   (void) event;
   if (timer_interrupt_flag)
   {
	   cyhal_pwm_start(&pwm_led_brightness);
	   timer_interrupt_flag = false;
   }
   else
   {
	   cyhal_pwm_stop(&pwm_led_brightness);
	   timer_interrupt_flag = true;
   }
}

/* [] END OF FILE */

/* [] END OF FILE */
