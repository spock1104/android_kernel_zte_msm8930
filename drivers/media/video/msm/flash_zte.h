/* Copyright (c) 2009-2013, The Linux Foundation. All rights reserved.
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

#ifndef FLASH_ZTE_H
#define FLASH_ZTE_H

#include <mach/camera.h>
#include <media/msm_camera.h>
#include "msm_camera_i2c.h"

struct msm_flash_ctrl_t;

struct msm_camera_i2c_fn_t {
	int32_t (*i2c_read)(struct msm_camera_i2c_client *client,	uint16_t addr, uint16_t *data,
											enum msm_camera_i2c_data_type data_type);
	int32_t (*i2c_write)(struct msm_camera_i2c_client *client, uint16_t addr, uint16_t data,
											enum msm_camera_i2c_data_type data_type);
};

struct msm_flash_fn_t {
	int32_t (*flash_init)(struct msm_flash_ctrl_t *);
	int32_t (*flash_release)(struct msm_flash_ctrl_t *);
	int32_t (*flash_off)(struct msm_flash_ctrl_t *);
	int32_t (*flash_low)(struct msm_flash_ctrl_t *);
	int32_t (*flash_high)(struct msm_flash_ctrl_t *);
};

struct msm_flash_reg_t {
	enum msm_camera_i2c_data_type default_data_type;
	struct msm_camera_i2c_reg_conf *init_setting;
	uint8_t init_setting_size;
	struct msm_camera_i2c_reg_conf *release_setting;
	uint8_t release_setting_size;
	struct msm_camera_i2c_reg_conf *off_setting;
	uint8_t off_setting_size;
	struct msm_camera_i2c_reg_conf *low_setting;
	uint8_t low_setting_size;
	struct msm_camera_i2c_reg_conf *high_setting;
	uint8_t high_setting_size;
};

struct msm_flash_ctrl_t {
	struct msm_camera_i2c_client *i2c_client;
	struct msm_camera_i2c_fn_t *i2c_ftbl;
	struct msm_flash_fn_t *flash_ftbl;
	void *data;
	struct msm_flash_reg_t *reg_setting;
};

int32_t msm_flash_init(struct msm_flash_ctrl_t *fctrl);
int32_t msm_flash_release(struct msm_flash_ctrl_t *fctrl);
int32_t msm_flash_off(struct msm_flash_ctrl_t *fctrl);
int32_t msm_flash_low(struct msm_flash_ctrl_t *fctrl);
int32_t msm_flash_high(struct msm_flash_ctrl_t *fctrl);
int32_t msm_flash_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
int32_t msm_flash_i2c_remove(struct i2c_client *client);

#endif /* FLASH_ZTE_H */
