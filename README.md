# LVGL 嵌入式登录界面项目
基于 LVGL 8.3.11 + SquareLine Studio 1.4.2 开发的嵌入式UI界面，适配ESP32-S3，可直接编译运行。

## ✨ 项目功能
- 账号、密码输入框，支持输入内容
- 登录按钮触发校验，弹窗提示登录结果
- 界面样式、布局由SquareLine可视化导出C代码
- 可作为嵌入式终端登录模块复用

## 🛠 开发环境
- LVGL版本：v8.3.11
- SquareLine Studio：v1.4.2
- 主控芯片：ESP32-S3
- 编译框架：ESP-IDF

## 📁 文件说明
- `Login/`：SquareLine Studio导出的UI源码文件夹，包含页面、控件、样式C文件
