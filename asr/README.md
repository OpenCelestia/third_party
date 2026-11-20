# asr — 已从开源版本移除

本组件原为 **Wanson（云知声）商业语音识别引擎**，以预编译静态库形式提供：

- `asr/wanson/lib/libasr.a`（商业二进制）
- `asr/wanson/include/asr.h`

该二进制**非开源、受商业授权约束**，已从公开发布中移除（见开源前扫描报告）。

## 影响与后续

- `asr/Makefile` 仍引用 wanson，**构建前请禁用** `CONFIG_THIRD_PARTY_ASRIB`（或移除本目录）。
- 如需语音识别能力，请：
  1. 通过合法商业授权渠道单独获取 Wanson 库并置于本目录（不纳入公开仓库）；或
  2. 替换为具备明确开源许可的 ASR 方案。
