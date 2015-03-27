
/* Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * Created by ZTE_JIA_20140331 jia.jia
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/leds-pmic8058.h>
#include <linux/pwm.h>
#include <linux/pmic8058-pwm.h>
#include <linux/hrtimer.h>
#include <linux/export.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

#if defined(CONFIG_MSM_CAMERA_EMODE)
#include <linux/sysdev.h>
#endif /* CONFIG_MSM_CAMERA_EMODE */

#include <mach/pmic.h>
#include <mach/camera.h>
#include <mach/gpio.h>
#include "msm_camera_i2c.h"
#include "flash_zte.h"

struct flash_work {
	struct work_struct my_work;
	int    x;
};

struct flash_work *work;
static struct timer_list flash_timer;
static struct workqueue_struct *flash_wq;
static struct i2c_client *i2c_client;
struct msm_flash_ctrl_t *flash_fctrl;

#if defined(CONFIG_MSM_CAMERA_EMODE)
static struct msm_flash_ctrl_t *flash_sysdev_fctrl;

static struct sysdev_class flash_sysdev_class = {
	.name = "led-flash"
};

static struct sys_device flash_sysdev;
#endif /* CONFIG_MSM_CAMERA_EMODE */

static int32_t msm_camera_flash_current_driver(struct msm_camera_sensor_flash_current_driver *current_driver,
																												unsigned led_state);
static int32_t msm_camera_flash_led(struct msm_camera_sensor_flash_external *external,
																						unsigned led_state);
static void flash_wq_function(struct work_struct *work);
static void flash_timer_callback(unsigned long data);
static int32_t msm_camera_flash_external(struct msm_camera_sensor_flash_external *external,
																									unsigned led_state);
static int32_t msm_camera_flash_pwm(struct msm_camera_sensor_flash_pwm *pwm,
																							unsigned led_state);
static int32_t msm_camera_flash_pmic(struct msm_camera_sensor_flash_pmic *pmic,
																							unsigned led_state);
static int32_t msm_strobe_flash_ctrl(struct msm_camera_sensor_strobe_flash_data *sfdata,
																					struct strobe_flash_ctrl_data *strobe_ctrl);

#if defined(CONFIG_MSM_CAMERA_EMODE)
static ssize_t store_torch(struct sys_device *dev, struct sysdev_attribute *attr, const char *buf, size_t buf_sz);
static ssize_t store_flash(struct sys_device *dev, struct sysdev_attribute *attr, const char *buf, size_t buf_sz);
static int32_t msm_camera_flash_register_sysdev(struct msm_flash_ctrl_t *fctrl);

static ssize_t store_torch(struct sys_device *dev, struct sysdev_attribute *attr, const char *buf, size_t buf_sz)
{
	int32_t enable = 0;
	int32_t ret;

	CDBG("%s: E\n", __func__);

	sscanf(buf, "%d", &enable);

	if (enable) {
		ret = flash_sysdev_fctrl->flash_ftbl->flash_init(flash_sysdev_fctrl);
		if (ret < 0) {
			pr_err("%s: failed %d\n", __func__, __LINE__);
			goto store_torch_failed;
		}
		ret = flash_sysdev_fctrl->flash_ftbl->flash_low(flash_sysdev_fctrl);
		if (ret < 0) {
			pr_err("%s: failed %d\n", __func__, __LINE__);
			goto store_torch_failed;
		}
	} else {
		(void)flash_sysdev_fctrl->flash_ftbl->flash_off(flash_sysdev_fctrl);
	}

	CDBG("%s: X\n", __func__);

	return buf_sz;

store_torch_failed:

  (void)flash_sysdev_fctrl->flash_ftbl->flash_off(flash_sysdev_fctrl);

	return ret;
}
static SYSDEV_ATTR(torch, S_IRUGO | S_IWUSR | S_IWGRP , NULL, store_torch);

static ssize_t store_flash(struct sys_device *dev, struct sysdev_attribute *attr, const char *buf, size_t buf_sz)
{
	int32_t enable = 0;
	int32_t ret;

	CDBG("%s: E\n", __func__);

	sscanf(buf, "%d", &enable);

	if (enable) {
		ret = flash_sysdev_fctrl->flash_ftbl->flash_init(flash_sysdev_fctrl);
		if (ret < 0) {
			pr_err("%s: failed %d\n", __func__, __LINE__);
			goto store_flash_failed;
		}
		ret = flash_sysdev_fctrl->flash_ftbl->flash_high(flash_sysdev_fctrl);
		if (ret < 0) {
			pr_err("%s: failed %d\n", __func__, __LINE__);
			goto store_flash_failed;
		}
	} else {
    (void)flash_sysdev_fctrl->flash_ftbl->flash_off(flash_sysdev_fctrl);
	}

	CDBG("%s: X\n", __func__);

	return buf_sz;

store_flash_failed:

  (void)flash_sysdev_fctrl->flash_ftbl->flash_off(flash_sysdev_fctrl);

	return ret;
}
static SYSDEV_ATTR(flash, S_IRUGO | S_IWUSR | S_IWGRP, NULL, store_flash);

static struct sysdev_attribute *flash_sysdev_attrs[] = {
	&attr_torch,
	&attr_flash,
};

/*
 * MSM Camera Flash Sys Device Register
 *
 * 1. Torch Mode
 *     enable: $ echo "1" > /sys/devices/system/led-flash/led-flash0/torch
 *    disable: $ echo "0" > /sys/devices/system/led-flash/led-flash0/torch
 *
 * 2. Flash Mode
 *     enable: $ echo "1" > /sys/devices/system/led-flash/led-flash0/flash
 *    disable: $ echo "0" > /sys/devices/system/led-flash/led-flash0/flash
 */
static int32_t msm_camera_flash_register_sysdev(struct msm_flash_ctrl_t *fctrl)
{
	int32_t i, rc;

	rc = sysdev_class_register(&flash_sysdev_class);
	if (rc) {
    return rc;
	}

	flash_sysdev.id = 0;
	flash_sysdev.cls = &flash_sysdev_class;
	rc = sysdev_register(&flash_sysdev);
	if (rc) {
		sysdev_class_unregister(&flash_sysdev_class);
		return rc;
	}

	for (i = 0; i < ARRAY_SIZE(flash_sysdev_attrs); ++i) {
		rc = sysdev_create_file(&flash_sysdev, flash_sysdev_attrs[i]);
		if (rc) {
			goto msm_camera_flash_register_sysdev_failed;
		}
	}

	flash_sysdev_fctrl = fctrl;

	return 0;

msm_camera_flash_register_sysdev_failed:

	while (--i >= 0) sysdev_remove_file(&flash_sysdev, flash_sysdev_attrs[i]);

	sysdev_unregister(&flash_sysdev);
	sysdev_class_unregister(&flash_sysdev_class);

	return rc;
}
#endif /* CONFIG_MSM_CAMERA_EMODE */

static int32_t msm_camera_flash_current_driver(
	struct msm_camera_sensor_flash_current_driver *current_driver,
	unsigned led_state)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	/* Evenly distribute current across all channels */
	switch (led_state) {
	case MSM_CAMERA_LED_OFF:
	case MSM_CAMERA_LED_LOW:
	case MSM_CAMERA_LED_HIGH:
	case MSM_CAMERA_LED_INIT:
	case MSM_CAMERA_LED_RELEASE:
		// Do nothing here
		rc = -ENODEV;
		break;

	default:
		rc = -EFAULT;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

static int32_t msm_camera_flash_led(
		struct msm_camera_sensor_flash_external *external,
		unsigned led_state)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (led_state) {
	case MSM_CAMERA_LED_INIT:
	case MSM_CAMERA_LED_RELEASE:
	case MSM_CAMERA_LED_OFF:
	case MSM_CAMERA_LED_LOW:
	case MSM_CAMERA_LED_HIGH:
		// Do nothing here
		rc = -ENODEV;
		break;

	default:
		rc = -EFAULT;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

static void __attribute__((unused)) flash_wq_function(struct work_struct *work)
{
	// TODO

	return;
}

static void __attribute__((unused)) flash_timer_callback(unsigned long data)
{
	queue_work(flash_wq, (struct work_struct *)work );
	mod_timer(&flash_timer, jiffies + msecs_to_jiffies(10000));
}

static int32_t msm_camera_flash_external(
	struct msm_camera_sensor_flash_external *external,
	unsigned led_state)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (led_state) {

	case MSM_CAMERA_LED_INIT:
		rc = flash_fctrl->flash_ftbl->flash_init(flash_fctrl);
		break;

	case MSM_CAMERA_LED_RELEASE:
		rc = flash_fctrl->flash_ftbl->flash_release(flash_fctrl);
		break;

	case MSM_CAMERA_LED_OFF:
		rc = flash_fctrl->flash_ftbl->flash_off(flash_fctrl);
		break;

	case MSM_CAMERA_LED_LOW:
		rc = flash_fctrl->flash_ftbl->flash_low(flash_fctrl);
		break;

	case MSM_CAMERA_LED_HIGH:
		rc = flash_fctrl->flash_ftbl->flash_high(flash_fctrl);
		break;

	default:
		rc = -EFAULT;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

static int32_t msm_camera_flash_pwm(
	struct msm_camera_sensor_flash_pwm *pwm,
	unsigned led_state)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (led_state) {
	case MSM_CAMERA_LED_LOW:
	case MSM_CAMERA_LED_HIGH:
	case MSM_CAMERA_LED_OFF:
	case MSM_CAMERA_LED_INIT:
	case MSM_CAMERA_LED_RELEASE:
		// Do nothing here
		rc = -ENODEV;
		break;

	default:
		rc = -EFAULT;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

static int32_t msm_camera_flash_pmic(
	struct msm_camera_sensor_flash_pmic *pmic,
	unsigned led_state)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (led_state) {
	case MSM_CAMERA_LED_OFF:
	case MSM_CAMERA_LED_LOW:
	case MSM_CAMERA_LED_HIGH:
	case MSM_CAMERA_LED_INIT:
	case MSM_CAMERA_LED_RELEASE:
		// Do nothing here
		rc = -ENODEV;
		break;

	default:
		rc = -EFAULT;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

static int32_t msm_strobe_flash_ctrl(struct msm_camera_sensor_strobe_flash_data *sfdata,
	struct strobe_flash_ctrl_data *strobe_ctrl)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (strobe_ctrl->type) {
	case STROBE_FLASH_CTRL_INIT:
	case STROBE_FLASH_CTRL_CHARGE:
	case STROBE_FLASH_CTRL_RELEASE:
		// Do nothing here
		rc = -ENODEV;
		break;

	default:
		rc = -EINVAL;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

int32_t msm_camera_flash_set_led_state(
	struct msm_camera_sensor_flash_data *fdata, unsigned led_state)
{
	int32_t rc;

	CDBG("%s: E\n", __func__);

	if (fdata->flash_type != MSM_CAMERA_FLASH_LED || fdata->flash_src == NULL) {
		return -ENODEV;
	}

	switch (fdata->flash_src->flash_sr_type) {
	case MSM_CAMERA_FLASH_SRC_PMIC:
		rc = msm_camera_flash_pmic(&fdata->flash_src->_fsrc.pmic_src, led_state);
		break;

	case MSM_CAMERA_FLASH_SRC_PWM:
		rc = msm_camera_flash_pwm(&fdata->flash_src->_fsrc.pwm_src,	led_state);
		break;

	case MSM_CAMERA_FLASH_SRC_CURRENT_DRIVER:
		rc = msm_camera_flash_current_driver(&fdata->flash_src->_fsrc.current_driver_src,	led_state);
		break;

	case MSM_CAMERA_FLASH_SRC_EXT:
		rc = msm_camera_flash_external(&fdata->flash_src->_fsrc.ext_driver_src,	led_state);
		break;

	case MSM_CAMERA_FLASH_SRC_LED1:
		rc = msm_camera_flash_led(&fdata->flash_src->_fsrc.ext_driver_src, led_state);
		break;

	default:
		rc = -ENODEV;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

int32_t msm_flash_ctrl(struct msm_camera_sensor_info *sdata,
	struct flash_ctrl_data *flash_info)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (flash_info->flashtype) {
	case LED_FLASH:
		rc = msm_camera_flash_set_led_state(sdata->flash_data, flash_info->ctrl_data.led_state);
		break;

	case STROBE_FLASH:
		rc = msm_strobe_flash_ctrl(sdata->strobe_flash_data, &(flash_info->ctrl_data.strobe_ctrl));
		break;

	default:
		pr_err("Invalid Flash MODE\n");
		rc = -EINVAL;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

int32_t msm_strobe_flash_init(struct msm_sync *sync, uint32_t sftype)
{
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	switch (sftype) {
	case MSM_CAMERA_STROBE_FLASH_XENON:
		// Do nothing here
		rc = -ENODEV;
		break;

	default:
		rc = -ENODEV;
		break;
	}

	CDBG("%s: X\n", __func__);

	return rc;
}

int32_t msm_flash_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	CDBG("%s: E\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		pr_err("%s: i2c_check_functionality failed!\n", __func__);
		return -ENODEV;
	}

	i2c_client = client;

	flash_fctrl = (struct msm_flash_ctrl_t *)id->driver_data;
	if (!flash_fctrl) {
		pr_err("%s: flash_fctrl is NULL!\n", __func__);
		return -EINVAL;
	}

	if (!flash_fctrl->i2c_client
			|| !flash_fctrl->i2c_ftbl
			|| !flash_fctrl->flash_ftbl
			|| !flash_fctrl->reg_setting) {
		pr_err("%s: flash_fctrl's member is NULL!\n", __func__);
		return -EINVAL;
	}

	flash_fctrl->i2c_client->client = client;

#if defined(CONFIG_MSM_CAMERA_EMODE)
	(void)msm_camera_flash_register_sysdev(flash_fctrl);
#endif /* CONFIG_MSM_CAMERA_EMODE */

	pr_info("%s %s probe succeeded\n", __func__, client->name);

	CDBG("%s: X\n", __func__);

	return 0;
}

int32_t msm_flash_i2c_remove(struct i2c_client *client)
{
	CDBG("%s: E\n", __func__);

	i2c_client = NULL;
	flash_fctrl = NULL;

	CDBG("%s: X\n", __func__);

	return 0;
}
