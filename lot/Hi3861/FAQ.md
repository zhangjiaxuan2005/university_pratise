# 常见问题列表

## Q1：IDE 编译异常

用 IDE（VS Code + devicetool-windows-tool 插件）一键编译代码时，提示如下编译异常：

```
Processing hi3861 (platform: cfbb; board: hi3861; framework: hb)
----------------------------------------------------------------------
PLATFORM: CFBB 4.0.400 > Hi3861
HARDWARE: HI3861 900MHz,
DEBUG: Current (jlink) On-board (jlink)
Building in debug mode
Verbose mode can be enabled via `-v, --verbose` option
builder(["src\out\hispark_pegasus\wifiiot_hispark_pegasus\target.elf"], [])
scons: *** [src\out\hispark_pegasus\wifiiot_hispark_pegasus\target.elf] 系统找不到指定的文件。

please check the compilation log: C:\Users\liang\.deveco-device-tool\logs\build\build.log
===================== [FAILED] Took 1.36 seconds =====================
```

**A1**：很有可能这是编译工具没有配置正确导致的，可以按以下**两种**方式解决（推荐使用方式一）：

**方式一**：

确认已经下载了 [DevTools_Hi3861V100_v1.0.zip](https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/DevTools_Hi3861V100_v1.0.zip) ，放置到 //tools/ 目录下并解压出来：

<img src="F:\Hi3861\pic\安装编译工具链.PNG" alt="安装编译工具链" style="zoom:67%;" />

然后按下图的步骤打开工程配置信息，并配置“compiler_bin_path”：

<img src="F:\Hi3861\pic\配置编译工具链路径.png" alt="配置编译工具链路径" style="zoom: 50%;" />!在弹出菜单中选择项目的 tools 目录即可：

<img src="F:\Hi3861\pic\配置编译工具链路径2.png" alt="配置编译工具链路径2" style="zoom: 67%;" />

确认OK后，重新编译，应该就可以了。

**方式二**：

打开IDE的工程配置，查看“工具链”标签页，可以看到开发工具包是一个“未安装”的状态，这时候可以点击“下载未安装的工具”或者“下载” 按键，即会自动下载和安装开发工具包到一个默认的路径。

<img src="F:\Hi3861\pic\配置编译工具链路径3.png" alt="配置编译工具链路径3" style="zoom: 67%;" />

**注意**，自动下载有可能会失败，这时候可以去查看工程目录下的 .deveco\deveco.cfg 文件【 .deveco 和 .vscode 两个文件夹是导入工程代码时自动生成的，可能需要windows系统设置显示隐藏文件后才能看到】，该文件中有如下一些字段：

```
"tool_selected": {
    "toolset": {
        "use": "defined",
        "custom": {
            "directory": true,
            "path": ""
        },
        "defined": {
            "version": "1.0.0",
            "size": "428 MB",
            "install_info": {
                "url": "https://hispark.obs.cn-east-3.myhuaweicloud.com/DevTools_Hi3861V100_v1.0.zip",
                "sha256": "",
                "fetch": "direct",
                "install": "unpack"
            },
            "check_installed": [
                "env_set.py"
            ],
            "key": "DevTools_Hi3861V100@1.0.0",
            "display": {
                "zh": "1.0.0",
                "en": "1.0.0"
            },
            "features": {
                "platform": "windows",
                "protocols": []
            },
            "category": "toolset",
            "store_path": "D:\\DevEco\\resources\\toolset\\DevTools_Hi3861V100\\1.0.0",
            "relative_path": "toolset\\DevTools_Hi3861V100\\1.0.0",
            "renderKey": "#toolset#DevTools_Hi3861V100#DevTools_Hi3861V100@1.0.0",
            "name": "DevTools_Hi3861V100"
        },
        "key_list": [
            "DevTools_Hi3861V100"
        ]
    }
}
```

其中的【"url": "https://hispark.obs.cn-east-3.myhuaweicloud.com/DevTools_Hi3861V100_v1.0.zip"】字段，可能会因为该压缩包存放的路径有了变化，而导致自动下载失败，这时候可以将该url修改为正确的下载路径如：

【"url": "https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/DevTools_Hi3861V100_v1.0.zip"】保存后，重新操作自动下载。

另外，上面的 【"store_path": "D:\\DevEco\\resources\\toolset\\DevTools_Hi3861V100\\1.0.0"】字段，是上述工具压缩包下载下来后，自动解压的工具包所保存的路径，我们也可以手动修改该路径去改变工具包保存路径。



如果前面已经手动下载过 [DevTools_Hi3861V100_v1.0.zip](https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/DevTools_Hi3861V100_v1.0.zip) ，放置到 //tools/ 目录下并解压出来了（如方式一的图所示），则在方式二的图中，也可以选择“导入按钮”，选中方式一中图片所示的 DevTools_Hi3861V100_v1.0.zip 压缩包即可。