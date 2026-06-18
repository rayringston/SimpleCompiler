# Simple Compiler

This is a simple compiler written in C++, to compile a pseudocode-type language into ARM64 instructions.
Tested using WSL running Ubuntu, using GNU's assembler and linker to create machine code. In the future, these too will be integrated into the simple compiler.

# How to Use
Basically you don't. This is meant to be run using Linux. Hypothetically it should also work using a different assembler and linker, but as of now it only uses the GNU toolchain for 64-bit ARM processors. Having this type of processor isn't actually necessary because of the toolchain, it'll kinda just cross compile to whatever architecture you have.

So, to run this you first need the Aarch64 Linux GNU tool chain:

```console
sudo apt-get update
sudo apt-get install binutils-aarch64-linux-gnu
```

Then go to the root directory of wherever you stored this repo, and run the build script:
```console
./build.sh test/test.txt
```

You can replace ```test/test.txt``` with the path to whatever file you want to compile.

---
Made by Ray Ringston (ray.ringston@gmail.com)

Last Updated: 6/17/2026
