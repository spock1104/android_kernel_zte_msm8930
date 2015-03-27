#ifndef 	__BOARD_ZTE_TS__
#define 	__BOARD_ZTE_TS__


#if defined(CONFIG_MACH_DEMETER)//p893a30
#define SYN_TURLY_FW_NAME	"T83_Truly_ZTE_PR1380113-s2202_32323033.img" 
#define SYN_SUCCESS_FW_NAME	"ZTE_T83_Success_PR1380113-s2202_32333036.img"

#elif  defined(CONFIG_MACH_WARPLTE)
#if defined(CONFIG_ZTE_BOARD_WARPLTE_PCB_CZTX)//P893V10
#define SYN_TURLY_FW_NAME	"ZTE_P893V10_Truly_PR1183396-s2202_32323038.img" 
#define SYN_SUCCESS_FW_NAME	"ZTE_P893A21_Success-PR1469183-s2202_32333043.img"
#define FTC_LAIBAO_FW_NAME	"Ver13_P893A21_FT5306_0x55_20130814_app.bin"
#else //N9510
#define SYN_TPK_FW_NAME		"ZTE_P177F03_TPK_PR1183396-s2202_32313032.img"
#define SYN_WINTEK_FW_NAME	"ZTE_P177F03_Wintek_PR1183396-s2202_32363032.img"
#define SYN_LAIBAO_FW_NAME	"ZTE_P177F03_Laibao_PR1183396-s2202_32373039.img"
#define FTC_TURLY_FW_NAME	"Ver0F_P188F03_ID0x5A_FT5326_20130605_app.bin" 
#endif

#elif defined(CONFIG_MACH_APOLLO)//P893U10
#define SYN_ECW_FW_NAME		"ZTE_U893U10_ECW_PR1469183-s2202_32393042.img"
#define SYN_JUNDA_FW_NAME	"ZTE_P893U10_Junda_PR1183396-s2202_32453035.img"
#define FTC_MUDONG_FW_NAME	"V12_P893U10_ID0x53_FT5326_127C2_20130419_app.bin"

#elif defined(CONFIG_MACH_NEX)//N800
#define SYN_ECW_FW_NAME		"ZTE_U893U10_ECW_PR1469183-s2202_32393042.img"
#define SYN_JUNDA_FW_NAME	"ZTE_P893U10_Junda_PR1183396-s2202_32453035.img"
#define FTC_MUDONG_FW_NAME	"V12_P893U10_ID0x53_FT5326_127C2_20130419_app.bin"

#elif defined(CONFIG_MACH_IRIS)//P893D10
#define SYN_TURLY_FW_NAME	"Z753_Truly_PR1350177-s3203_zte_35323037.img" 
#define SYN_LAIBAO_FW_NAME	"P893D10_Laibao_PR1350177-s3203_ZTE_35373045.img"
#define SYN_BAOMING_FW_NAME	"Z753_P893D10_BaoMing_PR1450672_S3203_DS4_3.2.1.35_35423036.img"

#elif defined (CONFIG_MACH_COEUS) || defined (CONFIG_MACH_HERA)|| defined (CONFIG_MACH_OCEANUS)//P893A21 P893C01  P893V22
#define SYN_SUCCESS_FW_NAME "ZTE_P893A21_Success-PR1469183-s2202_32333043.img"
#define SYN_GOWORLD_FW_NAME	"ZTE_P893C01_Goworld-PR1469183-s2202_32413033.img"
#define FTC_LAIBAO_FW_NAME	"Ver13_P893A21_FT5306_0x55_20130814_app.bin"

#elif defined(CONFIG_MACH_METIS)//p823a01
#define FTC_LAIBAO_FW_NAME	"Ver14_N880E_ID0x55_20130604_app.bin"	
#define FTC_GOWORLD_FW_NAME "Ver0B_20130725_P823A01_10883A_ID0x57_app.bin"

#else
#define SYN_TPK_FW_NAME		""
#define SYN_TURLY_FW_NAME	"" 
#define SYN_SUCCESS_FW_NAME	""
#define SYN_OFILM_FW_NAME	""
#define SYN_LEAD_FW_NAME	""
#define SYN_WINTEK_FW_NAME	""
#define SYN_LAIBAO_FW_NAME	""
#define SYN_CMI_FW_NAME		""
#define SYN_ECW_FW_NAME		""
#define SYN_GOWORLD_FW_NAME	""
#define SYN_BAOMING_FW_NAME	""
#define SYN_JUNDA_FW_NAME	""

#define FTC_TPK_FW_NAME		""
#define FTC_TURLY_FW_NAME	"" 
#define FTC_SUCCESS_FW_NAME	""
#define FTC_OFILM_FW_NAME	""
#define FTC_LEAD_FW_NAME	""
#define FTC_WINTEK_FW_NAME	""
#define FTC_LAIBAO_FW_NAME	""
#define FTC_CMI_FW_NAME		""
#define FTC_ECW_FW_NAME		""
#define FTC_GOWORLD_FW_NAME	""
#define FTC_BAOMING_FW_NAME	""
#define FTC_JUNDA_FW_NAME	""
#define FTC_JIAGUAN_FW_NAME	""
#define FTC_MUDONG_FW_NAME	""

#endif

#ifndef SYN_TPK_FW_NAME
#define SYN_TPK_FW_NAME		""
#endif
#ifndef SYN_TURLY_FW_NAME
#define SYN_TURLY_FW_NAME	"" 
#endif
#ifndef SYN_SUCCESS_FW_NAME
#define SYN_SUCCESS_FW_NAME	"" 
#endif
#ifndef SYN_OFILM_FW_NAME
#define SYN_OFILM_FW_NAME	"" 
#endif
#ifndef SYN_LEAD_FW_NAME
#define SYN_LEAD_FW_NAME	"" 
#endif
#ifndef SYN_WINTEK_FW_NAME
#define SYN_WINTEK_FW_NAME	"" 
#endif
#ifndef SYN_LAIBAO_FW_NAME
#define SYN_LAIBAO_FW_NAME	"" 
#endif
#ifndef SYN_CMI_FW_NAME
#define SYN_CMI_FW_NAME		"" 
#endif
#ifndef SYN_ECW_FW_NAME
#define SYN_ECW_FW_NAME		"" 
#endif
#ifndef SYN_GOWORLD_FW_NAME
#define SYN_GOWORLD_FW_NAME	"" 
#endif
#ifndef SYN_BAOMING_FW_NAME
#define SYN_BAOMING_FW_NAME	"" 
#endif
#ifndef SYN_JUNDA_FW_NAME
#define SYN_JUNDA_FW_NAME	"" 
#endif
#ifndef FTC_TPK_FW_NAME
#define FTC_TPK_FW_NAME		""
#endif
#ifndef FTC_TURLY_FW_NAME
#define FTC_TURLY_FW_NAME	"" 
#endif
#ifndef FTC_SUCCESS_FW_NAME
#define FTC_SUCCESS_FW_NAME	"" 
#endif
#ifndef FTC_OFILM_FW_NAME
#define FTC_OFILM_FW_NAME	"" 
#endif
#ifndef FTC_LEAD_FW_NAME
#define FTC_LEAD_FW_NAME	"" 
#endif
#ifndef FTC_WINTEK_FW_NAME
#define FTC_WINTEK_FW_NAME	"" 
#endif
#ifndef FTC_LAIBAO_FW_NAME
#define FTC_LAIBAO_FW_NAME	"" 
#endif
#ifndef FTC_CMI_FW_NAME
#define FTC_CMI_FW_NAME		"" 
#endif
#ifndef FTC_ECW_FW_NAME
#define FTC_ECW_FW_NAME		"" 
#endif
#ifndef FTC_GOWORLD_FW_NAME
#define FTC_GOWORLD_FW_NAME	"" 
#endif
#ifndef FTC_BAOMING_FW_NAME
#define FTC_BAOMING_FW_NAME	"" 
#endif
#ifndef FTC_JUNDA_FW_NAME
#define FTC_JUNDA_FW_NAME	"" 
#endif
#ifndef FTC_JIAGUAN_FW_NAME
#define FTC_JIAGUAN_FW_NAME	"" 
#endif
#ifndef FTC_MUDONG_FW_NAME
#define FTC_MUDONG_FW_NAME	"" 
#endif
/*
固件名按照这个顺序排列
*/
enum TOUCH_MOUDLE
{
	TPK=0,
	TRULY,
	SUCCESS,
	OFILM,
	LEAD,
	WINTEK,
	LAIBAO,
	CMI,
	ECW,
	GOWORLD,
	BAOMING,
	JUNDA,
	JIAGUAN,
	MUDONG,
	UNKNOW=0xff
};
#define SYN_MOUDLE_NUM_MAX 12
#define FTC_MOUDLE_NUM_MAX 14


#endif

