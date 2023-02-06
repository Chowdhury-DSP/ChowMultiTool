# ChowMultiTool

![CI](https://github.com/Chowdhury-DSP/ChowMultiTool/workflows/CI/badge.svg)
[![License](https://img.shields.io/badge/License-GPLv3-green.svg)](https://opensource.org/licenses/GPL-3.0)

ChowMultiTool is a swiss-army-knife sort of plugin, containing
a handful of little effects and other things that I've found useful.

**This is currently pre-alpha software, and may be unstable!
Please don't use it for anything important just yet.**

If you would like to stay updated about future releases, please
use the repository "Watch" button.

If you would lik to contribute to the development of the plugin,
Please get in touch via GitHub Issues, or some other method.

## Tools:

Unfinished:
- EQ
- Signal Generator
- Waveshaper
- Band-Splitter
- Pultec-style EQ
- Brickwall filters
- SVF

## Building

To build from scratch, you must have CMake installed.

```bash
# Clone the repository
$ git clone https://github.com/Chowdhury-DSP/ChowMultiTool.git
$ cd ChowMultiTool

# build with CMake
$ cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```

## License

ChowMultiTool is open source, and is licensed under the GPLv3 license.
Enjoy!
