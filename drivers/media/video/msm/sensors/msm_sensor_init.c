/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#include "msm_sensor.h"
#include "msm.h"
#include "msm_sensor_bayer.h"
#include "msm_sensor_info.h"

/*
 * ZTE_JIA_20140313 jia.jia
 *
 * add camera driver for P893A21(CONFIG_MACH_COEUS) by caidezun 20140517
 */
#if 0
#include "imx091.h"
#else
#if defined(CONFIG_OV8825_BAYER)
#include "ov8825_bayer.h"
#endif /* defined(CONFIG_OV8825_BAYER) */

#if defined(CONFIG_OV8835_BAYER)
#include "ov8835_bayer.h"
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_HI542_BAYER)
#include "hi542_bayer.h"
#endif /* defined(CONFIG_HI542_BAYER) */
#endif

static struct i2c_driver *sensor_i2c_driver[] = {
/*
 * ZTE_JIA_20140313 jia.jia
 *
 * add camera driver for P893A21(CONFIG_MACH_COEUS) by caidezun 20140517
 */
#if 0
	/* back camera */
	&imx091_i2c_driver,

	/* front camera */
#else
#if defined(CONFIG_OV8825_BAYER)
	&ov8825_i2c_driver_bayer,
#endif /* defined(CONFIG_OV8825_BAYER) */

#if defined(CONFIG_OV8835_BAYER)
	&ov8835_i2c_driver_bayer,
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_HI542_BAYER)
	&hi542_i2c_driver_bayer,
#endif /* defined(CONFIG_HI542_BAYER) */

#endif
};

/*
 * Attention:
 * use 'i2c_add_driver' instead of 'i2c_add_driver_async'
 * to fix the failure of 'msm_sensor_bayer_i2c_probe' in scenario of
 * async initialization with 'i2c_add_driver_async'.
 *
 * by ZTE_JIA_20140409 jia.jia
 */
static int __init msm_sensor_init_module(void)
{
	int index = 0;

	for (index = 0; index < ARRAY_SIZE(sensor_i2c_driver); index++)	{
		i2c_add_driver(sensor_i2c_driver[index]);
	}

	return 0;
}

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sensor driver probe");
MODULE_LICENSE("GPL v2");
