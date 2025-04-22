# Synopsis

Basic Flat Raycasting Algorithm in C. (using DDA algorithm).

> [!NOTE]
> With all my repositories, the `le-` prefix mean: Learning Exercise.


# Requirements

- [Raylib](https://www.raylib.com/)
- GCC

## Install dependencies on Mac OSX:

Install GCC:

```console
xcode-select --install
```

```console
gcc --version
Apple clang version 16.0.0 (clang-1600.0.26.4)
Target: arm64-apple-darwin24.1.0
Thread model: posix
```

Install Raylib:

```console
brew install raylib glfw
```

Version:

- Raylib 5.5

## Install dependencies on Linux (Debian / Ubuntu):

Install GCC _(as root user)_:

```console
apt-get install build-essential
```

Install Raylib _(as root user)_:

```console
apt-get install libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
    libwayland-dev libxkbcommon-dev
```

```console
gcc --version
gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
```

```console
apt-get install cmake
```

```console
mkdir -p /opt/src
cd /opt/src
git clone https://github.com/raysan5/raylib.git
```

```console
cd /opt/src/raylib
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/raylib-5.5 -DBUILD_SHARED_LIBS=ON ..
make
make install
```

In your `.bashrc` or `.zshrc` or `.exports` add:

```console
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/raylib-5.5/lib
```

# Build

```console
git clone https://github.com/odelbos/le-c-2-dda-raycasting
cd le-c-2-dda-raycasting
```

```console
./make.sh
```

# Run

```console
./raycast
```
