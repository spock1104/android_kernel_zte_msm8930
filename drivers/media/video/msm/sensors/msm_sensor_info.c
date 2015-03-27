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
 * Created by ZTE_JIA_20140529 jia.jia
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/sysdev.h>
#include "msm_sensor_info.h"


/*-----------------------------------------------------------------------------------------
 *
 * MACRO DEFINITION
 *
 *----------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
 *
 * TYPE DECLARATION
 *
 *----------------------------------------------------------------------------------------*/
enum msm_sensor_chip_info_t {
	CHIP_INFO_OV8825  = 0,
	CHIP_INFO_OV8835  = 1,
	CHIP_INFO_HI542   = 2,
	CHIP_INFO_OV9740  = 3,
	CHIP_INFO_UNKNOWN = 4,
	CHIP_INFO_MAX,
};

enum msm_sensor_module_info_t {
	MODULE_INFO_SUNNY   = 0,
	MODULE_INFO_TRULY   = 1,
	MODULE_INFO_QTECH   = 2,
	MODULE_INFO_LITEON  = 3,
	MODULE_INFO_MCNEX   = 4,
	MODULE_INFO_UNKNOWN = 5,
	MODULE_INFO_MAX,
};

struct msm_sensor_chip_map_t {
	uint16_t id;
	const char *name;
};

struct msm_sensor_module_map_t {
	uint16_t id;
	const char *name;
};


/*-----------------------------------------------------------------------------------------
 *
 * GLOBAL VARIABLE DEFINITION
 *
 *----------------------------------------------------------------------------------------*/
static enum msm_sensor_chip_info_t msm_sensor_backend_chip_info = CHIP_INFO_UNKNOWN;
static enum msm_sensor_chip_info_t msm_sensor_frontend_chip_info = CHIP_INFO_UNKNOWN;

static enum msm_sensor_module_info_t msm_sensor_backend_module_info = MODULE_INFO_UNKNOWN;

static struct msm_sensor_chip_map_t msm_sensor_chip_map[] = {
	[CHIP_INFO_OV8825] = {
		.id = 0x8825,
		.name = "OV8825-8.0Mp-AF",
	},

	[CHIP_INFO_OV8835] = {
		.id = 0x8830,
		.name = "OV8835-8.0Mp-AF",
	},

	[CHIP_INFO_HI542] = {
		.id = 0x00b1,
		.name = "HI542-5.0Mp-AF",
	},

	[CHIP_INFO_OV9740] = {
		.id = 0x9740,
		.name = "OV9740-1.0Mp-FF",
	},

	[CHIP_INFO_UNKNOWN] = {
		.id = 0xffff,
		.name = "UNKNOWN",
	},
};

static struct msm_sensor_module_map_t msm_sensor_module_map[] = {
	[MODULE_INFO_SUNNY] = {
		.id = 0x01,
		.name = "SUNNY",
	},

	[MODULE_INFO_TRULY] = {
		.id = 0x02,
		.name = "TRULY",
	},

	[MODULE_INFO_QTECH] = {
		.id = 0x06,
		.name = "QTECH",
	},

	[MODULE_INFO_LITEON] = {
		.id = 0x15,
		.name = "LITEON",
	},

	[MODULE_INFO_MCNEX] = {
		.id = 0x31,
		.name = "MCNEX",
	},

	[MODULE_INFO_UNKNOWN] = {
		.id = 0xffff,
		.name = "UNKNOWN",
	},
};

static struct sysdev_class msm_sensor_sysdev_class = {
	.name = "camera"
};

static struct sys_device msm_sensor_backend_sysdev = {
	.id = 0,
	.cls = &msm_sensor_sysdev_class,
};

static struct sys_device msm_sensor_frontend_sysdev = {
	.id = 1,
	.cls = &msm_sensor_sysdev_class,
};


/*-----------------------------------------------------------------------------------------
 *
 * FUNCTION DECLARATION
 *
 *----------------------------------------------------------------------------------------*/
static struct msm_sensor_chip_map_t msm_sensor_backend_get_chip_info(void);
static struct msm_sensor_chip_map_t msm_sensor_frontend_get_chip_info(void);
static struct msm_sensor_module_map_t msm_sensor_backend_get_module_info(void);
static ssize_t msm_sensor_backend_show_id(struct sys_device *dev,
																										 struct sysdev_attribute *attr,
																										 char *buf);
static ssize_t msm_sensor_frontend_show_id(struct sys_device *dev,
																										 struct sysdev_attribute *attr,
																										 char *buf);
static ssize_t msm_sensor_backend_show_name(struct sys_device *dev,
																												 struct sysdev_attribute *attr,
																												 char *buf);
static ssize_t msm_sensor_frontend_show_name(struct sys_device *dev,
																												 struct sysdev_attribute *attr,
																												 char *buf);


/*-----------------------------------------------------------------------------------------
 *
 * FUNCTION DEFINITION
 *
 *----------------------------------------------------------------------------------------*/
void msm_sensor_backend_set_chip_info(uint16_t chip_id)
{
	int32_t i;

	for (i = 0; i < ARRAY_SIZE(msm_sensor_chip_map); ++i) {
		if (msm_sensor_chip_map[i].id == chip_id) {
			msm_sensor_backend_chip_info = i;
			break;
		}
	}

	if (i == ARRAY_SIZE(msm_sensor_chip_map)) {
		msm_sensor_backend_chip_info = CHIP_INFO_UNKNOWN;
	}
}

void msm_sensor_frontend_set_chip_info(uint16_t chip_id)
{
	int32_t i;

	for (i = 0; i < ARRAY_SIZE(msm_sensor_chip_map); ++i) {
		if (msm_sensor_chip_map[i].id == chip_id) {
			msm_sensor_frontend_chip_info = i;
			break;
		}
	}

	if (i == ARRAY_SIZE(msm_sensor_chip_map)) {
		msm_sensor_frontend_chip_info = CHIP_INFO_UNKNOWN;
	}
}

void msm_sensor_backend_set_module_info(uint16_t module_id)
{
	int32_t i;

	for (i = 0; i < ARRAY_SIZE(msm_sensor_module_map); ++i) {
		if (msm_sensor_module_map[i].id == module_id) {
			msm_sensor_backend_module_info = i;
			break;
		}
	}

	if (i == ARRAY_SIZE(msm_sensor_module_map)) {
		msm_sensor_backend_module_info = MODULE_INFO_UNKNOWN;
	}
}

static struct msm_sensor_chip_map_t msm_sensor_backend_get_chip_info(void)
{
	return msm_sensor_chip_map[msm_sensor_backend_chip_info];
}

static struct msm_sensor_chip_map_t msm_sensor_frontend_get_chip_info(void)
{
	return msm_sensor_chip_map[msm_sensor_frontend_chip_info];
}

static struct msm_sensor_module_map_t __attribute__((__unused__)) msm_sensor_backend_get_module_info(void)
{
	return msm_sensor_module_map[msm_sensor_backend_module_info];
}

static ssize_t msm_sensor_backend_show_id(struct sys_device *dev,
																										 struct sysdev_attribute *attr,
																										 char *buf)
{
	return snprintf(buf, PAGE_SIZE, "0x%x\n", msm_sensor_backend_get_chip_info().id);
}

static ssize_t msm_sensor_frontend_show_id(struct sys_device *dev,
																										 struct sysdev_attribute *attr,
																										 char *buf)
{
	return snprintf(buf, PAGE_SIZE, "0x%x\n", msm_sensor_frontend_get_chip_info().id);
}

static ssize_t msm_sensor_backend_show_name(struct sys_device *dev,
																												 struct sysdev_attribute *attr,
																												 char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", msm_sensor_backend_get_chip_info().name);
}

static ssize_t msm_sensor_frontend_show_name(struct sys_device *dev,
																												 struct sysdev_attribute *attr,
																												 char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", msm_sensor_frontend_get_chip_info().name);
}

static struct sysdev_attribute msm_sensor_backend_sysdev_attrs[] = {
	_SYSDEV_ATTR(id, S_IRUGO | S_IWUSR | S_IWGRP, msm_sensor_backend_show_id, NULL),
	_SYSDEV_ATTR(name, S_IRUGO | S_IWUSR | S_IWGRP, msm_sensor_backend_show_name, NULL),
};

static struct sysdev_attribute msm_sensor_frontend_sysdev_attrs[] = {
	_SYSDEV_ATTR(id, S_IRUGO | S_IWUSR | S_IWGRP, msm_sensor_frontend_show_id, NULL),
	_SYSDEV_ATTR(name, S_IRUGO | S_IWUSR | S_IWGRP, msm_sensor_frontend_show_name, NULL),
};

/*
 * MSM Camera Sensor Info Sys Device Register
 *
 * 1. back-end sensor
 *
 *		/sys/devices/system/camera/camera0/id
 *		/sys/devices/system/camera/camera0/name
 *
 * 2. front-end sensor
 *
 *		/sys/devices/system/camera/camera1/id
 *		/sys/devices/system/camera/camera1/name
 */
static int __init msm_sensor_info_module(void)
{
	int32_t i = 0, j = 0, rc;

	rc = sysdev_class_register(&msm_sensor_sysdev_class);
	if (rc) {
		pr_err("%s: sysdev_class_register failed\n", __func__);
		return rc;
	}

	rc = sysdev_register(&msm_sensor_backend_sysdev);
	if (rc) {
		pr_err("%s: sysdev_register failed\n", __func__);
		goto msm_sensor_info_module_failed;
	}

	for (i = 0; i < ARRAY_SIZE(msm_sensor_backend_sysdev_attrs); ++i) {
		rc = sysdev_create_file(&msm_sensor_backend_sysdev, &msm_sensor_backend_sysdev_attrs[i]);
		if (rc) {
			pr_err("%s: sysdev_create_file failed\n", __func__);
			goto msm_sensor_info_module_failed;
		}
	}

	rc = sysdev_register(&msm_sensor_frontend_sysdev);
	if (rc) {
		pr_err("%s: sysdev_register failed\n", __func__);
		goto msm_sensor_info_module_failed;
	}

	for (j = 0; j < ARRAY_SIZE(msm_sensor_frontend_sysdev_attrs); ++j) {
		rc = sysdev_create_file(&msm_sensor_frontend_sysdev, &msm_sensor_frontend_sysdev_attrs[j]);
		if (rc) {
			pr_err("%s: sysdev_create_file failed\n", __func__);
			goto msm_sensor_info_module_failed;
		}
	}

	return 0;

msm_sensor_info_module_failed:

	while (--j >= 0) sysdev_remove_file(&msm_sensor_frontend_sysdev, &msm_sensor_frontend_sysdev_attrs[j]);
	sysdev_unregister(&msm_sensor_frontend_sysdev);

	while (--i >= 0) sysdev_remove_file(&msm_sensor_backend_sysdev, &msm_sensor_backend_sysdev_attrs[i]);
	sysdev_unregister(&msm_sensor_backend_sysdev);

	sysdev_class_unregister(&msm_sensor_sysdev_class);

	return rc;
}

module_init(msm_sensor_info_module);
MODULE_DESCRIPTION("Sensor info probe");
MODULE_LICENSE("GPL v2");
