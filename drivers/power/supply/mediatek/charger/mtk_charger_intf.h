/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */
#ifndef __MTK_CHARGER_INTF_H__
#define __MTK_CHARGER_INTF_H__

#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/switch.h>
#include <mt-plat/charger_type.h>
#include <mt-plat/mtk_charger.h>
#include <mt-plat/mtk_battery.h>

#include <mtk_gauge_time_service.h>

#include <mt-plat/charger_class.h>

/* PD */
#include <tcpm.h>

struct charger_manager;
#include "mtk_pe_intf.h"
#include "mtk_pe20_intf.h"
#include "mtk_pe40_intf.h"
#include "mtk_pdc_intf.h"

#define CHARGING_INTERVAL 10
#define CHARGING_FULL_INTERVAL 20

#define CHRLOG_ERROR_LEVEL   1
#define CHRLOG_DEBUG_LEVEL   2

extern int chr_get_debug_level(void);

#define chr_err(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_ERROR_LEVEL) {	\
		pr_notice(fmt, ##args);				\
	}							\
} while (0)

#define chr_info(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_ERROR_LEVEL) {	\
		pr_notice_ratelimited(fmt, ##args);		\
	}							\
} while (0)

#define chr_debug(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_DEBUG_LEVEL) {	\
		pr_notice(fmt, ##args);				\
	}							\
} while (0)

#define CHR_CC		(0x0001)
#define CHR_TOPOFF	(0x0002)
#define CHR_TUNING	(0x0003)
#define CHR_POSTCC	(0x0004)
#define CHR_BATFULL	(0x0005)
#define CHR_ERROR	(0x0006)
#define	CHR_PE40_INIT	(0x0007)
#define	CHR_PE40_CC	(0x0008)
#define	CHR_PE40_TUNING	(0x0009)
#define	CHR_PE40_POSTCC	(0x000A)
#define CHR_PE30	(0x000B)

#define BCM_FLAG_NONE              0
#define BCM_FLAG_CHARGE_TIMER      BIT(0)

/* charger_algorithm notify charger_dev */
enum {
	EVENT_EOC,
	EVENT_RECHARGE,
};

/* charger_dev notify charger_manager */
enum {
	CHARGER_DEV_NOTIFY_VBUS_OVP,
	CHARGER_DEV_NOTIFY_BAT_OVP,
	CHARGER_DEV_NOTIFY_EOC,
	CHARGER_DEV_NOTIFY_RECHG,
	CHARGER_DEV_NOTIFY_SAFETY_TIMEOUT,
	CHARGER_DEV_NOTIFY_VBUS_EVENT,
};

/*
 * Software JEITA
 * T0: -10 degree Celsius
 * T1: 0 degree Celsius
 * T2: 10 degree Celsius
 * T3: 45 degree Celsius
 * T4: 50 degree Celsius
 */
enum sw_jeita_state_enum {
	TEMP_BELOW_T0 = 0,
	TEMP_T0_TO_T1,
	TEMP_T1_TO_T2,
	TEMP_T2_TO_T3,
	TEMP_T3_TO_T4,
	TEMP_ABOVE_T4
};

enum adapter_power_category {
	ADAPTER_5W = 0,
	ADAPTER_7_5W,
	ADAPTER_9W,
	ADAPTER_12W,
	ADAPTER_15W,
};

struct sw_jeita_data {
	int sm;
	int pre_sm;
	int cv;
	bool charging;
	bool error_recovery_flag;
};

/* battery thermal protection */
enum bat_temp_state_enum {
	BAT_TEMP_LOW = 0,
	BAT_TEMP_NORMAL,
	BAT_TEMP_HIGH
};

enum vbus_voltage_state {
	VBUS_NORMAL = 0,
	VBUS_OVP,
	VBUS_UVLO
};

struct battery_thermal_protection_data {
	int sm;
	bool enable_min_charge_temp;
	int min_charge_temp;
	int min_charge_temp_plus_x_degree;
	int max_charge_temp;
	int max_charge_temp_minus_x_degree;
};

struct charger_custom_data {
	int battery_cv;	/* uv */
#ifdef CONFIG_MTK_USE_AGING_ZCV
	int battery_cv_aging1;	/* uv */
	int battery_cv_aging2;	/* uv */
#endif
	int max_charger_voltage;
	int max_charger_voltage_setting;
	int min_charger_voltage;

	int usb_charger_current_suspend;
	int usb_charger_current_unconfigured;
	int usb_charger_current_configured;
	int usb_charger_current;
	int ac_charger_current;
	int ac_charger_input_current;
	int non_std_ac_charger_current;
	int charging_host_charger_current;
	int apple_1_0a_charger_current;
	int apple_2_1a_charger_current;
	int ta_ac_charger_current;
	int pd_charger_current;
	int wireless_5w_charger_input_current;
	int wireless_5w_charger_current;
	int wireless_10w_charger_input_current;
	int wireless_10w_charger_current;
	int wireless_default_charger_input_current;
	int wireless_default_charger_current;

	/* sw jeita */
	int jeita_temp_above_t4_cv;
	int jeita_temp_t3_to_t4_cv;
	int jeita_temp_t2_to_t3_cv;
	int jeita_temp_t1_to_t2_cv;
	int jeita_temp_t0_to_t1_cv;
	int jeita_temp_below_t0_cv;
#ifdef CONFIG_MTK_USE_AGING_ZCV
	int jeita_temp_above_t4_cv_voltage_aging1;
	int jeita_temp_t3_to_t4_cv_voltage_aging1;
	int jeita_temp_t2_to_t3_cv_voltage_aging1;
	int jeita_temp_t1_to_t2_cv_voltage_aging1;
	int jeita_temp_t0_to_t1_cv_voltage_aging1;
	int jeita_temp_below_t0_cv_voltage_aging1;

	int jeita_temp_above_t4_cv_voltage_aging2;
	int jeita_temp_t3_to_t4_cv_voltage_aging2;
	int jeita_temp_t2_to_t3_cv_voltage_aging2;
	int jeita_temp_t1_to_t2_cv_voltage_aging2;
	int jeita_temp_t0_to_t1_cv_voltage_aging2;
	int jeita_temp_below_t0_cv_voltage_aging2;
#endif
	int temp_t4_thres;
	int temp_t4_thres_minus_x_degree;
	int temp_t3_thres;
	int temp_t3_thres_minus_x_degree;
	int temp_t2_thres;
	int temp_t2_thres_plus_x_degree;
	int temp_t1_thres;
	int temp_t1_thres_plus_x_degree;
	int temp_t0_thres;
	int temp_t0_thres_plus_x_degree;
	int temp_neg_10_thres;

	int temp_t0_to_t1_charger_current;
	int temp_t3_to_t4_charger_current;
	/* battery temperature protection */
	int mtk_temperature_recharge_support;
	int max_charge_temp;
	int max_charge_temp_minus_x_degree;
	int min_charge_temp;
	int min_charge_temp_plus_x_degree;

	/* pe */
	int pe_ichg_level_threshold;	/* ma */
	int ta_ac_12v_input_current;
	int ta_ac_9v_input_current;
	int ta_ac_7v_input_current;
	bool ta_12v_support;
	bool ta_9v_support;

	/* pe2.0 */
	int pe20_ichg_level_threshold;	/* ma */
	int ta_start_battery_soc;
	int ta_stop_battery_soc;

	/* pe4.0 */
	int pe40_single_charger_input_current;	/* ma */
	int pe40_single_charger_current;
	int pe40_dual_charger_input_current;
	int pe40_dual_charger_chg1_current;
	int pe40_dual_charger_chg2_current;
	int pe40_stop_battery_soc;

	/* pe4.0 cable impedance threshold (mohm) */
	u32 pe40_r_cable_1a_lower;
	u32 pe40_r_cable_2a_lower;
	u32 pe40_r_cable_3a_lower;

	/* dual charger */
	u32 chg1_ta_ac_charger_current;
	u32 chg2_ta_ac_charger_current;

	/* cable measurement impedance */
	int cable_imp_threshold;
	int vbat_cable_imp_threshold;

	/* bif */
	int bif_threshold1;	/* uv */
	int bif_threshold2;	/* uv */
	int bif_cv_under_threshold2;	/* uv */

	/* power path */
	bool power_path_support;

	int max_charging_time; /* second */
};

struct charger_data {
	int force_charging_current;
	int thermal_input_current_limit;
	int thermal_input_power_limit;
	int thermal_charging_current_limit;
	int input_current_limit;
	int charging_current_limit;
	int disable_charging_count;
	int input_current_limit_by_aicl;
	int junction_temp_min;
	int junction_temp_max;
	int force_input_current_limit;
};

struct power_detection_data {
	bool en;
	int iusb_ua;
	int type;
	int adapter_9w_aicl_min;
	int adapter_12w_aicl_min;
	int adapter_5w_iusb_lim;
	int adapter_9w_iusb_lim;
	int adapter_12w_iusb_lim;
	int aicl_trigger_iusb;
	int aicl_trigger_ichg;
};

#if defined(CONFIG_SW_AICL_SUPPORT)
struct sw_aicl_data {
	/* SW AICL detection */
	bool  aicl_done;
	int  aicl_result;
	int  iusb_ua;
	int  ibat_ua;
	bool aicl_enable;
	int aicl_charging_current_max;
	int aicl_input_current_max;
	int aicl_input_current_min;
	int aicl_step_current;
	int aicl_step_interval;
	int aicl_vbus_valid;
	int aicl_vbus_state_phase;
};
#endif

struct charger_manager {
	bool init_done;
	const char *algorithm_name;
	struct platform_device *pdev;
	void	*algorithm_data;
	int usb_state;
	bool usb_unlimited;
	bool disable_charger;
	unsigned int bcm_flag;

	struct charger_device *chg1_dev;
	struct notifier_block chg1_nb;
	struct charger_data chg1_data;
	struct charger_consumer *chg1_consumer;

	struct charger_device *chg2_dev;
	struct notifier_block chg2_nb;
	struct charger_data chg2_data;

	enum charger_type chr_type;
	bool can_charging;

	int (*do_algorithm)(struct charger_manager *cm);
	int (*plug_in)(struct charger_manager *cm);
	int (*plug_out)(struct charger_manager *cm);
	int (*do_charging)(struct charger_manager *cm, bool en);
	int (*do_event)(struct notifier_block *nb, unsigned long ev, void *v);
	int (*change_current_setting)(struct charger_manager *cm);

	/* notify charger user */
	struct srcu_notifier_head evt_nh;
	/* receive from battery */
	struct notifier_block psy_nb;

	/* common info */
	int battery_temp;

	/* sw jeita */
	bool enable_sw_jeita;
	struct sw_jeita_data sw_jeita;

	/* dynamic_cv */
	bool enable_dynamic_cv;

	bool cmd_discharging;
	bool safety_timeout;
	bool vbusov_stat;

	/* battery warning */
	unsigned int notify_code;
	unsigned int notify_test_mode;

	/* battery thermal protection */
	struct battery_thermal_protection_data thermal;

	/* dtsi custom data */
	struct charger_custom_data data;

	bool enable_sw_safety_timer;
	bool sw_safety_timer_setting;
	bool thermal_throttling_by_psy;
	bool dcap_support;
	bool dcap_enable;

	/* High voltage charging */
	bool enable_hv_charging;

	/* pe */
	bool enable_pe_plus;
	struct mtk_pe pe;

	/* pe 2.0 */
	bool enable_pe_2;
	struct mtk_pe20 pe2;

	/* pe 4.0 */
	bool enable_pe_4;
	struct mtk_pe40 pe4;

	/* type-C*/
	bool enable_type_c;

	/* pd */
	struct mtk_pdc pdc;

	int pd_type;
	struct tcpc_device *tcpc;
	struct notifier_block pd_nb;
	bool pd_reset;

	/* thread related */
	struct hrtimer charger_kthread_timer;
	struct gtimer charger_kthread_fgtimer;

	struct wakeup_source charger_wakelock;
	struct mutex charger_lock;
	struct mutex charger_pd_lock;
	spinlock_t slock;
	unsigned int polling_interval;
	bool charger_thread_timeout;
	wait_queue_head_t  wait_que;
	bool charger_thread_polling;

	/* kpoc */
	atomic_t enable_kpoc_shdn;
	/* top-off mode */
	int custom_charging_cv;
	int top_off_mode_cv;
#ifdef CONFIG_MTK_USE_AGING_ZCV
	int top_off_mode_cv_aging1;
	int top_off_mode_cv_aging2;
#endif
	unsigned int top_off_mode_enable; /* 0=ratail unit, 1=demo unit */
	/* ATM */
	bool atm_enabled;

	unsigned long top_off_difference_full_cv;
	unsigned long normal_difference_full_cv;

	/* Adapter Power Detection */
	struct power_detection_data power_detection;
#if defined(CONFIG_SW_AICL_SUPPORT)
	struct sw_aicl_data sw_aicl;
#endif
	struct wakeup_source cable_wakelock;

	/* Enable the feature detect bad charger */
	bool enable_bat_eoc_protect;
	bool bat_eoc_protect;
	uint32_t soc_exit_eoc;

	struct timespec disconnect_time;
	__kernel_time_t disconnect_duration;
	__kernel_time_t bat_eoc_protect_reset_time;
	__kernel_time_t backup_bat_eoc_protect_reset_time;

	/* For SW UVLO/OVP protection */
	int vbus_state;
	bool vbus_recovery_from_uvlo;

	/* For invalid charger det */
	bool invalid_charger_det_done;
	struct switch_dev invalid_charger;

	/* For INDPM mode workround */
	bool dpm_disable_charging;
	int soc_exit_dpm_eoc;
	int cv_enter_dpm_eoc;
	int dpm_state_count;
};

/* charger related module interface */
extern int charger_manager_notifier(struct charger_manager *info, int event);
extern int mtk_switch_charging_init(struct charger_manager *info);
extern int mtk_dual_switch_charging_init(struct charger_manager *info);
extern int mtk_linear_charging_init(struct charger_manager *info);
extern void _wake_up_charger(struct charger_manager *info);
extern int mtk_get_dynamic_cv(struct charger_manager *info, unsigned int *cv);
extern bool is_dual_charger_supported(struct charger_manager *info);
extern int charger_enable_vbus_ovp(struct charger_manager *pinfo, bool enable);
extern bool is_typec_adapter(struct charger_manager *info);
extern bool is_charger_invalid(void);
extern int mtk_get_battery_cv(struct charger_manager *info);

/* pmic API */
extern unsigned int upmu_get_rgs_chrdet(void);
extern int pmic_get_vbus(void);
extern int pmic_get_charging_current(void);
extern int pmic_get_battery_voltage(void);
extern int pmic_get_bif_battery_voltage(int *vbat);
extern int pmic_is_bif_exist(void);
extern int pmic_enable_hw_vbus_ovp(bool enable);
extern bool pmic_is_battery_exist(void);

/* FIXME */
enum usb_state_enum {
	USB_SUSPEND = 0,
	USB_UNCONFIGURED,
	USB_CONFIGURED
};

bool __attribute__((weak)) is_usb_rdy(void)
{
	return true;
}

/* procfs */
#define PROC_FOPS_RW(name)						\
static int mtk_chg_##name##_open(struct inode *node, struct file *file)	\
{									\
	return single_open(file, mtk_chg_##name##_show, PDE_DATA(node));\
}									\
static const struct file_operations mtk_chg_##name##_fops = {		\
	.owner = THIS_MODULE,						\
	.open = mtk_chg_##name##_open,					\
	.read = seq_read,						\
	.llseek = seq_lseek,						\
	.release = single_release,					\
	.write = mtk_chg_##name##_write,				\
}

#endif /* __MTK_CHARGER_INTF_H__ */
