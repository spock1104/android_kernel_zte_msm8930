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

/*
 * Created by ZTE_JIA_20140305 jia.jia
 */

#define OV8825_SENSOR_NAME "ov8825"
DEFINE_MSM_MUTEX(ov8825_mut);

static struct msm_camera_i2c_reg_conf ov8825_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf ov8825_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_sensor_ctrl_t ov8825_s_ctrl;

static struct v4l2_subdev_info ov8825_subdev_info[] = {
	{
		.code = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt = 1,
		.order = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_sensor_id_info_t ov8825_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x8825,
};

/*
 * 'CAM_VIO'/'CAM_VANA'/'CAM_VDIG'/'CAM_VAF' DISUSED here
 * 'CAM_VIO' connected to 'VREG_LVS1_1P8'
 * 'CAM_VDIG' connected to 'VREG_L11_1P8'
 * 'CAM_VANA'/'CAM_VAF' connected to 'VPH_PWR'
 *
 * refer to 'msm8930_rpm_regulator_init_data'
 * refer to 'msm_8930_cam_vreg'
 * refer to 'ov8825_power_seq'
 */
static enum msm_camera_vreg_name_t ov8825_veg_seq[] = {
	// Do nothing here
};

/*
 * 'REQUEST_VREG'/'ENABLE_VREG' DISUSED here
 * 'CAM_VIO' connected to 'VREG_LVS1_1P8'
 * 'CAM_VDIG' connected to 'VREG_L11_1P8'
 * 'CAM_VANA'/'CAM_VAF' connected to 'VPH_PWR'
 *
 * refer to 'msm8930_rpm_regulator_init_data'
 * refer to 'msm_8930_cam_vreg'
 * refer to 'ov8825_veg_seq'
 */
static struct msm_camera_power_seq_t ov8825_power_seq[] = {
	{REQUEST_GPIO, 0},
	{ENABLE_GPIO, 0},
	{CONFIG_CLK, 1},
	{CONFIG_I2C_MUX, 0},
};

static const struct i2c_device_id ov8825_i2c_id[] = {
	{OV8825_SENSOR_NAME, (kernel_ulong_t)&ov8825_s_ctrl},
	{ }
};

static struct i2c_driver ov8825_i2c_driver_bayer = {
	.id_table = ov8825_i2c_id,
	.probe  = msm_sensor_bayer_i2c_probe,
	.driver = {
		.name = OV8825_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov8825_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct v4l2_subdev_core_ops ov8825_subdev_core_ops = {
	.ioctl = msm_sensor_bayer_subdev_ioctl,
	.s_power = msm_sensor_bayer_power,
};

static struct v4l2_subdev_video_ops ov8825_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_bayer_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov8825_subdev_ops = {
	.core = &ov8825_subdev_core_ops,
	.video  = &ov8825_subdev_video_ops,
};

static int32_t ov8825_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
			MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		pr_err("%s: %s: read id failed\n", __func__,
			s_ctrl->sensordata->sensor_name);
		return rc;
	}

	CDBG("%s: read id: %x expected id %x:\n", __func__, chipid,
		s_ctrl->sensor_id_info->sensor_id);
	if (chipid != s_ctrl->sensor_id_info->sensor_id) {
		pr_err("msm_sensor_match_id chip id doesnot match\n");
		return -ENODEV;
	}

	msm_sensor_backend_set_chip_info(chipid);

	return rc;
}

static struct msm_sensor_fn_t ov8825_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_config = msm_sensor_bayer_config,
	.sensor_power_up = msm_sensor_bayer_power_up,
	.sensor_power_down = msm_sensor_bayer_power_down,
	.sensor_get_csi_params = msm_sensor_bayer_get_csi_params,
	.sensor_match_id = ov8825_match_id,
};

static struct msm_sensor_reg_t ov8825_sensor_reg = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov8825_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov8825_start_settings),
	.stop_stream_conf = ov8825_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ov8825_stop_settings),
};

static struct msm_sensor_ctrl_t ov8825_s_ctrl = {
	.sensor_i2c_client = &ov8825_sensor_i2c_client,
	.sensor_i2c_addr = 0x6C,
	.vreg_seq = ov8825_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(ov8825_veg_seq),
	.power_seq = &ov8825_power_seq[0],
	.num_power_seq = ARRAY_SIZE(ov8825_power_seq),
	.sensor_id_info = &ov8825_id_info,
	.msm_sensor_mutex = &ov8825_mut,
	.sensor_v4l2_subdev_info = ov8825_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov8825_subdev_info),
	.sensor_v4l2_subdev_ops = &ov8825_subdev_ops,
	.func_tbl = &ov8825_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
	.msm_sensor_reg = &ov8825_sensor_reg,
};
