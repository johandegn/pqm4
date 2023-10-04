# Embedded FAEST
This is the working project for making an embedded version of FAEST for the Cortex-M4 running using pqm4.

## How To
### Initially
First, build the faest project.
```
mkdir faest-ref/build
cd faest-ref/build
meson ..
ninja
ninja test
```
See `faest-ref/README.md` for details.

Then, build the pqm4 project.
```
make -j8 PLATFORM=stm32f4discovery
```

## Prepare and move to pqm4
To prepare an implementation and move it to the correct folder in pqm4, stand in the this projects root and run either:
- `./faest-to-pqm4.py name` where name is fx faest_128f, or
- `./faest-to-pqm4.py` without name to select all.

## Test FAEST on host machine
The file `faest-test.c` uses a FAEST variant to generate a keypair, sign a message and verify the message. This serves as a simple test for making changes to the code, without using the existing testing framework.
Use either of the following to compile and run it:
- `make run VARIANT=name` where name is fx faest_128f, or
- `make run` which defaults to faest_em_128s

