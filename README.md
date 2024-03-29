[![CMake](https://github.com/ZephirFXEC/Lumen/actions/workflows/cmake.yml/badge.svg)](https://github.com/ZephirFXEC/Lumen/actions/workflows/cmake.yml)
![Untitled-1](https://user-images.githubusercontent.com/66848869/202930412-3f399dad-4305-4be7-a370-21385a8eb718.png)
> Development has been moved in the DEV branch (last working version in Main) 😔


## Library Used
- Vulkan
- OneTBB
- GLM 
- GLFW
- ImGUI

## Compiler Used
- MSVC 19.X
- LLVM 15.0.2 
>(Using Clang-cl)

## How to Build 
 - Have VCPKG installed
 - install dependencies / Libraries using vcpkg 
 > Make sure to install imgui[docking-experimental,vulkan-binding, glfw-binding]
 - Set -DCMAKE_TOOLCHAIN_FILE to ``~/vcpkg/scripts/buildsystems/vcpkg.cmake``
 - Build it in your favorite IDE & Enjoy 😌

 

## About me

Enzo Crema – [@ZephirFX](https://twitter.com/zephirfxx) – ZephirFX.Pro@gmail.com


## Contributing

1. Fork it (<https://github.com/ZephirFXEC/Lumen/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request
