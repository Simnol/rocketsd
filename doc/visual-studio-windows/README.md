# Dev setup for Visual Studio 2019

## 1. Environment setup

- Install [Git](https://gitforwindows.org/)
- Install [Python3](https://www.python.org/downloads/) (making sure to install `pip` and to add Python to the `PATH`).
- Install Visual Studio 2019, including the 'CMake Tools' component.

Run the following in the terminal prompt of your choosing (I recommend Powershell
within [Windows Terminal](https://www.microsoft.com/en-ca/p/windows-terminal/9n0dx20hk701))

```powershell
pip install conan
conan --version # Should be 1.40.3 or above
conan profile new default --detect
conan profile update settings.build_type=Debug default
```

Using a text editor, open the file at `%USERPROFILE%/.conan/profiles/default`.
It should look like this:

```ini
[settings]
os=Windows
os_build=Windows
arch=x86_64
arch_build=x86_64
compiler=Visual Studio
compiler.version=16
build_type=Debug
[options]
[build_requires]
[env]
```

## 2. Clone code

Get the sources, including the embedded `anirniq-shared` submodule:

```powershell
git clone git@github.com/ngc7293/rocketsd.git --recursive
cd rocketsd
```

## 3. Install Conan packages

From within the previously cloned repo:

```powershell
# Configure Conan to use custom artifactory with prebuild Qt
conan config set general.revisions_enabled=1
conan remote add davidbourgault https://artifactory.davidbourgault.ca/artifactory/api/conan/conan

# Install 3rd parties
mkdir out/build/windows-default
cd out/build/windows-default
conan install ../../.. -r davidbourgault
```

## 4. Configure Visual Studio

On the first screen, select 'Continue without code'. Once the main window has
appeared, select `File > Open > CMake`.

![Open CMake](https://github.com/ngc7293/rocketsd/blob/develop/doc/visual-studio-windows/vs-open-cmake.png)

Once the project has been loaded, select `Project > CMake Settings`.

![CMake Settings](https://github.com/ngc7293/rocketsd/blob/develop/doc/visual-studio-windows/vs-cmake-settings.png)

A new `CMakePresets.json` file should appear in the solution explorer. It should
look like this:

```json
{
  "version": 2,
  "configurePresets": [
    {
      "name": "windows-default",
      "displayName": "Windows x64 Debug",
      "description": "Target Windows with the Visual Studio development environment.",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "architecture": {
        "value": "x64",
        "strategy": "external"
      },
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/out/install/${presetName}"
      },
      "vendor": { "microsoft.com/VisualStudioSettings/CMake/1.0": { "hostOS": [ "Windows" ] } }
    }
  ]
}
```

Note: `binaryDir` should match the folder created during step 3.

Next configure the project by selecting `Project > Configure rocketsd`. You can
now build the project by pressing `F7` or `Ctrl + Shift + B` or selecting
`Build > Build All`.

## 5. Launching with debugger

Once the build has completed, select the `rockestd.exe (bin\rocketsd.exe)` debug
target.

![Select Debug Target](https://github.com/ngc7293/rocketsd/blob/develop/doc/visual-studio-windows/vs-select-rocketsd.png)

Next, go to `Debug > Debug and Launch Settings for rocketsd`.

![Set Debug Settings](https://github.com/ngc7293/rocketsd/blob/develop/doc/visual-studio-windows/vs-debug-settings.png)

This should open a new JSON file. Add the `"arg"` element so it looks like this:

```json
{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "default",
      "project": "CMakeLists.txt",
      "projectTarget": "rocketsd.exe (bin\\rocketsd.exe)",
      "name": "rocketsd.exe (bin\\rocketsd.exe)",
      "args": ["-c", "../../../../config.json", "-p", "../../../../protocol.xml"] // Add this line
    }
  ]
}
```

The path `../../../../config.json` must point to a valid config.json. The path
provided here points to the one kept for debugging and demoing purposes at the
repo's root. You can also use a full path (`C:\Users\...` if you so chose.)

It is expected that `rocketsd` will now launch under Visual Studio's debugger.
