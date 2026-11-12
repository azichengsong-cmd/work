#!/usr/bin/env python3
"""
将任意图片转换为 uboot bmp_logo 工具可用的 8 位调色板 BMP（1024x600）。

用法:
    python3 scripts/make_logo.py <源图片> <输出BMP>

说明:
    - uboot 的 tools/bmp_logo 只支持 8 位调色板 BMP（256色索引格式）
    - 调色板可用上限 = 256 - DEFAULT_CMAP_SIZE(16) = 240，这里取 200 留余量
    - 输出尺寸固定 1024x600（与 LCD 分辨率一致，开机全屏显示）
"""
import sys
from PIL import Image

W, H = 1024, 600
MAX_COLORS = 200


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)
    src, dst = sys.argv[1], sys.argv[2]

    img = Image.open(src).convert('RGB')
    if img.size != (W, H):
        img = img.resize((W, H), Image.LANCZOS)
    img = img.quantize(colors=MAX_COLORS, method=Image.MEDIANCUT)
    img.save(dst, format='BMP')
    print('OK: %s  size=%s  mode=%s' % (dst, img.size, img.mode))


if __name__ == '__main__':
    main()
