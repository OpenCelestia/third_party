# zxing (Celestia wrapper)

板端 WiFi QR 解码库，产物仍为 `libzxing.so` / `zxing_scan_qrcode()` ABI。

| 项 | 说明 |
|----|------|
| 解码引擎 | **zxing-cpp v3.1.1**（`zxing-cpp/`，Apache-2.0） |
| 入口 | `include/zxing_scan.h` → `zxing_scan_qrcode()` |
| 构建 | 仅编 `zxing-cpp/core`，`ZXING_WRITERS=OFF`，只启用 `ZXING_ENABLE_QRCODE` |
| 历史 | 旧 ~2008 C++ amalgamation 与 quirc fallback 已移除；曾用 v2.3.0，现升至 v3.1.1 |

许可全文：`LICENSE`（与 `zxing-cpp/LICENSE` 同为 Apache-2.0）。
