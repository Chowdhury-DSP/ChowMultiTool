# ChowMultiTool

![CI](https://github.com/Chowdhury-DSP/ChowMultiTool/workflows/CI/badge.svg)
[![License](https://img.shields.io/badge/License-GPLv3-green.svg)](https://opensource.org/licenses/GPL-3.0)

ChowMultiTool is a swiss-army-knife sort of plugin, containing
a handful of little effects and other things that I've found useful.

**This software is currently in beta. We expect it to be mostly stable,
but there may be breaking changes introduced in between now and version
1.0.0.**

## Quick Links:

- Latest Release (coming soon)
- ChowMultiTool for iOS (coming soon)
- [Nightly Builds](https://chowdsp.com/nightly.html#multitool)
- [TestFlight Builds](https://testflight.apple.com/join/KuFujgKZ)

## Tools:

- EQ
- Signal Generator
- Waveshaper
- Band-Splitter
- Analog-style EQ
- Brickwall Filters
- State Variable Filters

### Video walkthrough:
[![Walkthrough video link](https://img.youtube.com/vi/9J8roV1Eoj0/0.jpg)](https://youtu.be/9J8roV1Eoj0)

## Building

To build from scratch, you must have CMake installed.

Currently, the plugin has only been tested building with Clang (across
Mac, Windows, and Linux). It is likely that the plugin would build fine
with other major compilers, but for the moment, we're planning to ship
Clang builds on all platforms.

```bash
# Clone the repository
$ git clone https://github.com/Chowdhury-DSP/ChowMultiTool.git
$ cd ChowMultiTool

# build with CMake
$ cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```

## Credits

- Extra Icons - [FontAwesome](https://fontawesome.com/) and [FontAudio](https://github.com/fefanto/fontaudio)
- Math Expression Parsing - [ExprTk](http://www.partow.net/programming/exprtk/)
- UI Inspection - [melatonin_inspector](https://github.com/sudara/melatonin_inspector)
- Limited-memory BFGS algorithm - [LBFGSpp](https://github.com/yixuan/LBFGSpp)
- Vector Support - [Eigen](https://gitlab.com/libeigen/eigen) 

## License

ChowMultiTool is open source, and is licensed under the GPLv3 license.
Enjoy!
