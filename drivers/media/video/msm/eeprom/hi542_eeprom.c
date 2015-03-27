/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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
 * Created by zhaishichang 20140702
 * add eeprom info for hi542
 */

#include <linux/module.h>
#include "msm_camera_eeprom.h"
#include "msm_camera_i2c.h"

DEFINE_MUTEX(hi542_eeprom_mutex);
static struct msm_eeprom_ctrl_t hi542_eeprom_t;

static const struct i2c_device_id hi542_eeprom_i2c_id[] = {
	{"hi542_eeprom", (kernel_ulong_t)&hi542_eeprom_t},
	{ }
};

static int32_t msm_camera_eeprom_read_tbl_hi542(struct msm_eeprom_ctrl_t *ectrl,
	struct msm_camera_eeprom_read_t *read_tbl, uint16_t tbl_size)
{
	int32_t i, rc;
	uint16_t read_flag;

	CDBG("%s: E\n", __func__);

	if (!read_tbl) {
		return 0;
	}

	msm_camera_i2c_write(&ectrl->i2c_client, 0x03D0, 0xE9, MSM_CAMERA_I2C_BYTE_DATA); //man_spec_edof_ctrl_edof_fw_spare_0 Gain x7
	msm_camera_i2c_write(&ectrl->i2c_client, 0x0800, 0x07, MSM_CAMERA_I2C_BYTE_DATA); //0F,MSM_CAMERA_I2C_BYTE_DATA);EMI disable

	//otp_cfg1 timing setting
	msm_camera_i2c_write(&ectrl->i2c_client, 0x0740, 0x1A, MSM_CAMERA_I2C_BYTE_DATA);

	//otp_cfg8
	msm_camera_i2c_write(&ectrl->i2c_client, 0x0747, 0x06, MSM_CAMERA_I2C_BYTE_DATA);

	//otp_cf12 control register setting
	msm_camera_i2c_write(&ectrl->i2c_client, 0x0711, 0x81, MSM_CAMERA_I2C_BYTE_DATA);

	//opt_addr_h address setting 0x0381
	//msm_camera_i2c_write(&ectrl->i2c_client, 0x0720, 0x03, MSM_CAMERA_I2C_BYTE_DATA);

	//opt_addr_l
	//msm_camera_i2c_write(&ectrl->i2c_client, 0x0721, 0x81, MSM_CAMERA_I2C_BYTE_DATA);

	for (i = 0; i < 1; i++) {
		//opt_addr_h address setting 0x0381
		msm_camera_i2c_write(&ectrl->i2c_client, 0x0720, 0x03, MSM_CAMERA_I2C_BYTE_DATA);

		//opt_addr_l
		msm_camera_i2c_write(&ectrl->i2c_client, 0x0721, 0x8f, MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_read(&ectrl->i2c_client, 0x0722, &read_flag, MSM_CAMERA_I2C_BYTE_DATA);

		CDBG("%s: read_flag: 0x%x", __func__, read_flag);

		if (read_flag == 1) {
			//opt_addr_h address setting 0x0381
			msm_camera_i2c_write(&ectrl->i2c_client, 0x0720, 0x03, MSM_CAMERA_I2C_BYTE_DATA);

			//opt_addr_l
			msm_camera_i2c_write(&ectrl->i2c_client, 0x0721, 0x90, MSM_CAMERA_I2C_BYTE_DATA);

			rc = msm_camera_eeprom_read(ectrl, read_tbl[i].reg_addr, read_tbl[i].dest_ptr, read_tbl[i].num_byte,
				read_tbl[i].convert_endian);
			if (rc < 0) {
				pr_err("%s: msm_camera_eeprom_read failed\n", __func__);
				return rc;
			}
		} else if (read_flag == 3) {
			//opt_addr_h address setting 0x0381
			msm_camera_i2c_write(&ectrl->i2c_client, 0x0720, 0x03, MSM_CAMERA_I2C_BYTE_DATA);

			//opt_addr_l
			msm_camera_i2c_write(&ectrl->i2c_client, 0x0721, 0x92, MSM_CAMERA_I2C_BYTE_DATA);

			rc = msm_camera_eeprom_read(ectrl, read_tbl[i].reg_addr, read_tbl[i].dest_ptr, read_tbl[i].num_byte,
				read_tbl[i].convert_endian);
			if (rc < 0) {
				pr_err("%s: msm_camera_eeprom_read failed\n", __func__);
				return rc;
			}
		} else if (read_flag == 7) {
			//opt_addr_h address setting 0x0381
			msm_camera_i2c_write(&ectrl->i2c_client, 0x0720, 0x03, MSM_CAMERA_I2C_BYTE_DATA);

			//opt_addr_l
			msm_camera_i2c_write(&ectrl->i2c_client, 0x0721, 0x94, MSM_CAMERA_I2C_BYTE_DATA);

			rc = msm_camera_eeprom_read(ectrl, read_tbl[i].reg_addr, read_tbl[i].dest_ptr, read_tbl[i].num_byte,
				read_tbl[i].convert_endian);
			if (rc < 0) {
				pr_err("%s: msm_camera_eeprom_read failed\n", __func__);
				return rc;
			}
		}
	}

	msm_camera_i2c_write(&ectrl->i2c_client, 0x0711, 0x00, MSM_CAMERA_I2C_BYTE_DATA); //opt_ctl2

	CDBG("%s: X\n", __func__);

	return 0;
}

int32_t msm_eeprom_i2c_probe_hi542(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_eeprom_ctrl_t *e_ctrl_t = NULL;

	CDBG("%s: E\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s: i2c_check_functionality failed\n", __func__);
		goto probe_failure;
	}

	e_ctrl_t = (struct msm_eeprom_ctrl_t *)(id->driver_data);
	e_ctrl_t->i2c_client.client = client;

	if (e_ctrl_t->i2c_addr != 0) {
		e_ctrl_t->i2c_client.client->addr = e_ctrl_t->i2c_addr;
	}

	CDBG("%s: client = %x\n", __func__, (unsigned int) client);

	/* Assign name for sub device */
	snprintf(e_ctrl_t->sdev.name, sizeof(e_ctrl_t->sdev.name),
		"%s", e_ctrl_t->i2c_driver->driver.name);

	if (e_ctrl_t->func_tbl.eeprom_init != NULL) {
		rc = e_ctrl_t->func_tbl.eeprom_init(e_ctrl_t,
			e_ctrl_t->i2c_client.client->adapter);
	}
	msm_camera_eeprom_read_tbl_hi542(e_ctrl_t,
		e_ctrl_t->read_tbl,
		e_ctrl_t->read_tbl_size);

	if (e_ctrl_t->func_tbl.eeprom_format_data != NULL) {
		e_ctrl_t->func_tbl.eeprom_format_data();
	}

	if (e_ctrl_t->func_tbl.eeprom_release != NULL) {
		rc = e_ctrl_t->func_tbl.eeprom_release(e_ctrl_t);
	}

	/* Initialize sub device */
	v4l2_i2c_subdev_init(&e_ctrl_t->sdev,
		e_ctrl_t->i2c_client.client,
		e_ctrl_t->eeprom_v4l2_subdev_ops);

	CDBG("%s: result: %d\n", __func__, rc);

	return rc;

probe_failure:

	pr_err("%s: failed! rc: %d\n", __func__, rc);

	return rc;
}

static struct i2c_driver hi542_eeprom_i2c_driver = {
	.id_table = hi542_eeprom_i2c_id,
	.probe  = msm_eeprom_i2c_probe_hi542,
	.remove = __exit_p(hi542_eeprom_i2c_remove),
	.driver = {
		.name = "hi542_eeprom",
	},
};

static int __init hi542_eeprom_i2c_add_driver(void)
{
	return i2c_add_driver(hi542_eeprom_t.i2c_driver);
}

static struct v4l2_subdev_core_ops hi542_eeprom_subdev_core_ops = {
	.ioctl = msm_eeprom_subdev_ioctl,
};

static struct v4l2_subdev_ops hi542_eeprom_subdev_ops = {
	.core = &hi542_eeprom_subdev_core_ops,
};

static uint8_t hi542_wbcalib_data[2];
static struct msm_calib_wb hi542_wb_data;

static struct msm_camera_eeprom_info_t hi542_calib_supp_info = {
	{FALSE, 0, 0, 1},
	{TRUE, 3, 0, 128},
	{FALSE, 0, 0, 1},
	{FALSE, 0, 0, 1},
	{FALSE, 0, 0, 1},
};

static struct msm_camera_eeprom_read_t hi542_eeprom_read_tbl[] = {
	{0x0722, &hi542_wbcalib_data[0], 2, 0},
};

static struct msm_camera_eeprom_data_t hi542_eeprom_data_tbl[] = {
	{&hi542_wb_data, sizeof(struct msm_calib_wb)},
};

static void hi542_format_wbdata(void)
{
	hi542_wb_data.r_over_g = (uint16_t)(hi542_wbcalib_data[0]);
	hi542_wb_data.b_over_g = (uint16_t)(hi542_wbcalib_data[1]);
	hi542_wb_data.gr_over_gb = 128;
	if ((hi542_wb_data.r_over_g >= 128)
		|| (hi542_wb_data.b_over_g >= 128)
		|| (hi542_wb_data.r_over_g <= 0)
		|| (hi542_wb_data.b_over_g <= 0)) {
		hi542_wb_data.r_over_g = 85;
		hi542_wb_data.b_over_g = 76;
		hi542_wb_data.gr_over_gb = 128;
	}

	CDBG("r_over_g: 0x%x b_over_g: 0x%x gr_over_gb: 0x%x\n", hi542_wb_data.r_over_g,
		hi542_wb_data.b_over_g, hi542_wb_data.gr_over_gb);
}

static void hi542_format_calibrationdata(void)
{
	//hi542_get_module_info();
	hi542_format_wbdata();
}

static struct msm_eeprom_ctrl_t hi542_eeprom_t = {
	.i2c_driver = &hi542_eeprom_i2c_driver,
	.i2c_addr = 0x40,
	.eeprom_v4l2_subdev_ops = &hi542_eeprom_subdev_ops,

	.i2c_client = {
		.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
	},

	.eeprom_mutex = &hi542_eeprom_mutex,

	.func_tbl = {
		.eeprom_init = NULL,
		.eeprom_release = NULL,
		.eeprom_get_info = msm_camera_eeprom_get_info,
		.eeprom_get_data = msm_camera_eeprom_get_data,
		.eeprom_set_dev_addr = NULL,
		.eeprom_format_data = hi542_format_calibrationdata,
	},

	.info = &hi542_calib_supp_info,
	.info_size = sizeof(struct msm_camera_eeprom_info_t),
	.read_tbl = hi542_eeprom_read_tbl,
	.read_tbl_size = ARRAY_SIZE(hi542_eeprom_read_tbl),
	.data_tbl = hi542_eeprom_data_tbl,
	.data_tbl_size = ARRAY_SIZE(hi542_eeprom_data_tbl),
};

subsys_initcall(hi542_eeprom_i2c_add_driver);
MODULE_DESCRIPTION("HI542 EEPROM");
MODULE_LICENSE("GPL v2");
