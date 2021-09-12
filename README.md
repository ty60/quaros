# quaros
Quaros is a toy operating system developed for x86 architecture.  
The functionalities are very primitive, but implements the most basic features of an OS.
- Context switching and scheduling
- Virtual memory
- Interrupt handling
- System calls

But does not have a lot of features that actual modern OSes have.
- Multi-processor support
- Locks
- 64 bit mode
- Network
- and much more...

## Usage
Build:
```bash
$ make
```
Run in qemu:
```bash
$ make run
```
It is currently only tested on qemu.
__Please don't run it on real hardware.__

## Demo
<p align="center">
  <img width="600" src="https://raw.githubusercontent.com/ty60/quaros/master/demo.svg">
</p>
