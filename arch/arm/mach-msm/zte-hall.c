/*
 * Driver for some zte_hall functions
 */
#include <linux/module.h> 
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/input/pmic8xxx-pwrkey.h>
#include <linux/gpio.h>
#include <linux/suspend.h>
#include <linux/platform_device.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/wakelock.h>


#include "zte-hall.h"

#define	ENOMEM		12	/* Out of memory */
suspend_state_t new_state_backup= PM_SUSPEND_ON;/////liukejing;
static int factory_mode = 0;
int hall_is_factory_mode = 0;
int hall_current_factory_mode = 0;
struct hall_pwrkey *hallpwrkey;
struct wake_lock hall_wake_lock;
#define HALL_WAKELOCK_TIMEOUT 14

static struct zte_hall_platform_data hall_pdata;
static int hall_mesc = 120;
static void hall_timer_func(unsigned long);
static DEFINE_TIMER(hall_timer, hall_timer_func, 0, 0);
enum {
	DEBUG_HALL_STATE = 1U << 1,
};
static char *hall_dev_name[] = {
	"hall-wakelock",
};

unsigned int hallstate=(unsigned int)HALL_STATE_NULL;
module_param_named(hall_timer_debug, hall_mesc, int, S_IRUGO | S_IWUSR | S_IWGRP);


static int factory_mode_set(const char *val, struct kernel_param *kp)
{
	int ret;

	ret = param_set_int(val, kp);

	if (ret)
		return ret;
	
	if ((factory_mode & 0x02) == 0)
		hall_current_factory_mode = 0;
	else
		hall_current_factory_mode = 1;

	return 0;
}

static int factory_mode_get(char *buffer, struct kernel_param *kp)
{				
	switch(hallstate)
	{
		case HALL_STATE_OPEN:
			hall_is_factory_mode = 1;
			break;
		case HALL_STATE_CLOSE:
			hall_is_factory_mode = 0;
			break;
		default:
			hall_is_factory_mode = 0;
		break;
	}

	return	sprintf(buffer,"%d",hall_is_factory_mode);
}
module_param_call(factory_mode, factory_mode_set, factory_mode_get,
			&factory_mode, 0644);
static void hall_timer_func (unsigned long _pwrkey)
{
		bool open;
		open = gpio_get_value(hall_pdata.hall_gpio);////0:close;1:open
	    wake_unlock(&hall_wake_lock);
			if(open)
				{
				if(hall_mesc&DEBUG_HALL_STATE) 
					{
						pr_info("hall_detect_high_interrupt\n");
					}
				if(!hall_current_factory_mode)
					{
						if(hall_mesc&DEBUG_HALL_STATE) 
						{
							pr_info("hall_detect_high return reason:%d\n",new_state_backup);
						}   
						if(new_state_backup == PM_SUSPEND_MEM)////lcd off
						{			
							input_report_key(hallpwrkey->hall_pwr, KEY_POWER, 1);
							input_sync(hallpwrkey->hall_pwr);
							input_report_key(hallpwrkey->hall_pwr, KEY_POWER, 0);
							input_sync(hallpwrkey->hall_pwr);
							
							pr_info("hall_detect_high_interrupt:hall open\n");
	
						}
					}
					hallstate = HALL_STATE_OPEN;
				}
			else
			{
				if(hall_mesc&DEBUG_HALL_STATE) 
					{
						pr_info("hall_detect_low_interrupt return reason:%d\n",new_state_backup);
						pr_info("hall_detect_low_interrupt\n");
					}  
			 	
				if(!hall_current_factory_mode)
				{
						if(new_state_backup == PM_SUSPEND_ON)////lcd on
						{
	
							input_report_key(hallpwrkey->hall_pwr, KEY_POWER, 1);
							input_sync(hallpwrkey->hall_pwr);
							input_report_key(hallpwrkey->hall_pwr, KEY_POWER, 0);
							input_sync(hallpwrkey->hall_pwr);
							
							pr_info("hall_detect_low_interrupt:hall close\n");
	
						  }
				}
				hallstate = HALL_STATE_CLOSE;
				
			}

}

static irqreturn_t hall_detect_interrupt(int irq, void *_pwrkey)////open
{
	mod_timer(&hall_timer, jiffies + ((unsigned long)hall_mesc/10));
	wake_lock_timeout(&hall_wake_lock, HALL_WAKELOCK_TIMEOUT);

	if(hall_mesc&DEBUG_HALL_STATE) 
		{
			pr_info("hall timer start in hall_detect_interrupt\n");
		}
	return IRQ_HANDLED;
}

static int __devinit zte_hall_probe(struct platform_device *pdev)
{
	const struct zte_hall_platform_data *pdata = pdev->dev.platform_data;
	int ret = 0;
	int err;
	struct input_dev *hall_pwr;
	hallpwrkey = kzalloc(sizeof(*hallpwrkey), GFP_KERNEL);
	if (pdata->hall_gpio)
	{
		hall_pdata.hall_gpio = pdata->hall_gpio;

		if (!hallpwrkey)
		{
			pr_info("alloc address of hallpwrkey failed\n");
			return -ENOMEM;
		}
	hall_pwr = input_allocate_device();
	if (!hall_pwr) {
		pr_info("alloc address of pwr failed\n");
		err = -ENOMEM;
		goto free_pwrkey;
	}

	input_set_capability(hall_pwr, EV_KEY, KEY_POWER);
	err = input_register_device(hall_pwr);
	if (err) {
		pr_info( "Can't register hall ");
		
		goto free_input_dev;
	}

	hall_pwr->name = "hall_imitate_pwrkey";
	hall_pwr->phys = "hall_imitate_pwrkey/input0";
 
	hallpwrkey->hall_pwr = hall_pwr;
	wake_lock_init(&hall_wake_lock, WAKE_LOCK_SUSPEND,
				hall_dev_name[0]);

	ret = request_irq(gpio_to_irq(hall_pdata.hall_gpio),
			hall_detect_interrupt,
			(IRQF_TRIGGER_RISING |
			IRQF_TRIGGER_FALLING),
			"hall-detect", hallpwrkey);
	hallstate = gpio_get_value(hall_pdata.hall_gpio);

	if (ret) {
		pr_info("could not request IRQ HALL_DETECT_PIN for detect pin\n");
		gpio_free(hall_pdata.hall_gpio);
		}
	ret = enable_irq_wake(gpio_to_irq(hall_pdata.hall_gpio));
	if (ret) {
		pr_info("could not irq_set_irq_wake for detect pin\n");
		}
	}
	return 0;
free_input_dev:
	input_free_device(hall_pwr);
free_pwrkey:
	kfree(hallpwrkey);
	return err;

	}
static int __devexit zte_hall_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver zte_hall_device_driver = {
	.probe		= zte_hall_probe,
	.remove		= __devexit_p(zte_hall_remove),
	.driver		= {
		.name	= "zte-hall",
		.owner	= THIS_MODULE,
	}
};

static int __init zte_hall_init(void)
{
	return platform_driver_register(&zte_hall_device_driver);
}

static void __exit zte_hall_exit(void)
{
	platform_driver_unregister(&zte_hall_device_driver);
}

late_initcall(zte_hall_init);
module_exit(zte_hall_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hall driver for zte");
MODULE_ALIAS("platform:zte-hall");

