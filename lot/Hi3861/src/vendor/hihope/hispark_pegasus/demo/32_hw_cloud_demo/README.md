**注意：**

31_hw_cloud_demo 和 32_hw_cloud_demo 两个示例程序，实现的功能是一模一样的，稍微有点区别的地方在于：

> 31_hw_cloud_demo 所使用的 libiot_link.a 库，是预编译的；
>
> 32_hw_cloud_demo 则是直接使用 源代码编译出 libiot_link.a。libiot_link.a 库 的源代码，见：[这里](https://gitee.com/openharmony-sig/knowledge_demo_smart_home/tree/master/dev/third_party/iot_link)



在编译 libiot_link.a 的源代码时，因为 Hi3861 的SDK已包含一个预编译的 libmqtt.a 文件，会导致编译异常，可以修改：./third_party/iot_link/BUILD.gn 

```
group("iot_link") {
    deps = [
        "inc:inc",
        "link_log:link_log",
        "link_misc:link_misc",
        "network/dtls:dtls",
        "network/mqtt:mqtt_paho",   # 由 mqtt 修改为 mqtt_paho
        "oc_mqtt:oc_mqtt",
        "queue:queue"
    ]
}
```

和修改 ./third_party/iot_link/network/mqtt/BUILD.gn

```
static_library("mqtt_paho") {     # 由 mqtt 修改为 mqtt_paho
        cflags = mqtt_cflags
        defines = mqtt_paho_defs
        sources = mqtt_paho_src
        include_dirs = mqtt_paho_inc
}
```

重新编译即可。