/* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
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
 /*===========================================================================
 when         who        what, where, why                         comment tag
 2010-12-15   ruijiagui  Add ZTE_FEATURE_SD_DUMP feature          ZTE_RJG_RIL_20121214
 ===========================================================================*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/cpu.h>
#include <linux/interrupt.h>
#include <linux/mfd/pmic8058.h>
#include <linux/mfd/pmic8901.h>
#include <linux/mfd/pm8xxx/misc.h>

#include <asm/mach-types.h>

#include <mach/msm_iomap.h>
#include <mach/restart.h>
#include <mach/socinfo.h>
#include <mach/irqs.h>
#include <mach/scm.h>
#ifdef CONFIG_MACH_STORMER
#include <mach/gpio.h>
#endif
#include "msm_watchdog.h"
#include "timer.h"


//ZTE_RJG_RIL_20121214 begin
#include <mach/boot_shared_imem_cookie.h>
//ZTE_RJG_RIL_20121214 end
#define WDT0_RST	0x38
#define WDT0_EN		0x40
#define WDT0_BARK_TIME	0x4C
#define WDT0_BITE_TIME	0x5C

#define PSHOLD_CTL_SU (MSM_TLMM_BASE + 0x820)

#define RESTART_REASON_ADDR 0x65C
#define DLOAD_MODE_ADDR     0x0
#define SDDUMP_MAGIC_NUM          0x20121221

#define SCM_IO_DISABLE_PMIC_ARBITER	1

#ifdef CONFIG_LGE_CRASH_HANDLER
#define LGE_ERROR_HANDLER_MAGIC_NUM	0xA97F2C46
#define LGE_ERROR_HANDLER_MAGIC_ADDR	0x18
void *lge_error_handler_cookie_addr;
static int ssr_magic_number = 0;
#endif

static int restart_mode;
static int ignore_sd_dump = 0;
void *restart_reason;

int pmic_reset_irq;
static void __iomem *msm_tmr0_base;

#ifdef CONFIG_MSM_DLOAD_MODE
static int in_panic;
static void *dload_mode_addr;

/* Download mode master kill-switch */
static int dload_set(const char *val, struct kernel_param *kp);
static int download_mode = 1;


static int dload_ignor_sd_dump_set(const char *val, struct kernel_param *kp);

module_param_call(download_mode, dload_set, param_get_int,
			&download_mode, 0644);


module_param_call(ignore_sd_dump, dload_ignor_sd_dump_set, param_get_int,
			&ignore_sd_dump, 0644);


/*Use Qualcomm's usb product_id, as Qualcomm's engineer 
  may not install OEM's usb driver,wangzy,3of4*/
extern void use_qualcomm_usb_product_id(void);
/*end*/

static int panic_prep_restart(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	in_panic = 1;
	/*Use Qualcomm's usb product_id if enter download due to panic,wangzy,4of4*/
	use_qualcomm_usb_product_id();
	/*end*/
	return NOTIFY_DONE;
}

static struct notifier_block panic_blk = {
	.notifier_call	= panic_prep_restart,
};

static void set_dload_mode(int on)
{
	if (dload_mode_addr) {
		__raw_writel(on ? 0xE47B337D : 0, dload_mode_addr);
		__raw_writel(on ? 0xCE14091A : 0,
		       dload_mode_addr + sizeof(unsigned int));
#ifdef CONFIG_LGE_CRASH_HANDLER
		__raw_writel(on ? LGE_ERROR_HANDLER_MAGIC_NUM : 0,
				lge_error_handler_cookie_addr);
#endif
        //ZTE_RJG_RIL_20121214 begin
        //Add flag for sd dump
        __raw_writel((on && !ignore_sd_dump) ? SDDUMP_MAGIC_NUM : 0,
		       &(((struct boot_shared_imem_cookie_type *)dload_mode_addr)->err_fatal_magic));

        //ZTE_RJG_RIL_20121214 end
		mb();
	}
}

static int dload_set(const char *val, struct kernel_param *kp)
{
	int ret;
	int old_val = download_mode;

	ret = param_set_int(val, kp);

	if (ret)
		return ret;

	/* If download_mode is not zero or one, ignore. */
	if (download_mode >> 1) {
		download_mode = old_val;
		return -EINVAL;
	}

	set_dload_mode(download_mode);
#ifdef CONFIG_LGE_CRASH_HANDLER
	ssr_magic_number = 0;
#endif

	return 0;
}


static int dload_ignor_sd_dump_set(const char *val, struct kernel_param *kp)
{
	int ret;
	int old_val = ignore_sd_dump;
    pr_err("dload_ignor_sd_dump_set old ignore_sd_dump %d\n", ignore_sd_dump);

	ret = param_set_int(val, kp);
    pr_err("dload_ignor_sd_dump_set new ignore_sd_dump %d\n", ignore_sd_dump);

	if (ret)
		return ret;

	/* If ignor_sd_dump is not zero or one, ignore. */
	if (ignore_sd_dump >> 1) {
		ignore_sd_dump = old_val;
		return -EINVAL;
	}

    set_dload_mode(download_mode);
	return 0;
}
#else
#define set_dload_mode(x) do {} while (0)
#endif

void msm_set_restart_mode(int mode)
{
	restart_mode = mode;
#ifdef CONFIG_LGE_CRASH_HANDLER
	if (download_mode == 1 && (mode & 0xFFFF0000) == 0x6D630000)
		panic("LGE crash handler detected panic");
#endif
}
EXPORT_SYMBOL(msm_set_restart_mode);

//ZTE_RIL_RJG_20130709 begin
void msm_ignore_sd_dump(int enable)
{
	ignore_sd_dump = !!enable;
}
EXPORT_SYMBOL(msm_ignore_sd_dump);
//ZTE_RIL_RJG_20130709 end

static void __msm_power_off(int lower_pshold)
{
	printk(KERN_CRIT "Powering off the SoC\n");
#ifdef CONFIG_MSM_DLOAD_MODE
	set_dload_mode(0);
#endif
#ifdef CONFIG_ZTE_BATTERY_SWITCH
	if(1 == is_factory_mode)
		set_bs_poweroff();
#endif	
	pm8xxx_reset_pwr_off(0);

	if (lower_pshold) {
		__raw_writel(0, PSHOLD_CTL_SU);
		mdelay(10000);
		printk(KERN_ERR "Powering off has failed\n");
	}
	return;
}

static void msm_power_off(void)
{
	/* MSM initiated power off, lower ps_hold */
	__msm_power_off(1);
}

//static void cpu_power_off(void *data)
void cpu_power_off(void *data)
{
	int rc;

	pr_err("PMIC Initiated shutdown %s cpu=%d\n", __func__,
						smp_processor_id());
	if (smp_processor_id() == 0) {
		/*
		 * PMIC initiated power off, do not lower ps_hold, pmic will
		 * shut msm down
		 */
		__msm_power_off(0);

		pet_watchdog();
		pr_err("Calling scm to disable arbiter\n");
		/* call secure manager to disable arbiter and never return */
		rc = scm_call_atomic1(SCM_SVC_PWR,
						SCM_IO_DISABLE_PMIC_ARBITER, 1);

		pr_err("SCM returned even when asked to busy loop rc=%d\n", rc);
		pr_err("waiting on pmic to shut msm down\n");
	}

	preempt_disable();
	while (1)
		;
}

void msm_restart(char mode, const char *cmd);
static irqreturn_t resout_irq_handler(int irq, void *dev_id)
{
#if 0
	pr_warn("%s PMIC Initiated shutdown\n", __func__);
	oops_in_progress = 1;
	smp_call_function_many(cpu_online_mask, cpu_power_off, NULL, 0);
	if (smp_processor_id() == 0)
		cpu_power_off(NULL);
#else
   #ifdef CONFIG_ZTE_LONGPOWER_FOR_HW_RESET
	pr_info("%s PMIC Initiated reset\n", __func__);
	download_mode=0;
	msm_restart(0,0);
   #else
	pr_info("%s PMIC Initiated reset to dload mode\n", __func__);

	/*slf2012_0613 note:pm8921 will not care the PS_HOLD when stay on,so pull PS_HPLD
	to low cannot reset the handset */
	pm8xxx_stay_on();

	restart_mode = RESTART_DLOAD;
	download_mode=1;
	msm_restart(0,0);
   #endif
#endif
	preempt_disable();
	while (1)
		;
	return IRQ_HANDLED;
}

#ifdef CONFIG_LGE_CRASH_HANDLER
#define SUBSYS_NAME_MAX_LENGTH	40

int get_ssr_magic_number(void)
{
	return ssr_magic_number;
}

void set_ssr_magic_number(const char* subsys_name)
{
	int i;
	const char *subsys_list[] = {
		"modem", "riva", "dsps", "lpass",
		"external_modem", "gss",
	};

	ssr_magic_number = (0x6d630000 | 0x0000f000);

	for (i=0; i < ARRAY_SIZE(subsys_list); i++) {
		if (!strncmp(subsys_list[i], subsys_name,
					SUBSYS_NAME_MAX_LENGTH)) {
			ssr_magic_number = (0x6d630000 | ((i+1)<<12));
			break;
		}
	}
}

void set_kernel_crash_magic_number(void)
{
	pet_watchdog();
	if (ssr_magic_number == 0)
		__raw_writel(0x6d630100, restart_reason);
	else
		__raw_writel(restart_mode, restart_reason);
}
#endif /* CONFIG_LGE_CRASH_HANDLER */

void msm_restart(char mode, const char *cmd)
{

#ifdef CONFIG_MSM_DLOAD_MODE
    #if 0
	/* This looks like a normal reboot at this point. */
	set_dload_mode(0);

	/* Write download mode flags if we're panic'ing */
	set_dload_mode(in_panic);

	/* Write download mode flags if restart_mode says so */
	if (restart_mode == RESTART_DLOAD) {
		set_dload_mode(1);
#ifdef CONFIG_LGE_CRASH_HANDLER
		writel(0x6d63c421, restart_reason);
		goto reset;
#endif
	}

	/* Kill download mode if master-kill switch is set */
	if (!download_mode)
		set_dload_mode(0);
    #else
	if(restart_mode == RESTART_DLOAD)
		set_dload_mode(1);
	else if(download_mode)
		set_dload_mode(in_panic);
	else
		set_dload_mode(0);
    #endif
#endif

	printk(KERN_NOTICE "Going down for restart now\n");
#ifdef CONFIG_MACH_STORMER
/************add start for orise 1283 panel*********/
	gpio_direction_output(58,1);
	mdelay(10);
	gpio_direction_output(58,0);
	mdelay(20);
	gpio_direction_output(58,1);
	mdelay(50);
/************add end for orise 1283 panel*********/
#endif
	pm8xxx_reset_pwr_off(1);

	if (cmd != NULL) {
		if (!strncmp(cmd, "bootloader", 10)) {
			__raw_writel(0x77665500, restart_reason);
		} else if (!strncmp(cmd, "recovery", 8)) {
			__raw_writel(0x77665502, restart_reason);
		} else if (!strncmp(cmd, "oem-", 4)) {
			unsigned long code;
			code = simple_strtoul(cmd + 4, NULL, 16) & 0xff;
			__raw_writel(0x6f656d00 | code, restart_reason);
		} else if (!strncmp(cmd, "ftmmode", 7)) {
			/*ZTE_BOOT_20140114,support:adb reboot ftmmode*/
			__raw_writel(0x776655ee, restart_reason);
		}
		else {
			__raw_writel(0x77665501, restart_reason);
		}
	} else {
		__raw_writel(0x77665501, restart_reason);
	}
#ifdef CONFIG_LGE_CRASH_HANDLER
	if (in_panic == 1)
		set_kernel_crash_magic_number();
reset:
#endif /* CONFIG_LGE_CRASH_HANDLER */

	__raw_writel(0, msm_tmr0_base + WDT0_EN);
	if (!(machine_is_msm8x60_fusion() || machine_is_msm8x60_fusn_ffa())) {
		mb();
		__raw_writel(0, PSHOLD_CTL_SU); /* Actually reset the chip */
		mdelay(5000);
		pr_notice("PS_HOLD didn't work, falling back to watchdog\n");
	}

	__raw_writel(1, msm_tmr0_base + WDT0_RST);
	__raw_writel(5*0x31F3, msm_tmr0_base + WDT0_BARK_TIME);
	__raw_writel(0x31F3, msm_tmr0_base + WDT0_BITE_TIME);
	__raw_writel(1, msm_tmr0_base + WDT0_EN);

	mdelay(10000);
	printk(KERN_ERR "Restarting has failed\n");
}

static int __init msm_pmic_restart_init(void)
{
	int rc;

	if (pmic_reset_irq != 0) {
		rc = request_any_context_irq(pmic_reset_irq,
					resout_irq_handler, IRQF_TRIGGER_HIGH,
					"restart_from_pmic", NULL);
		if (rc < 0)
			pr_err("pmic restart irq fail rc = %d\n", rc);
	} else {
		pr_warn("no pmic restart interrupt specified\n");
	}

#ifdef CONFIG_LGE_CRASH_HANDLER
	__raw_writel(0x6d63ad00, restart_reason);
#endif

	return 0;
}

late_initcall(msm_pmic_restart_init);

static int __init msm_restart_init(void)
{
#ifdef CONFIG_MSM_DLOAD_MODE
	atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
	dload_mode_addr = MSM_IMEM_BASE + DLOAD_MODE_ADDR;
#ifdef CONFIG_LGE_CRASH_HANDLER
	lge_error_handler_cookie_addr = MSM_IMEM_BASE +
		LGE_ERROR_HANDLER_MAGIC_ADDR;
#endif
	set_dload_mode(download_mode);
#endif
	msm_tmr0_base = msm_timer_get_timer0_base();
	restart_reason = MSM_IMEM_BASE + RESTART_REASON_ADDR;
	pm_power_off = msm_power_off;

	return 0;
}
early_initcall(msm_restart_init);
