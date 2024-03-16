# WabiSonorance

_A JUCE synthesizer plugin built with [Pamplejuce](https://github.com/sudara/pamplejuce/blob/main/README.md)_.

_UI designed using [Melatonin Inspector](https://github.com/sudara/melatonin_inspector)_.

## Introduction

See [PROPOSAL.md](./PROPOSAL.md) for a description of the project. See the [YouTube Demo](https://youtu.be/3KDtoU0Px40) for a quick overview of the synth's features.

## Building from Source

Follow the instructions found in the [Pamplejuce repo](https://github.com/sudara/pamplejuce). Generally, you need only basic libraries on your system, a C++ compiler supporting C++20 such as LLVM Clang, and CMake. Use latest versions available for each tool to ensure the compiler doesn't complain about the C++ Code.

1. `git clone git@github.com:jnickg/wabisonorance.git && cd wabisonorance`
2. `mkdir build && cd build`
3. `cmake ..`
4. `cmake --build .`

These commands will build `wabisonorance` and install the synthesizer to your system. By default, it's deployed to `~/.vst3/WabiSonorance Synthesizer.vst3`. A standalone version can also be built and used.

## Support

[Open an Issue](https://github.com/jnickg/wabisonorance/issues/new) with questions, feature requests, or bug reports.
