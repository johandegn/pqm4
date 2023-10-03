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
To prepare an implementation and move it to the correct folder in pqm4, stand in the this projects root and run either
- `faest-to-pqm4.py name` where name is fx faest_128f, or
- `faest-to-pqm4.py` without name to select all.

