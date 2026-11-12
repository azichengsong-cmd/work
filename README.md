# i.MX6ULL 项目工作区同步仓库

北京迅为 i.MX6ULL 开发板项目（NXP 原厂 SDK，kernel 4.1.15，交叉编译器 arm-linux-gnueabihf-gcc 4.9.4）。

由于本工作区直连 github.com 受网络限制，本仓库通过 GitHub 连接器（API）逐批提交同步。

## 仓库结构

| 目录/文件 | 说明 |
| --- | --- |
| `docs/iMX6ULL项目需求与完成计划.md` | 17 项需求总表与前 4 项实现说明、验收标准 |
| `patches/` | 每处改动的统一 diff（基于改动前备份 .bak 生成），**这是改动回溯的核心** |
| `project-files/` | 被修改 SDK 源文件的镜像（当前状态快照） |
| `app/rs485_test.c` | RS485（UART3 → /dev/ttymxc2）收发测试程序 |
| `.gitignore` | 本地工作区排除规则（sdk/、tools/ 等大目录不入库） |

## 已完成改动（需求 1~4）

1. **uboot logo**：`lcdtype` 默认值 `hdmi` → `1024x600`（displays[] 中无 hdmi 面板导致视频初始化失败）
2. **调试串口改 UART2**：`CONFIG_MXC_UART_BASE` → UART2_BASE、`console=ttymxc1`、新增 uart2 引脚、内核 dts 使能 uart2 并禁用冲突的红外节点 ir_recv
3. **DDR mtest 范围**：128MB → 496MB（留 16MB 给 uboot 重定位区）
4. **RS485**：UART3 硬件自动方向控制，无需 GPIO，测试程序见 `app/rs485_test.c`

对应补丁：
- `patches/0001-uboot-lcdtype-1024x600-mtest-uart2.patch`
- `patches/0002-uboot-uart2-pads.patch`
- `patches/0003-kernel-dts-uart2-console.patch`

## 回溯方式

- 每次改动：先备份原文件为 `*.bak`，改完后生成 `patches/NNNN-描述.patch`
- 在 GitHub 上查看某次提交的 diff，或对 patch 应用/反应用（`patch -p1` / `patch -R -p1`）即可还原任意状态

## 镜像说明

- `project-files/.../mx6ul_topeet.h`：已上传（385 行，逐块校对）
- `project-files/.../mx6ul_topeet.c` 与 `.../imx6ull-14x14-evk.dts`：因文件较大（1325/1120 行）未在本仓库维护全量镜像，完整内容见本地工作区 git 提交；其全部改动通过 `patches/` 完整记录
