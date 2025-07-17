# JPdy - A Jeopardy Game

This is a simple Jeopardy game built with C++ and the Qt framework.

## Qt Licensing and Dynamic Linking

This project uses the Qt framework under the LGPLv3 license. To comply with the terms of the LGPL, this application is dynamically linked against the Qt libraries.

### How Dynamic Linking is Achieved

In our `CMakeLists.txt`, we use `find_package(Qt6 REQUIRED COMPONENTS Widgets)` to locate the Qt libraries on the system. When you build the project, CMake automatically configures the build system to link against the shared Qt libraries (e.g., `.so` files on Linux, `.dylib` files on macOS, or `.dll` files on Windows).

This means that the Qt libraries are not bundled directly into the application executable. Instead, the operating system loads them at runtime. This allows users to replace the Qt libraries with their own versions.

### Verifying Dynamic Linking

You can verify that the application is dynamically linked by using system tools.

**On macOS:**
```bash
otool -L build-debug/jpdy
```
Look for paths pointing to Qt libraries (e.g., `@rpath/QtWidgets.framework/...`).

**On Linux:**
```bash
ldd build-debug/jpdy
```
Look for `libQt6Widgets.so.6` and other Qt libraries.

**On Windows:**
You can use a tool like "Dependencies" to inspect the `jpdy.exe` and see its imported DLLs, which will include `Qt6Widgets.dll` and others.

By following this approach, we ensure that the project complies with the Qt LGPLv3 license requirements.
