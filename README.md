## Draws ascii art or bitmap image from user-input Chinese text

Build with cmake and Visual Studio 2022, requires freetype2 installed

You can install it via vcpkg on windows: `vcpkg install freetype`

then specify `-DCMAKE_TOOLCHAIN_FILE` to vcpkg's cmake script path
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

executables:

- banner.exe: draws text as ascii art, uses Microsoft YaHei font family (msyh.ttc).

- bmpbanner.exe: draws text as bmp image, uses Microsoft YaHei font family.

- hzk16ascii.exe: draws text as ascii art, uses HZK16 font.

- hzk16bmp.exe: draws text as bmp image, uses HZK16 font.

---
## 将用户输入的文本绘制为字符画或bmp图片

使用cmake + Visual Studio 2022构建，需要安装freetype2

在Windows上可以通过vcpkg安装：`vcpkg install freetype`

然后指定`-DCMAKE_TOOLCHAIN_FILE`为vcpkg的cmake脚本路径
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

可执行文件：
- banner.exe: 绘制字符画，使用微软雅黑字体

- bmpbanner.exe: 绘制bmp图片，使用微软雅黑字体

- hzk16ascii.exe: 绘制字符画，使用HZK16字体

- hzk16bmp.exe: 绘制bmp图片，使用HZK16字体
