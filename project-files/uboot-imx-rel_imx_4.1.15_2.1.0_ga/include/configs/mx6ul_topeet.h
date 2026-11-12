/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX6UL board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __MX6UL_TOPEET_CONFIG_H
#define __MX6UL_TOPEET_CONFIG_H


#include <asm/arch/imx-regs.h>
#include <linux/sizes.h>
#include <asm/arch/imx6-pinfunc.h>
#include "mx6_common.h"
#include <asm/mach-imx/gpio.h>

#ifdef CONFIG_SYS_BOOT_NAND
#define CONFIG_NAND_MXS
#define CONFIG_CMD_NAND
#endif


#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART2_BASE

/* add by lsb 20190903 */
#define CONFIG_DISPLAY_LCD_TYPE		"1024x600"

/* end add */

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x1F000000)

/* SPL options */
#include "imx6_spl.h"

#define CONFIG_SYS_FSL_USDHC_NUM	2
#define CONFIG_SYS_FSL_ESDHC_ADDR	0

#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART2_BASE

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* Firmware version */
#define CONFIG_SYS_FSL_SEC_COMPAT	CAAM

#define CONFIG_FSL_USDHC

#define CONFIG_SYS_LDB_OFFSET		0
#define CONFIG_SYS_LDB_WIDTH		24

#define CONFIG_DISPLAY_LDB_OFFSET	0
#define CONFIG_DISPLAY_LDB_WIDTH	24

#ifndef CONFIG_DISPLAY_LCD_TYPE
#define CONFIG_DISPLAY_LCD_TYPE "1024x600"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	"lcdtype=1024x600\0" \
	"panel=TFT43AB\0" \
	"fdt_addr=0x83000000\0" \
	"fdt_high=0xffffffff\0"	  \
	"console=ttymxc1\0" \
	"splashpos=m,m\0" \
	"bootargs=console=ttymxc1,115200 "  \
		"root=ubi0:rootfs rootfstype=ubifs "		     \
		"ubi.mtd=3 rootwait=1"  \
	"bootcmd=nand read ${loadaddr} 0x4000000 0x800000;"\
		"nand read ${fdt_addr} 0x6000000 0x100000;"\
		"bootz ${loadaddr} - ${fdt_addr}\0" \
	"bootm_size=0x10000000\0" \
	"ipaddr=192.168.1.103\0" \
	"ethact=FEC0\0" \

#else
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	"script=boot.scr\0" \
	"lcdtype=1024x600\0" \
	"image=zImage\0" \
	"console=ttymxc1\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_file=undefined\0" \
	"fdt_addr=0x83000000\0" \
	"boot_fdt=try\0" \
	"ip_dyn=yes\0" \
	"splashimage=0x8c000000\0" \
	"splashpos=m,m\0" \
	"mmcdev=" __stringify(CONFIG_SYS_MMC_ENV_DEV) "\0" \
	"mmcpart=1\0" \
	"mmcroot=/dev/mmcblk1p2 rootwait rw\0" \
	"mmcfatdev=0:1\0" \
	"mmckernelload=fatload mmc ${mmcfatdev} ${loadaddr} ${image}\0" \
	"mmcdtbload=fatload mmc ${mmcfatdev} ${fdt_addr} ${fdt_file}\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot}\0" \
	"loadbootscript=" \
		"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=mmc dev ${mmcdev}; if mmc rescan; then " \
		"if run loadbootscript; then " \
		"run bootscript; " \
		"else " \
		"if run mmckernelload && run mmcdtbload; then " \
		"if run loadfdt; then " \
		"run mmcboot; " \
		"else " \
		"run netboot; " \
		"fi; " \
		"fi; " \
		"fi; " \
		"fi;\0" \
	"loadfdt=mmc dev ${mmcdev}; if mmc rescan; then " \
		"if run mmcdtbload; then " \
		"echo loaded ${fdt_file}; " \
		"else " \
		"echo WARN: Cannot load the DT; " \
		"fi; " \
		"fi;\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
		"if run loadfdt; then " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"else " \
		"if test ${boot_fdt} = try; then " \
		"bootz; " \
		"else " \
		"echo WARN: Cannot load the DT; " \
		"fi; " \
		"fi; " \
		"else " \
		"bootz; " \
		"fi;\0" \
	"netargs=setenv bootargs console=${console},${baudrate} " \
		"root=/dev/nfs " \
		"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
		"netboot=echo Booting from net ...; " \
		"run netargs; " \
		"if test ${ip_dyn} = yes; then " \
		"setenv get_cmd dhcp; " \
		"else " \
		"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${image}; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
		"if ${get_cmd} ${fdt_addr} ${fdt_file}; then " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"else " \
		"if test ${boot_fdt} = try; then " \
		"bootz; " \
		"else " \
		"echo WARN: Cannot load the DT; " \
		"fi; " \
		"fi; " \
		"else " \
		"bootz; " \
		"fi;\0" \

#define CONFIG_BOOTCOMMAND \
	"run loadimage; "

/* Miscellaneous configurable options */
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x1F000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		256

#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_CMD_GREPENV

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR
#define PHYS_SDRAM_SIZE			SZ_512M

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	0x00008000

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* environment organization */
#ifdef CONFIG_SYS_BOOT_NAND
#define CONFIG_ENV_SIZE			SZ_128K
#define CONFIG_ENV_OFFSET		0x400000
#else
#define CONFIG_ENV_SIZE			SZ_8K
#define CONFIG_ENV_OFFSET		0x80000
#endif

/* USB Configs */
#define CONFIG_CMD_USB
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_MXC_USB_PORTSC		(PTS_EHCI)
#define CONFIG_MXC_USB_PORT		1
#endif

#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_FEC_MXC_PHYADDR		0x0

#define CONFIG_MII
#define CONFIG_MII_DEFAULT		0

#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_SERVERIP			192.168.1.100
#define CONFIG_IPADDR			192.168.1.103

#define CONFIG_ARP_TIMEOUT		200UL

/* Miscellaneous configurable options */
#define CONFIG_SYS_CONSOLE_INFO_QUIET
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x1F000000)

/* MMC Configs */
#ifdef CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_CDNET_INSTALLED
#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC2_BASE_ADDR

/* NAND pin config */
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_TRIMFFS

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_UBI
#define CONFIG_RBTREE

#if defined(CONFIG_SYS_BOOT_NAND)
#define CONFIG_SYS_BOOT_NAND_SUPPORT
#endif

#define CONFIG_APBH_DMA
#define CONFIG_APBH_DMA_BURST
#define CONFIG_APBH_DMA_BURST8

#endif

#define CONFIG_ENV_IS_IN_MMC

#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#define CONFIG_SYS_I2C_SPEED		100000

#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC boot */

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

/* Framebuffer and LCD */
#define CONFIG_VIDEO
#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_VIDEO_MXS
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_CMD_BMP
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP

#define CONFIG_ETHPRIME			"FEC"

#ifndef CONFIG_DISPLAY_LCD_TYPE
#define CONFIG_DISPLAY_LCD_TYPE "1024x600"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	"script=boot.scr\0" \
	"lcdtype=1024x600\0" \
	"image=zImage\0" \
	"console=ttymxc1\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_file=undefined\0" \
	"fdt_addr=0x83000000\0" \
	"boot_fdt=try\0" \
	"ip_dyn=yes\0" \
	"splashimage=0x8c000000\0" \
	"splashpos=m,m\0" \
	"mmcdev=" __stringify(CONFIG_SYS_MMC_ENV_DEV) "\0" \
	"mmcpart=1\0" \
	"mmcroot=/dev/mmcblk1p2 rootwait rw\0" \
	"mmcfatdev=0:1\0" \
	"mmckernelload=fatload mmc ${mmcfatdev} ${loadaddr} ${image}\0" \
	"mmcdtbload=fatload mmc ${mmcfatdev} ${fdt_addr} ${fdt_file}\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot}\0" \
	"loadbootscript=" \
		"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=mmc dev ${mmcdev}; if mmc rescan; then " \
		"if run loadbootscript; then " \
		"run bootscript; " \
		"else " \
		"if run mmckernelload && run mmcdtbload; then " \
		"if run loadfdt; then " \
		"run mmcboot; " \
		"else " \
		"run netboot; " \
		"fi; " \
		"fi; " \
		"fi; " \
		"fi;\0" \
	"loadfdt=mmc dev ${mmcdev}; if mmc rescan; then " \
		"if run mmcdtbload; then " \
		"echo loaded ${fdt_file}; " \
		"else " \
		"echo WARN: Cannot load the DT; " \
		"fi; " \
		"fi;\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
		"if run loadfdt; then " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"else " \
		"if test ${boot_fdt} = try; then " \
		"bootz; " \
		"else " \
		"echo WARN: Cannot load the DT; " \
		"fi; " \
		"fi; " \
		"else " \
		"bootz; " \
		"fi;\0" \
	"netargs=setenv bootargs console=${console},${baudrate} " \
		"root=/dev/nfs " \
		"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
		"netboot=echo Booting from net ...; " \
		"run netargs; " \
		"if test ${ip_dyn} = yes; then " \
		"setenv get_cmd dhcp; " \
		"else " \
		"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${image}; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
		"if ${get_cmd} ${fdt_addr} ${fdt_file}; then " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"else " \
		"if test ${boot_fdt} = try; then " \
		"bootz; " \
		"else " \
		"echo WARN: Cannot load the DT; " \
		"fi; " \
		"fi; " \
		"else " \
		"bootz; " \
		"fi;\0" \

#define CONFIG_BOOTCOMMAND \
	"run loadimage; "

/* Miscellaneous configurable options */
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x1F000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#define CONFIG_CMDLINE_EDITING

/* USB Configs */
#define CONFIG_CMD_USB
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_MXC_USB_PORTSC		(PTS_EHCI)
#define CONFIG_MXC_USB_PORT		1
#endif

#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_SYS_BOOT_RAMDISK_HIGH

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

#endif	/*__MX6UL_TOPEET_CONFIG_H */
