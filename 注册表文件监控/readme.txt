1. ProcessInject文件夹包含进程注入源码，需要创建服务进程，命令如下：
 sc create [服务名称] binpath= "[ProcessInject可执行文件的绝对路径]"
2. testFileDatabase文件夹下包含监控源码，可执行文件为testFileDatabase.dll
将testFileDatabase.dll文件拷贝到c:/windows/system32/目录下
3. 启动ProcessInject服务即可
运行环境：windows XP ，VS2010 sp1