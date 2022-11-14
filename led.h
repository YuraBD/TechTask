/*******************************************************************************
* Include guard
*******************************************************************************/

#ifndef SOURCE_LED_H_
#define SOURCE_LED_H_

/*******************************************************************************
* Global constants
*******************************************************************************/
/* Allowed duty cycle for maximum brightness */
#define LED_MAX_BRIGHTNESS      (100u)

/* Allowed duty cycle for minimum brightness*/
#define LED_MIN_BRIGHTNESS      (0u)

/* LED blink timer clock value in Hz  */
#define DEFAULT_LED_BLINK_TIMER_CLOCK_HZ          (10000u)

/* LED blink timer period value */
#define DEFAULT_LED_BLINK_TIMER_PERIOD            (10000u)

/*******************************************************************************
* Structures and enumerations
*******************************************************************************/

typedef enum
{
	LED_ON,
    LED_OFF,
	LED_BLINKING
} led_state_t;

typedef struct
{
    led_state_t state;
    uint32_t brightness;
    uint32_t blinking_period;
} led_data_t;


/*******************************************************************************
* Function prototypes
*******************************************************************************/
cy_rslt_t initialize_led(void);
void update_led_state(led_data_t *led_data);


#endif /* SOURCE_LED_H_ */

/* [] END OF FILE */
