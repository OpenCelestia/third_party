# Third-Party Notices — Celestia `third_party`

本文件登记 Celestia 项目内置的第三方开源组件及其许可协议。**这些组件不由 Anyka
以 Apache-2.0 重新许可，均保留各自上游的原始协议。** 分发 Celestia 时须一并满足下列
各协议的义务（保留版权与许可声明、随附协议全文、按需回源等）。

> 生成日期：2026-07-07　|　对应仓库：GitHub `OpenCelestia/third_party`（https://github.com/OpenCelestia/third_party）
> 各组件的许可全文见其目录下的 `LICENSE` / `COPYING` / `LICENCE.txt` / `README`。

## 组件清单

| 组件 | 版本 | 许可协议 | Copyleft | 许可文件 |
|------|------|----------|:--------:|----------|
| awtk | 1.7.1 | LGPL（ZLG，另有商业授权） | 弱 | `awtk/awtk-1.7.1/LICENSE` |
| cJSON | 1.7.15 | MIT | 否 | `cjson/cjson-1.7.15/LICENSE` ⬅ 已补齐 |
| curl | 8.0.1 / 8.3.0 | curl（MIT/X 派生） | 否 | `curl/*/COPYING` |
| FFmpeg | 4.2.9 / 7.0 | LGPL-2.1-or-later | **弱** | `ffmpeg/*/COPYING.LGPLv2.1` |
| FreeType | 2.12.0 | FTL 或 GPLv2（双许可） | 弱 | `freetype/freetype-2.12.0/LICENSE.TXT` |
| libiconv | 1.15 | LGPL-2.1（库）/ GPL（工具） | **弱** | `iconv/iconv-1.15/COPYING.LIB` |
| iniparser | （内置） | MIT | 否 | `iniparser/LICENSE` ⬅ 已补齐 |
| JerryScript | 2.4.0 | Apache-2.0 | 否 | `jerryscript/jerryscript-2.4.0/LICENSE` |
| libjpeg (IJG) | 9e | IJG License | 否 | `jpeg/jpeg-9e/LICENSE` ⬅ 已补齐（详见 README 的 LEGAL ISSUES） |
| JPEGDEC | 1.4.2 | Apache-2.0 | 否 | `jpegdec/jpegdec-1.4.2/LICENSE` ⬅ 已补齐 |
| libpng | 1.6.43 | libpng License（zlib/PNG 类） | 否 | `libpng/libpng-1.6.43/LICENSE` |
| libwebsockets | 4.3 / 4.4 | MIT | 否 | `libwebsockets/*/LICENSE` |
| LVGL | 9.1.0 / 9.3.0 | MIT | 否 | `lvgl/*/LICENCE.txt` |
| Mbed TLS | 2.16.9 / 2.28.10 | Apache-2.0 | 否 | `mbedtls/*/LICENSE` |
| minIni | （内置） | Apache-2.0 | 否 | `minIni/LICENSE` ⬅ 已补齐 |
| OpenSSL | 1.0.2h / 3.1.5 | 1.x：OpenSSL+SSLeay 双许可；3.x：Apache-2.0 | 否 | `openssl/*/LICENSE*` |
| tslib | 1.22 | LGPL-2.1 | **弱** | `tslib/tslib-1.22/COPYING` |
| zlib | 1.2.3 / 1.3.1 | zlib License | 否 | `zlib/*/LICENSE`（1.2.3 已补齐） |
| zxing-cpp | v3.1.1（vendored under `zxing/zxing-cpp/`） | Apache-2.0 | 否 | `zxing/LICENSE` / `zxing/zxing-cpp/LICENSE` — WIFI QR via `zxing_scan_qrcode()`；Writers/zint 未启用 |

## 需特别关注的项

### ✅ xmlparser — 已移除
`xmlparser/` 许可未明（无版权/LICENSE/出处），经核实**全代码库无任何引用**（无 `#include`、
无 API 调用、无链接），属未使用组件，故**已直接删除**，并在三个产品 `config.mk` 中将
`CONFIG_THIRD_PARTY_XMLPARSER` 置为 `n`。无需替换为其他解析器。

### ✅ asr（Wanson）— 已移除
原 `asr/wanson`（Wanson 商业 ASR 静态库）为非开源商业二进制，**已从公开发布移除**。
构建前请禁用 `CONFIG_THIRD_PARTY_ASRIB`。详见 `asr/README.md`。

### ✅ libh264 — 已移除（与 FFmpeg 重复）
`libh264/` 内容实为 FFmpeg 的 `libavcodec`/`libavutil`（LGPL-2.1+），并非 Cisco
OpenH264（BSD）。三个产品均 `CONFIG_THIRD_PARTY_LIBH264 = n`（未启用），且 H.264
解码已由 FFmpeg（`--enable-decoder=h264`）提供，故 libh264 冗余，**已直接删除**。

## LGPL 组件合规要点（awtk / ffmpeg / iconv / tslib）

Celestia 自研代码为 Apache-2.0（非 GPL），与上述 LGPL 组件共存的前提：

1. **动态链接**：以 `.so` 形式动态链接 LGPL 库，避免静态链接的传染。
2. **可重链接（LGPL §6）**：若必须静态链接，需向最终用户提供足以重新链接的目标文件或源码。
3. **随附协议全文并提供源码获取途径**。
4. **FFmpeg 构建边界**：当前 `third_party/ffmpeg/Makefile` 的 configure 选项
   **未启用** `--enable-gpl` / `--enable-nonfree`，**未引入** libx264/libx265，
   故停留在 **LGPL** 边界。
   - ⚠️ 但当前使用 `--enable-static`（静态库）。为满足 LGPL §6 的重链接义务，
     建议改为 `--enable-shared` 动态链接，或随产品提供 FFmpeg 的可重链接目标文件。
   - configure 选项快照见本文件末尾附录，变更构建选项时须重新评估许可边界。
5. **AWTK**：ZLG AWTK 为 LGPL + 商业双授权，请确认商用场景下的授权方式。
6. **H.264 专利**：FFmpeg 的 H.264 编解码可能涉及 MPEG-LA 专利池，
   商用出货需单独评估专利许可（与开源许可相互独立）。

## 附录：FFmpeg configure 选项快照（截至 2026-07-07）

来源：`third_party/ffmpeg/Makefile`

```
--prefix=<build> --enable-cross-compile --cross-prefix=<toolchain> --arch=<arch>
--enable-static --disable-debug --disable-dxva2 --disable-avdevice
--disable-swresample --disable-postproc --disable-avfilter --disable-ffplay
--disable-swscale --disable-ffprobe --disable-decoder=vvc --disable-programs
--disable-doc --enable-pthreads --disable-everything
--enable-avformat --enable-avcodec --enable-avutil --enable-network
--enable-protocol=file,tcp,udp,http,rtsp,rtmp
--enable-demuxer=mov,avi,matroska,flv,mpegts,image2,mp3,wav
--enable-decoder=h264,hevc,mjpeg,mpeg4,mp3,aac,pcm_s16le,pcm_s16be
--enable-parser=h264,hevc,mjpeg,mpeg4,aac,mpegaudio
```

> 未见 `--enable-gpl` / `--enable-nonfree` / `--enable-libx264` → 结论：**LGPL**。
