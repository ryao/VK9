# VK9 Build guide
## Linux (Ubuntu 18.04)
### Dependencies
- [wine 3.5](https://www.winehq.org/) or newer
- [Meson 0.43](http://mesonbuild.com/) build system or newer
- [MinGW64 7.3](http://mingw-w64.org/) compiler and headers (requires threading support) or newer
- [Boost 1.63.0](https://boost.org/) or newer
- [Eigen 3](http://eigen.tuxfamily.org/)
- [Windows VulkanSDK](https://vulkan.lunarg.com/)
- [SPIRV-Headers latest](https://github.com/KhronosGroup/SPIRV-Headers)
- [shaderc latest](https://github.com/google/shaderc)

On Ubuntu 18.04, MinGW and Meson can be installed with the following packages:
```
g++-mingw-w64
gcc-mingw-w64
binutils-mingw-w64
mingw-w64-tools
meson
```

Wine is outdated in the Ubuntu 18.04 repository and must be installed via PPA.
The Boost, Eigen and Vulkan development files are required in their MinGW version and must therefore be built manually.

By default, the Ubuntu MinGW installation does not have threading support
which leads to various compiler errors related to "std::mutex" and "std::thread".
This can be fixed by selecting the "posix" alternative of MinGW with the following commands:
```
update-alternatives --config x86_64-w64-mingw32-gcc
update-alternatives --config x86_64-w64-mingw32-g++
update-alternatives --config i686-w64-mingw32-gcc
update-alternatives --config i686-w64-mingw32-g++
```

#### Boost 32 bit
* Download latest boost libraries from boost.org

* Create file with name "user-config.jam" in the boost root directory with the following content:
```
using gcc : 7.3 : i686-w64-mingw32-g++
    :
    <rc>i686-w64-mingw32-windres
    <archiver>i686-w64-mingw32-ar
;
```
* Now run
```
./bootstrap.sh
./b2 toolset=gcc target-os=windows variant=release threading=multi threadapi=win32 link=static runtime-link=static --prefix=/usr/i686-w64-mingw32/include --user-config=user-config.jam -j 4 --with-system --with-thread --with-filesystem --with-program_options --with-log -sNO_BZIP2=1 -sNO_ZLIB=0 -a -d+2 --layout-tagged
```
* Copy compiled libraries from the "stage/lib" directory to "/usr/i686-w64-mingw32/lib"
* Rename "libboost\_thread\_win32.a" to "libboost\_thread.a"
* Rename all boost libraries ".a" suffix to "-mt-$BOOST\_VERSION.a" (replace $BOOST\_VERSION with your boost version, e.g. 1_63). If you have the "rename" utility installed, simply run:
```
rename 's/\.a/-mt-1_63.a/' libboost*
```
* Copy/link header files from the "./boost" directory to "/usr/i686-w64-mingw32/include"

#### Boost 64 bit
* Download latest boost libraries from boost.org
* If you reuse the 32bit boost directory delete the "bin.v2" and "stage" directories.

* Create file with name "user-config.jam" in the boost root directory with the following content:
```
using gcc : 7.3 : x86_64-w64-mingw32-g++
    :
    <rc>x86_64-w64-mingw32-windres
    <archiver>x86_64-w64-mingw32-ar
;
```
* Now run
```
./bootstrap.sh
./b2 toolset=gcc target-os=windows variant=release threading=multi threadapi=win32 address-model=64 link=static runtime-link=static --prefix=/usr/x86_64-w64-mingw32/include --user-config=user-config.jam -j 4 --with-system --with-thread --with-filesystem --with-program_options --with-log -sNO_BZIP2=1 -sNO_ZLIB=0 -a -d+2 --layout-tagged
```
* Copy compiled libraries from the "stage/lib" directory to "/usr/x68_64-w64-mingw32/lib"
* Rename "libboost\_thread\_win32.a" to "libboost\_thread.a"
* Rename all boost libraries ".a" suffix to "-mt-$BOOST\_VERSION.a" (replace $BOOST\_VERSION with your boost version, e.g. 1_63). If you have the "rename" utility installed, simply run:
```
rename 's/\.a/-mt-1_63.a/' libboost*
```
* Copy/link header files from the "./boost" directory to "/usr/x68_64-w64-mingw32/include"

#### Vulkan-Loader (vulkan-1)
* Download the LunarG Windows Vulkan SDK from https://vulkan.lunarg.com/ and install it in 64bit WINEPREFIX.
##### 32 bit
* Copy "VulkanSDK_dir/Include/vulkan/" to "/var/opt/i686-w64-mingw32/vulkan/include/vulkan"
* Copy "vulkan-1.dll" and "vulkan-1.lib" from "VulkanSDK_dir/Source/lib32/" to "/var/opt/i686-w64-mingw32/vulkan/lib"
##### 64 bit
* Copy "VulkanSDK_dir/Include/vulkan/" to "/var/opt/x68_64-w64-mingw32/vulkan/include/vulkan"
* Copy "vulkan-1.dll" and "vulkan-1.lib" from "VulkanSDK_dir/Source/lib/" to "/var/opt/x68_64-w64-mingw32/vulkan/l

#### SPIRV-Headers
* Clone https://github.com/KhronosGroup/SPIRV-Headers repository
* Copy the "include/spirv/1.2/" directory to "/var/opt/SPIRV-Headers/include/spirv"

#### Eigen
* Download Eigen from http://eigen.tuxfamily.org/index.php?title=Main_Page
* Estract "Eigen" directory from the tarball to "/var/opt/Eigen/include/Eigen"

#### glslc
* Clone https://github.com/google/shaderc.git
* To fetch all required third party dependencies run in the cloned directory:
```
cd third_party
git clone https://github.com/google/glslang.git
git clone https://github.com/KhronosGroup/SPIRV-Headers.git spirv-headers
git clone https://github.com/KhronosGroup/SPIRV-Tools.git spirv-tools
cd ../
```
* Now build "glslc" by running the following commands:
```
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DSHADERC_SKIP_INSTALL=true -DSHADERC_SKIP_TESTS=true ../
ninja
```
* Finally, copy "build/glslc/glslc" to "/usr/local/bin"

### VK9
Now simply run
```
./package-release.sh v-0.x ./
```
This create a tarball with the following structure:
```
vk9-0.x
- Shaders        - Compiled shaders, content goes to application directory
- x32
  - d3d9.dll     - 32bit VK9 dll, goes to system32 on 32bit WINEPREFIX or to sysWOW64 on 64bit WINEPREFIX
  - setup_vk9.sh - VK9 setup script for 32bit WINEPREFIX (sets d3d9 to native)
  - VK9.conf     - VK9 config file, goes to application directory
- x64
  - d3d9.dll     - 64bit VK9 dll, goes to system32 on 64bit WINEPREFIX
  - setup_vk9.sh - VK9 setup script for 64bit WINEPREFIX (sets d3d9 to native)
  - VK9.conf     - VK9 config file, goes to application directory
```

