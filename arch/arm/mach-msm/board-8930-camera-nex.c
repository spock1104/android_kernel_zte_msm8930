/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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
 * Created by zhaishichang to add camera driver for N800_KK 20140612
 */

#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <mach/socinfo.h>
#include <mach/camera.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "board-8930.h"

#ifdef CONFIG_MSM_CAMERA

static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 1*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 2*/
		.drv = GPIOMUX_DRV_2MA,
		.dir = GPIOMUX_OUT_HIGH,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 3*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_5, /*active 4*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_6, /*active 5*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 6*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_3, /*active 7*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*i2c suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_KEEPER,
	},
	{
		.func = GPIOMUX_FUNC_2, /*active 9*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},
};

#if defined(CONFIG_HI542_BAYER) || defined(CONFIG_OV9740)
static struct msm_gpiomux_config msm8930_cam_common_configs[] = {
	{
		.gpio = 4,  // MCLK, Front-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[9],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 5,  // MCLK, Back-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 9,  // DVDD
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 15,  // VAAM
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 18,  // Flash Strobe
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 53,  // Standby, Front-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 54,  // Standby, Back-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 55,  // AVDD
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 76,  // Reset, Front-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 91,  // Flash Torch Mode
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 107,  // Reset, Back-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};
#endif /* defined(CONFIG_HI542_BAYER) || defined(CONFIG_OV9740) */

#if defined(CONFIG_HI542_BAYER) || defined(CONFIG_OV9740)
static struct msm_gpiomux_config msm8930_cam_2d_configs[] = {
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
};
#endif /* defined(CONFIG_HI542_BAYER) || defined(CONFIG_OV9740) */

#ifdef CONFIG_MSM_CAMERA_FLASH
#if defined(CONFIG_LM3642)
static struct msm_camera_sensor_flash_src msm_flash_src_lm3642 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = 91, // Flash Torch Mode
	._fsrc.ext_driver_src.led_flash_en = 18, // Flash Strobe
	._fsrc.ext_driver_src.flash_id = MAM_CAMERA_EXT_LED_FLASH_LM3642,
};
#endif
#endif

static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 27648000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 800000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 800000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_video_ls_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 800000000,
		.ib  = 3522000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_dual_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 302071680,
		.ib  = 1208286720,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_video_ls_vectors),
		cam_video_ls_vectors,
	},
	{
		ARRAY_SIZE(cam_dual_vectors),
		cam_dual_vectors,
	},
};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

static struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
		.csid_core = 0,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
	{
		.csid_core = 1,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
};

#if defined(CONFIG_HI542_BAYER) || defined(CONFIG_OV9740)
/*
 * 'CAM_VIO'/'CAM_VANA'/'CAM_VDIG'/'CAM_VAF' DISUSED here
 * 'CAM_VIO' connected to 'VREG_L11_1P8'
 * 'CAM_VDIG' connected to 'VREG_L11_1P8'
 * 'CAM_VANA'/'CAM_VAF' connected to 'VPH_PWR'
 *
 * refer to 'msm8930_rpm_regulator_init_data'
 * refer to 'msm_8930_cam_vreg'
 * refer to 'hi542_power_seq'
 */
static struct camera_vreg_t msm_8930_cam_vreg[] = {
// Do nothing here
};
#endif /* defined(CONFIG_HI542_BAYER) || defined(CONFIG_OV9740) */

static struct gpio msm8930_common_cam_gpio[] = {
	{20, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{21, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};

#if defined(CONFIG_OV9740)
static struct gpio msm8930_front_cam_gpio[] = {
	{55, GPIOF_DIR_OUT, "CAM_AVDD_EN"},
	{9, GPIOF_DIR_OUT, "CAM_DVDD_EN"},
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},
	{53, GPIOF_DIR_OUT, "CAM_STBY_N"},
};
#endif /* defined(CONFIG_OV9740) */

#if defined(CONFIG_HI542_BAYER)
static struct gpio msm8930_back_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{9, GPIOF_OUT_INIT_HIGH, "DVDD_LDO_EN"},
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},
	{55, GPIOF_OUT_INIT_HIGH, "AVDD_LDO_EN"},
	//{15, GPIOF_OUT_INIT_HIGH, "MOTOR_LDO_EN"},
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_gpio_set_tbl msm8930_front_cam_gpio_set_tbl[] = {
	{55, GPIOF_OUT_INIT_HIGH, 0},
	{9, GPIOF_OUT_INIT_HIGH, 0},
	{53, GPIOF_OUT_INIT_HIGH, 1},
	{53, GPIOF_OUT_INIT_LOW, 1},
	{76, GPIOF_OUT_INIT_HIGH, 1},
	{76, GPIOF_OUT_INIT_LOW, 1},
	{76, GPIOF_OUT_INIT_HIGH, 1},
};
#endif /* defined(CONFIG_OV9740) */

#if defined(CONFIG_HI542_BAYER)
static struct msm_gpio_set_tbl msm8930_back_cam_gpio_set_tbl[] = {
	{53, GPIOF_OUT_INIT_LOW, 1},
	{76, GPIOF_OUT_INIT_LOW, 1},
	{54, GPIOF_OUT_INIT_LOW, 1},
	{55, GPIOF_OUT_INIT_HIGH, 2000},
	{9, GPIOF_OUT_INIT_HIGH, 2000},
	{107, GPIOF_OUT_INIT_LOW, 5000},
	{107, GPIOF_OUT_INIT_HIGH, 2000},
	{54, GPIOF_OUT_INIT_HIGH, 5000},
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_gpio_conf msm_8930_front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio_set_tbl),
};
#endif

#if defined(CONFIG_HI542_BAYER)
static struct msm_camera_gpio_conf msm_8930_back_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_back_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio_set_tbl),
};
#endif

/*
 * add eeprom info for hi542 by zhaishichang  20140702
 */

#if defined(CONFIG_HI542_BAYER)
#if defined(CONFIG_HI542_EEPROM)
static struct i2c_board_info hi542_eeprom_i2c_info = {
	I2C_BOARD_INFO("hi542_eeprom", 0x40>>3),
};

static struct msm_eeprom_info hi542_eeprom_info = {
	.board_info = &hi542_eeprom_i2c_info,
	.bus_id = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.eeprom_i2c_slave_addr = 0x6C,
	.eeprom_reg_addr = 0x05,
	.eeprom_read_length = 6,
};
#endif  /* defined(CONFIG_HI542_BAYER) */
#endif  /* defined(CONFIG_HI542_EEPROM) */


#if defined(CONFIG_HI542_BAYER)
static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x18>>1),
};
#endif /* defined(CONFIG_HI542_BAYER) */

/*
 * Change actuator info for hi542 by zhaishichang  20140702
 * change "msm_act_main_cam_2_info" to "msm_act_main_cam_1_info"
 * change "MSM_ACTUATOR_MAIN_CAM_2" to "MSM_ACTUATOR_MAIN_CAM_1"
 */
#if defined(CONFIG_HI542_BAYER)
static struct msm_actuator_info msm_act_main_cam_1_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name       = MSM_ACTUATOR_MAIN_CAM_1,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 15,
	.vcm_enable     = 1,
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_HI542_BAYER)
static struct msm_camera_sensor_flash_data flash_hi542 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
#if defined(CONFIG_LM3642)
	.flash_src	= &msm_flash_src_lm3642
#else
	.flash_src=NULL
#endif
#endif
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_sensor_flash_data flash_ov9740 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= NULL
#endif
};
#endif /* defined(CONFIG_OV9740) */

#if defined(CONFIG_HI542_BAYER)
static struct msm_camera_csi_lane_params hi542_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_csi_lane_params ov9740_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};
#endif /* defined(CONFIG_OV9740) */

#if defined(CONFIG_HI542_BAYER)
static struct msm_camera_sensor_platform_info sensor_board_info_hi542 = {
	.mount_angle = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &hi542_csi_lane_params,
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_sensor_platform_info sensor_board_info_ov9740 = {
	.mount_angle = 270,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &ov9740_csi_lane_params,
};
#endif /* defined(CONFIG_OV9740) */

/*
 * Change actuator info for hi542 by zhaishichang  20140702
 * change "msm_act_main_cam_2_info" to "msm_act_main_cam_1_info"
 */
#if defined(CONFIG_HI542_BAYER)
static struct msm_camera_sensor_info msm_camera_sensor_hi542_data = {
	.sensor_name = "hi542",
	.pdata = &msm_camera_csi_device_data[0],
	.flash_data = &flash_hi542,
	.sensor_platform_info = &sensor_board_info_hi542,
	.csi_if = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_1_info,

/*
 * Add eeprom info for hi542 by zhaishichang  20140702
 */
#if defined(CONFIG_HI542_EEPROM)
	.eeprom_info = &hi542_eeprom_info,
#endif /* defined(CONFIG_HI542_EEPROM) */
};
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_sensor_info msm_camera_sensor_ov9740_data = {
	.sensor_name = "ov9740",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_ov9740,
	.sensor_platform_info = &sensor_board_info_ov9740,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
	.actuator_info = NULL,
};
#endif /* defined(CONFIG_OV9740) */

static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

#ifdef CONFIG_I2C
struct i2c_board_info msm8930_camera_i2c_boardinfo[] = {
#if defined(CONFIG_HI542_BAYER)
	{
	I2C_BOARD_INFO("hi542", 0x40>>1),
	.platform_data = &msm_camera_sensor_hi542_data,
	},
#endif /* defined(CONFIG_HI542_BAYER) */

#if defined(CONFIG_OV9740)
	{
	I2C_BOARD_INFO("ov9740", 0x20>>1),
	.platform_data = &msm_camera_sensor_ov9740_data,
	},
#endif /* defined(CONFIG_OV9740) */

#if defined(CONFIG_LM3642)
	{
	I2C_BOARD_INFO("lm3642", 0xC6>>1),
	},
#endif /* defined(CONFIG_LM3642) */
};

struct msm_camera_board_info msm8930_camera_board_info = {
	.board_info = msm8930_camera_i2c_boardinfo,
	.num_i2c_board_info = ARRAY_SIZE(msm8930_camera_i2c_boardinfo),
};
#endif

void __init msm8930_init_cam(void)
{
	msm_gpiomux_install(msm8930_cam_common_configs,
			ARRAY_SIZE(msm8930_cam_common_configs));

	platform_device_register(&msm_camera_server);
	platform_device_register(&msm8960_device_csiphy0);
	platform_device_register(&msm8960_device_csiphy1);
	platform_device_register(&msm8960_device_csid0);
	platform_device_register(&msm8960_device_csid1);
	platform_device_register(&msm8960_device_ispif);
	platform_device_register(&msm8960_device_vfe);
	platform_device_register(&msm8960_device_vpe);
}
#endif
