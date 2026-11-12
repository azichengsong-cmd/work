# i.MX6ULL 项目需求与完成计划

## 一、项目背景

- 开发板：北京迅为 i.MX6ULL（商业级核心板，512MB DDR3 + 8GB eMMC）
- 软件基础：NXP 原厂 SDK
  - uboot：`sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga`
  - kernel：`sdk/linux-imx-rel_imx_4.1.15_2.1.0_ga`（4.1.15）
- 交叉编译器：`arm-linux-gnueabihf-gcc` 4.9.4
  - 路径：`app/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin`
  - 环境脚本：`app/mkenv.sh`
- 显示：RGB 1024x600，对应设备树 `topeet_emmc_1024x600.dtb`

## 二、需求总表

| 编号 | 需求 | 主要涉及模块 | 当前状态 |
| --- | --- | --- | --- |
| 1 | uboot 下添加 logo 显示 | uboot | 代码已改，待板端验证 |
| 2 | 修改调试串口为 UART2 | uboot + 内核设备树 | 代码已改，待板端验证 |
| 3 | DDR 压力测试 | uboot | 代码已改，待板端验证 |
| 4 | 调试 RS485 接口 | 内核设备树 + 应用 | 测试程序已编，待板端验证 |
| 5 | 适配 4G 模块 EC20 | 内核/驱动 + 应用 | 待开始 |
| 6 | 对 eMMC 添加 data 分区（50MB） | 分区表/文件系统 | 待开始 |
| 7 | 两路网口 RJ45 正常通信 | 内核设备树/驱动 | 待开始 |
| 8 | LCD RGB 屏 1024x600 正常显示 logo（内核中实现） | 内核设备树 | 待开始 |
| 9 | 触摸屏 GT911 驱动移植 | 内核驱动 + 设备树 | 待开始 |
| 10 | HDMI 正常显示 | 内核/uboot | 待开始 |
| 11 | UVC 摄像头图像正常显示 | 内核配置 + 应用 | 待开始 |
| 12 | 时钟芯片 RX8010 时间值正常 | 内核驱动 + 设备树 | 待开始 |
| 13 | TF 卡可正常读写 | 内核设备树/驱动 | 待开始 |
| 14 | WIFI 驱动移植 RTL8723BU | 内核驱动 + 固件 | 待开始 |
| 15 | 若干 GPIO 可控制（export）：GPIO_0、GPIO_5、UART1_CTS | 设备树/内核配置 | 待开始 |
| 16 | 文件系统中添加中文支持 | 文件系统（buildroot） | 待开始 |
| 17 | 系统优化：优化启动时间、裁剪多余驱动 | uboot + 内核 + 文件系统 | 待开始 |

说明：编号顺序按需求原文排列；“当前状态”以本文档最后更新时间为准。

## 三、前四项需求的实现说明

### 3.1 uboot 添加 logo 显示（第 1 项）

**原因**：uboot 视频功能（`CONFIG_VIDEO`、`CONFIG_VIDEO_LOGO`、`CONFIG_SPLASH_SCREEN`、`CONFIG_CMD_BMP`）本身已经打开，
但默认环境变量 `lcdtype=hdmi`，而板级 `displays[]` 里没有 hdmi 面板，`board_video_skip()` 返回错误后视频初始化被跳过，所以不显示 logo。

**修改内容**：

| 文件 | 修改 |
| --- | --- |
| `include/configs/mx6ul_topeet.h` | `CONFIG_DISPLAY_LCD_TYPE` 由 `"hdmi"` 改为 `"1024x600"` |
| `include/configs/mx6ul_topeet.h` | NAND 默认环境 `lcdtype=hdmi` 改为 `lcdtype=1024x600`；eMMC 默认环境新增 `lcdtype=1024x600` |

**效果**：uboot 启动时初始化 1024x600 面板，并把内置 logo（`tools/logos/freescale.bmp`，480x272）按 `splashpos=m,m` 居中显示；
同时 uboot 会根据 `lcdtype` 自动设置 `fdt_file=topeet_emmc_1024x600.dtb`。

**若要换成自己的全屏 logo**：把图片转成 8 位调色板 BMP（1024x600）替换 `tools/logos/freescale.bmp`，重新编译 uboot 即可。
转换命令示例：

```bash
convert logo.png -resize 1024x600! -colors 256 BMP3:freescale.bmp
```

### 3.2 修改调试串口为 UART2（第 2 项）

**原因**：uboot 的调试串口基地址、引脚、默认 console 都指向 UART1；内核的 `stdout-path` 也指向 uart1。
另外 UART2 的 RX 引脚（`GPIO1_IO21`）与板上的红外接收节点 `ir_recv` 冲突，必须二选一。

**修改内容**：

| 文件 | 修改 |
| --- | --- |
| `include/configs/mx6ul_topeet.h` | `CONFIG_MXC_UART_BASE` 改为 `UART2_BASE` |
| `include/configs/mx6ul_topeet.h` | 默认环境 `console=ttymxc0` 改为 `console=ttymxc1` |
| `board/freescale/mx6ul_topeet/mx6ul_topeet.c` | 新增 `uart2_pads[]`，`setup_iomux_uart()` 同时配置 UART1/UART2 引脚 |
| `arch/arm/boot/dts/imx6ull-14x14-evk.dts` | `stdout-path` 改为 `&uart2` |
| 同上 | `pinctrl_uart2` 删除借用 UART3 的两个引脚（RTS/CTS） |
| 同上 | `&uart2` 删除 `fsl,uart-has-rtscts`，`status` 改为 `okay` |
| 同上 | `ir_recv`（红外接收，占用 `GPIO1_IO21`）`status` 改为 `disabled` |

**保留项**：uboot 仍配置 UART1 引脚；内核 `&uart1` 仍为 `okay`，`/dev/ttymxc0` 可作为普通串口使用。

**板端环境变量**（旧环境优先级高于编译默认值）：

```
setenv console ttymxc1
setenv lcdtype 1024x600
setenv fdt_file topeet_emmc_1024x600.dtb
saveenv
```

**接线**：USB-TTL 的 RX 接核心板 UART2_TXD（Pin48），TX 接 UART2_RXD（Pin49），共地，电平选 3.3V，串口参数 115200/8N1。

> 注意：底板上的红外接收头如果物理连接在 UART2_RXD 网络上，会干扰接收，需要按原理图确认；软件侧已禁用 `ir_recv`。

### 3.3 DDR 压力测试（第 3 项）

**原因**：uboot 的 `mtest` 默认只测试 128MB，没有覆盖 512MB 全部 DDR。

**修改内容**：

| 文件 | 修改 |
| --- | --- |
| `include/configs/mx6ul_topeet.h` | `CONFIG_SYS_MEMTEST_END` 由 `START + 0x8000000` 改为 `START + 0x1F000000`（496MB，留 16MB 给 uboot 重定位区） |

**测试方法**（uboot 命令行）：

```
mtest 0x80000000 0x9F000000 0 0     # 无限循环，Ctrl+C 停止
```

**验收标准**：连续多轮输出 `Tested N iteration(s) with 0 errors.`。

**可选方案**：NXP 官方 DDR Stress Tester（Windows + USB OTG + 板级 .inc），用于产线校准；本项目当前不采用。

### 3.4 调试 RS485 接口（第 4 项）

**硬件说明**：迅为板 RS485 走 UART3，经 SP3485 转换，方向由 `RS485_1_TX` 硬件自动控制，不需要 GPIO。
内核设备树中 `&uart3` 已 `okay`，对应设备节点 `/dev/ttymxc2`。板上 UART3 通过跳线 J46 选择 RS232/RS485。

**修改内容**：

| 文件 | 修改 |
| --- | --- |
| `app/rs485_test.c` | 新增测试程序（termios，115200/8N1，模式 0 发送、模式 1 接收） |

**编译**：

```bash
arm-linux-gnueabihf-gcc app/rs485_test.c -o app/rs485_test
```

**测试步骤**：

1. J46 跳线选择 RS485；
2. USB 转 RS485 适配器 A/B 接板上 A/B（收不到可对调）；
3. 板端执行 `./rs485_test /dev/ttymxc2 0`；
4. PC 串口助手 115200/8N1 接收；反向再执行 `./rs485_test /dev/ttymxc2 1` 验证接收。

**验收标准**：双向收发无乱码，与 PC 端内容一致。

## 四、编译与烧写说明

```bash
# 每个新终端先加载环境
source /home/acheng/imx6ull/app/mkenv.sh

# 编译 uboot（保留现有 .config，不要执行 distclean/defconfig）
cd /home/acheng/imx6ull/sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga
make -j8
# 产物：u-boot.imx

# 只编译 1024x600 设备树（不要执行内核 create.sh，它会重置 .config）
cd /home/acheng/imx6ull/sdk/linux-imx-rel_imx_4.1.15_2.1.0_ga
make topeet_emmc_1024x600.dtb
# 产物：arch/arm/boot/dts/topeet_emmc_1024x600.dtb
```

烧写方式：使用现有 USB OTG + mfgtool/uuu 流程烧写 `u-boot.imx`；dtb 放到 eMMC 的 FAT 启动分区并保持文件名为 `topeet_emmc_1024x600.dtb`。

本次（最近一次）编译结果：uboot 与 dtb 均编译通过；uboot 镜像中已确认包含 `console=ttymxc1`、`lcdtype=1024x600`。

## 五、依赖与待办

- 板端验证第 1~4 项（需 1024x600 屏幕、USB-TTL、USB 转 RS485 适配器）。
- 自定义 logo 图片（1024x600 BMP，8 位调色板），需要时提供。
- 确认底板红外接收头与 UART2_RXD 的物理连接关系（若已连接，接收可能受干扰）。
- 后续第 5~17 项按需求顺序推进。

## 六、变更记录

| 日期 | 内容 |
| --- | --- |
| 2026-11-12 | 完成第 1~4 项代码修改：uboot logo/lcdtype、UART2 调试串口、DDR mtest 范围、RS485 测试程序；uboot 与 dtb 编译通过 |
