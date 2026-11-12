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
| 2 | 修改调试串口为 UART2 | uboot + 内核设备树 | 首次改动串口失联已回退为 UART1，待接线确认后重新启用 |
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

## 三、前四项需求的实现说明

### 3.1 uboot 添加 logo 显示（第 1 项）

**原因**：uboot 视频功能（`CONFIG_VIDEO`、`CONFIG_VIDEO_LOGO`、`CONFIG_SPLASH_SCREEN`、`CONFIG_CMD_BMP`）本身已经打开，
但默认环境变量 `lcdtype=hdmi`，而板级 `displays[]` 里没有 hdmi 面板，`board_video_skip()` 返回错误后视频初始化被跳过，所以不显示 logo。

**修改内容**：

| 文件 | 修改 |
| --- | --- |
| `include/configs/mx6ul_topeet.h` | `CONFIG_DISPLAY_LCD_TYPE` 由 `"hdmi"` 改为 `"1024x600"` |
| `include/configs/mx6ul_topeet.h` | NAND 默认环境 `lcdtype=hdmi` 改为 `lcdtype=1024x600`；eMMC 默认环境新增 `lcdtype=1024x600` |

**效果**：uboot 启动时初始化 1024x600 面板，并把内置 logo（`tools/logos/freescale.bmp`）按 `splashpos=m,m` 全屏居中显示；
同时 uboot 会根据 `lcdtype` 自动设置 `fdt_file=topeet_emmc_1024x600.dtb`。

**自定义全屏 logo 制作与烧录步骤（使用 docs/aabb.bmp）**：

> 分工标记：【我已完成✔】= AI 已执行并验证，不用再做；【你执行→】= 需要你在 Ubuntu / 开发板上操作。
> **本次要烧录的文件：u-boot.imx（971KB，必须）+ topeet_emmc_1024x600.dtb（本次已回退为与出厂等效，上次若替换过请用新编译的再替换一次）；zImage、文件系统无改动，不用重新烧。**
> UART2 改动已全部回退（原因见 3.2 回退说明），当前串口仍走 UART1。

1. 【我已完成✔】**转换图片格式**。uboot 的 `tools/bmp_logo` 工具只认 8 位调色板 BMP（256 色索引格式），
   原始 `docs/aabb.bmp` 是 24 位色，需要转换。转换脚本已写好（`scripts/make_logo.py`，用 Python PIL，
   缩放到 1024x600、量化为 200 色，低于工具上限 240 色）：

   ```bash
   cd /home/acheng/imx6ull
   python3 scripts/make_logo.py docs/aabb.bmp /tmp/logo_8bit.bmp
   # 输出: PC bitmap, 1024 x 600 x 8, image size 614400
   ```

2. 【我已完成✔】**替换 uboot 内置 logo**。原 NXP logo（480x272）已备份：

   ```bash
   cp sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga/tools/logos/freescale.bmp \
      sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga/tools/logos/freescale.bmp.orig
   cp /tmp/logo_8bit.bmp sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga/tools/logos/freescale.bmp
   ```

3. 【我已完成✔】**环境变量偏移必须同步挪动（关键）**。1024x600 的 8 位 logo 会让 u-boot 从 505KB 增大到 966KB，
   而原环境变量存放在 eMMC 偏移 768KB 处（`CONFIG_ENV_OFFSET (12 * SZ_64K)`），会被新的 uboot 镜像覆盖，
   之后一旦 `saveenv` 还会写坏 uboot 自身。已挪到 8MB 处：

   ```c
   #define CONFIG_ENV_OFFSET		(128 * SZ_64K)   /* 8MB */
   ```

   为什么不只用 2MB？因为第一次改动（UART2 版）已经把 `console=ttymxc1` 的 ENV 写到了 2MB 处，
   如果只改回 UART1 但 ENV 还在 2MB，新 uboot 启动时会读到旧 ENV 里的 `console=ttymxc1`，
   覆盖编译默认值 `console=ttymxc0`，导致 CH340G（UART1）口无输出。改到 8MB 彻底避开残留的旧 ENV 区域。

   新镜像结束于约 966KB，ENV 在 8MB 处，间隔约 7MB，安全。

4. 【我已完成✔】**重新编译 uboot**：

   ```bash
   source /home/acheng/imx6ull/app/mkenv.sh
   cd /home/acheng/imx6ull/sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga
   make -j8
   # 产物 u-boot.imx ≈ 971KB
   # 验证: grep LOGO_WIDTH include/bmp_logo.h  →  BMP_LOGO_WIDTH 1024
   ```

5. 【你执行→】**烧录到开发板**。需要替换迅为烧录工具里的 **2 个文件**：

   | # | 文件 | 来源路径（Ubuntu 里） | 原因 |
   | --- | --- | --- | --- |
   | 1 | u-boot.imx（994304 字节，14:06 编译） | `sdk/uboot-imx-rel_imx_4.1.15_2.1.0_ga/u-boot.imx` | 新 logo + ENV 偏移 8MB（避开旧 ENV） + 串口 UART1 恢复 |
   | 2 | topeet_emmc_1024x600.dtb | `sdk/linux-imx-rel_imx_4.1.15_2.1.0_ga/arch/arm/boot/dts/topeet_emmc_1024x600.dtb` | 本次已回退为与出厂等效；上次烧录若替换过 dtb，请用本次新编译的再替换一次 |

   方式 A（你正在用的：迅为 mfgtool，OTG 烧录）：
   把上面 2 个文件从 Ubuntu 拷到 Windows（共享文件夹/SCP 均可），替换烧录工具目录里的同名文件，
   **保持工具里的原文件名不变**（若工具里的名字不同，以工具里的名字为准重命名我们的文件）；
   然后 USB OTG 连接、拨码/按键进烧录模式、按迅为手册烧写。

   方式 B（板上自烧写，板子还能进老 uboot 时可用）：把新 `u-boot.imx` 拷到 eMMC 的 FAT 启动分区
   （改名为 `u-boot-new.imx`），在 uboot 命令行执行（NXP 标准 uboot 起始扇区为 2，即 0x400 偏移）：

     ```
     mmc dev 1
     fatload mmc 1:1 0x80800000 u-boot-new.imx
     mmc write 0x80800000 2 0x800      # 写入 2048 个扇区(1MB)，覆盖新镜像 971KB
     ```

6. 【你执行→】**首次开机**。烧录后上电：屏幕应显示你自己的 logo（全屏）。ENV 已挪到 8MB，
   eMMC 上 2MB 处残留的旧 ENV（console=ttymxc1）不会被读到，uboot 直接用编译默认值（`console=ttymxc0`、`lcdtype=1024x600`），
   CH340G 串口应该恢复输出。进入 uboot 后执行一次固化：

   ```
   setenv console ttymxc0
   setenv lcdtype 1024x600
   setenv fdt_file topeet_emmc_1024x600.dtb
   saveenv
   ```

   **烧录前确认**（方式 B 或不确定分区布局时）：在 uboot 里执行 `mmc part`，确认分区 1 的起始扇区
   不超过 16384（即 8MB），否则 ENV 会写进分区，需要告诉我再调整偏移。

**以后换 logo 只需重复 1、2、4、5 步**（环境变量偏移不用再改）。

### 3.2 修改调试串口为 UART2（第 2 项）

> **2026-11-12 回退说明**：首次烧录验证发现串口完全无输出。原因：uboot console 已改为 ttymxc1（UART2），
> 但板上 dtb 仍为出厂版（uart2 = disabled），内核中 ttymxc1 设备不存在；且串口线接在 UART1 上。
> uboot 阶段的新 logo 显示正常（屏幕先显示新 logo 数秒），随后被内核/出厂 rootfs 的启动画面接管，属于正常接力（内核阶段 logo 属第 8 项需求）。
> 已将 UART2 相关改动**全部回退**：uboot `UART1_BASE`/`console=ttymxc0`、板级 `uart2_pads`、
> dts `stdout-path`/`ir_recv`/`pinctrl_uart2`/`&uart2 status`。回退后 dts 与板级 C 文件与出厂备份 diff 为空。
> 重新启用 UART2 时必须三者配套一次切换：uboot console + 内核 dts + 核心板 48/49 脚接线。

**原因**：uboot 的调试串口基地址、引脚、默认 console 都指向 UART1；内核的 `stdout-path` 也指向 uart1。
另外 UART2 的 RX 引脚（`GPIO1_IO21`）与板上的红外接收节点 `ir_recv` 冲突，必须二选一。

**修改内容**（以下改动已于 2026-11-12 全部回退，仅留档供重新启用时参考）：

| 文件 | 修改（已回退） |
| --- | --- |
| `include/configs/mx6ul_topeet.h` | `CONFIG_MXC_UART_BASE` 改为 `UART2_BASE` |
| `include/configs/mx6ul_topeet.h` | 默认环境 `console=ttymxc0` 改为 `console=ttymxc1` |
| `board/freescale/mx6ul_topeet/mx6ul_topeet.c` | 新增 `uart2_pads[]`，`setup_iomux_uart()` 同时配置 UART1/UART2 引脚 |
| `arch/arm/boot/dts/imx6ull-14x14-evk.dts` | `stdout-path` 改为 `&uart2` |
| 同上 | `pinctrl_uart2` 删除借用 UART3 的两个引脚（RTS/CTS） |
| 同上 | `&uart2` 删除 `fsl,uart-has-rtscts`，`status` 改为 `okay` |
| 同上 | `ir_recv`（红外接收，占用 `GPIO1_IO21`）`status` 改为 `disabled` |

**保留项**：uboot 仍配置 UART1 引脚；内核 `&uart1` 仍为 `okay`，`/dev/ttymxc0` 可作为普通串口使用。

**板端环境变量**（串口恢复 UART1 后使用）：

```
setenv console ttymxc0
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

本次（最近一次）编译结果：uboot 与 dtb 均编译通过；uboot 镜像中已确认包含 `console=ttymxc0`、`lcdtype=1024x600`（UART2 已回退）。

## 五、依赖与待办

- 板端验证第 1~4 项（需 1024x600 屏幕、USB-TTL、USB 转 RS485 适配器）。
- 自定义 logo 图片（1024x600 BMP，8 位调色板），需要时提供。
- 确认底板红外接收头与 UART2_RXD 的物理连接关系（若已连接，接收可能受干扰）。
- 后续第 5~17 项按需求顺序推进。

## 六、变更记录

| 日期 | 内容 |
| --- | --- |
| 2026-11-12 | 完成第 1~4 项代码修改：uboot logo/lcdtype、UART2 调试串口、DDR mtest 范围、RS485 测试程序；uboot 与 dtb 编译通过 |
| 2026-11-12 | 内置 logo 替换为自定义图 docs/aabb.bmp（1024x600 全屏，8 位 BMP）；`CONFIG_ENV_OFFSET` 768KB→2MB（防止被 966KB 的 uboot 镜像覆盖）；新增 `scripts/make_logo.py` 转换脚本；u-boot.imx ≈ 971KB |
| 2026-11-12 | 烧录步骤标注分工（【我已完成✔】/【你执行→】）；明确本次需烧录 2 个文件：u-boot.imx + topeet_emmc_1024x600.dtb，zImage/文件系统不用重新烧 |
| 2026-11-12 | UART2 改动全部回退为 UART1（串口失联根因：新 uboot console=ttymxc1 与出厂 dtb uart2=disabled 不配套）；保留 logo/lcdtype/env 偏移/mtest；u-boot.imx 重新编译（994304 字节，ttymxc1 已清零），dtb 与出厂等效 |
| 2026-11-12 | **ENV 偏移从 2MB(32*64K)→8MB(128*64K)**，原因：旧 uboot(UART2 版) 的 saveenv 已在 2MB 处写入 console=ttymxc1，新 uboot 读到会覆盖编译默认 console=ttymxc0，导致 CH340G(UART1) 口无输出；改到 8MB 彻底避开；u-boot.imx 重新编译（14:06，console=ttymxc0、ttymxc1=0次、logo 1024x600） |
