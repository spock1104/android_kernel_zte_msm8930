#ifdef CONFIG_ZTE_HALL_AVAILABLE
struct zte_hall_platform_data {
		int hall_gpio; /*Battery Switch OFF pin*/
	};
struct hall_pwrkey {
	struct input_dev *hall_pwr;
};

typedef enum 
{
    HALL_STATE_NULL,
	HALL_STATE_OPEN,
	HALL_STATE_CLOSE,

} HALL_STATE;

#endif
