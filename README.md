# intel4004-emulator
Intel 4004 Emulator with Basic Instructions and Assembler. [Assembler repo](https://github.com/PinkFlowerDelivery/intel4004-asm)

## How to build 
```
git clone https://github.com/PinkFlowerDelivery/intel4004-emulator
cd intel4004-emulator
mkdir build 
cmake -S . -B ./build
cmake --build build

./build/i4004emu path/to/program.bin [-debug]
```

## Supported Instructions
```
nop, jcn, fim, fin, jin, jun, jms, inc, isz, add, sub, ld, xch, bbl, ldm, clb, clc, iac, cmc, cma, ral, rar, tcc, dac, tcs, stg, daa
```
