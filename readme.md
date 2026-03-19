# Anago command-line software for Kazzo dumper

This project is a Linux port of the **anago** 
NES/Famicom flashing and dumping utility 
(from the [Unagi](https://osdn.net/projects/unagi/wiki/FrontPage) project) which is used with the [kazzo](https://osdn.net/projects/unagi/releases/46303) board. This specifically ports version 0.6.0 which was generally more reliable and had a larger script complement than version 0.6.2. That said, I'm sure some investigation could convert missing scripts, and potentially the 0.6.2 version may be more compatible to begin with, but what can you do. I doubt anyone besides myself will use this anyways.

Here is a brief summary of the differences between this codebase and the upstream original:

 1. Per comments in the upstream **porting.txt** document, all Windows references were removed and usually replaced with **unistd.h**. Sleeps were replaced with usleep accordingly.
 2. Static makefile was replaced with a cmake configuration, which simplifies dependency resolution.
 3. Squirrel library upgraded to version 3 per since this is what's already in the Debian repos
 4. Code specific to the unagi GUI removed and code consolidated to a single main folder structure.
 5. Rather than relying on the full kazzo source tree, I simply included the two required H files in this tree. This simplifies the build further.
 6. Script files are not read from the current directory, but rather from **$prefixdir**/share/anago, or from ~/.config/anago. This allows the anago tool to be run from anywhere and thus behaves more like a standard Unix application.

I did start changes by commiting the unmodified upstream code, so you can also browse the change history for more details.

# Build & Install

## Dependencies

This project only requires the following:

 * [Squirrel](http://squirrel-lang.org/)
 * [Ncurses](https://invisible-island.net/ncurses/)
 * [libusb](http://www.linux-usb.org/)
 * [cmake](https://cmake.org/)

And to regenerate the manual page:

 * [txt2man](https://github.com/mvertes/txt2man)

On a Debian/Ubuntu-based distro, these can be installed using the following command:
  
    apt install libsquirrel-dev libncurses-dev libusb-dev cmake

## Build

The build sequence is actually quite simple. For a basic build, start with a terminal in the anago source folder and execute the following:

    mkdir build
    cd build
    export CFLAGS="$CFLAGS -fcommon"
    cmake ..
    make
    cp ../scripts/*.nut .

To regenerate the manual page, if you have **txt2man** installed, use the following:

    make manual
    
## Install

After the above is run, simply execute the following, as root, in the build folder. This will install to **/usr/local** by default. Refer to cmake documentation for setting the prefix directory, if alternate destinations are needed.

    make install

## Packaging

The packaging process is slightly different than the above, as the prefix directory needs to be explicitly redirected so the tool is aware of the package destination. Execute the following from the **build** directory to create packages:

    cmake -D CMAKE_INSTALL_PREFIX=/usr ..
    make package

For packaging formats other than .deb, edit the **CMakeLists** and add the package format to the **CPACK_GENERATOR** line.
    
# Usage

Refer to include manual page or original readme files for more details ([anago_en.txt](anago_en.txt), ...).

<ins>**Important :**</ins> The 2 files **dumpcore.nut** and **flashcore.nut** must be into the same directory than the anago executable or into one of the directories :

  * ***${HOME}***/.config/anago/
  * ***CMAKE_INSTALL_PREFIX***/***SHARE_DIR***
  * ../scripts/

<ins>**General command-line syntax :**</ins>

    ./anago mode script_file target_file [...]

## Dump Mode

Used to create iNES ROM images from carts.

    ./anago d?? script_file dump_file [mapper] [submapper]

* **d??** or **D??** &nbsp; &nbsp; &nbsp; If given, the **??** portion determines the length of the ROM to dump in multiples of the mapper's configured width for the program ROM portion and character ROM portion respectively. For example, UNROM boards use 128KB PRG ROMs while UOROM boards use 256KB ROMs. A dumper script is provided for UNROM. To dump a UOROM board, use the UNROM script with the d2 option. Each **?** can be one of the values :
     * **2** is applicable for unrom.ad, mmc3.ad and mmc5.ad
     * **4** is applicable for mmc5.ad

* **d??** &nbsp; &nbsp; &nbsp; Dumping progress display with ncurses.

* **D??** &nbsp; &nbsp; &nbsp; Dumping progress display is same as unagi.exe.

* **script_file** &nbsp; &nbsp; &nbsp; Specifies **.ad** file for target cartridge.

* **dump_file** &nbsp; &nbsp; &nbsp; Output file, typically ending in .nes.

* **mapper** &nbsp; &nbsp; &nbsp; Facultative. Overrides the mapper number in the output ROM image. This is useful for mapper ASICs that are assigned multiple iNES mapper numbers such as MMC3/6.

* **submapper** &nbsp; &nbsp; &nbsp; Facultative. Overrides the submapper number in the output ROM image.

### Example 1.1 : Getting an image for UNROM

    ./anago d unrom.ad unrom.nes

anago get vram mirroring connection from target cartridge.

### Example 1.2 : Getting an image for UOROM

    ./anago d2 unrom.ad unrom.nes

UOROM data size is twice that of UNROM. This must be specicifed by hand.

### Example 2 : Getting an image for Metal Slader Glory / ELROM

    ./anago d22 mmc5.ad hal_4j.nes

In many cases, it is enough by 2M+2M to dump images for MMC5. MMC5 can manage 8M+8M. Please specify it individually according to capacity. 

### Example 3 : Getting an image for Ys III / TKSROM

    ./anago d mmc3.ad vfr_q8_12.nes 118

Enter digits on last commandline word to change mapper number for MMC3 variants. 

## Flash Programming Mode

Write to Flash ROM chips on specially prepaired flash carts.

    ./anago f?? script_file programming_file prg_chip chr_chip
    ./anago F?? script_file programming_file prg_chip chr_chip
    ./anago a?? script_file programming_file prg_chip chr_chip

* **a** &nbsp; &nbsp; &nbsp; Use DRIVER_DUMMY

* **f** &nbsp; &nbsp; &nbsp; Usual flashing mode. Use DRIVER_KAZZO.

* **F** &nbsp; &nbsp; &nbsp; Idem but validate written memory and abort if there is a discrepancey.

* **f??** or **F??** &nbsp; &nbsp; &nbsp; If given, the **??** portion controls the fill mode for the PRG and CHR Flash ROM chips respectively. If omitted, _Fill Mode_ is assumed. Use DRIVER_KAZZO. Each **?** can be one of the values :

   * **f** &nbsp; &nbsp; &nbsp; Fill mode. The entire ROM chip is overwritten, duplicating the PRG or CHR ROM contents as many times as nessecary to fill the chip. This is the slowest option but may be needed under some curcumstances. If unsure, use this mode. In general, if your mapper has an unknonw power-on state or the top address lines of the ROM may float at runtime, always use this mode.


   * **t** &nbsp; &nbsp; &nbsp; Only writes the ROM image once to the top-most portion of the chip. This mode is typically safe for CHR-ROM for any mapper, as the power-on state is immaterial with a proper initialization routine.

   * **b** &nbsp; &nbsp; &nbsp; Only writes the ROM image once to the bottom-most portion of the chip. Use with caution, most mapper's power-on states are incompatible with this mode.

   * **e** &nbsp; &nbsp; &nbsp; Write nothing for this memory device. This is equivalent to specifiying a dummy device.

* **script_file** &nbsp; &nbsp; &nbsp; Specifies **.af** file for target cartridge.

* **programming_file** &nbsp; &nbsp; &nbsp; Specifies the input iNES image file.

* **prg_chip** / **chr_chip** &nbsp; &nbsp; &nbsp; Specifies the Flash ROM chip present for each memory type in the target flash cart. Supported devices are listed in **flashdevice.nut**. **dummy** is a special device name that will transfer no data. Use this to avoid overwriting one of the memory chips.

### Example 1.1 : Tranfer 1M+1M image to mmc3 / TLROM.

    ./anago f mmc3.af tlrom_1M_1M.nes AM29F040B AM29F040B

In this case the board is configured with 4M flash ROMs. The MMC3 can only map 2M of CHR-ROM, so the high address line is assumed to be tied (either high or low). Anago would transfer the PRG portion four times to fill the PRG chip, and the CHR portion two times to fill the CHR chip.

### Example 1.2 : Tranfer 1M+1M image to mmc3 / TLROM.

    ./anago ftt mmc3.af tlrom_1M_1M.nes AM29F040B AM29F040B

In this example the PRG and CHR ROMs are only written once, this time to the top portion of each chip, in order to save time. The program code will need to be aware of this arrangement and only use the top-most bank numbers.

### Example 1.3 : Tranfer 1M+1M image to mmc3 / TLROM.

    ./anago fbt mmc3.af tlrom_1M_1M.nes AM29F040B AM29F040B

In this example the PRG ROM is written to the bottom 1M of the PRG ROM chip and the CHR ROM is written to the top 1M of the CHR ROM chip. The origional author remarks that this often works with Konami and Namcot titles. 

Incidentally, mmc3 is not compatible with Namcot 109. The 109 board has hard-wired vram mirroring, mmc3 does not. Furthermore Namcot 106 is a fictitious device with well known extra sound. Don't miss 109 and 106.

* Note that the second translator does not fully understand the meaning of the above paragraph.

### Example 2 : tranfer UNROM(1M) image to UOROM.

    ./anago ft uorom.af unrom.nes AM29F040B

If charcter memory is RAM, charcter device can be skip. Again the program will need to only use the upper bank numbers.

### Example 3 : Transfferring only charcter ROM image to mmc1/ SLROM

    ./anago ftt mmc1_slrom.af skrom.nes dummy AM29F040B
    ./anago fet mmc1_slrom.af skrom.nes AM29F040B AM29F040B

Both commands result in the same behavior. Choose the one that suits your taste.

## Notes

* If the concepts of Top and Bottom transfer are not clear, and their impact and requirements on your software are not obvious, please use the full transfer mode at all times.

* Anago does not have WRAM reading or writing mode at this time. Please use the Unagi utility for this purpose if needed.

# Errors and solutions

## Linker error

    /usr/bin/ld : CMakeFiles/anago.dir/reader_dummy.c.o:(.data.rel.ro.local+0x0) : définitions multiples de « DRIVER_DUMMY »; CMakeFiles/anago.dir/anago.c.o:(.rodata+0x60) : défini pour la première fois ici
    /usr/bin/ld : CMakeFiles/anago.dir/reader_kazzo.c.o:(.data.rel.ro.local+0x0) : définitions multiples de « DRIVER_KAZZO »; CMakeFiles/anago.dir/anago.c.o:(.rodata+0x0) : défini pour la première fois ici
    collect2: error: ld returned 1 exit status
    make[2]: *** [CMakeFiles/anago.dir/build.make:298 : anago] Erreur 1
    make[1]: *** [CMakeFiles/Makefile2:96 : CMakeFiles/anago.dir/all] Erreur 2
    make: *** [Makefile:171 : all] Erreur 2

<ins>**Solution :**</ins> This error can be solved by adding the **-fcommon** option for the ld linker before compilation :

In the root directory (**anago-master/**), run :

    $ rm -rf build
    $ mkdir build
    $ cd build
    $ export CFLAGS="$CFLAGS -fcommon"
    $ cmake ..
    $ make
    $ cp ../scripts/dumpcore.nut .

## USB permission error

    Could not find USB device "kazzo" with vid=0x16c0 pid=0x5dc; code: 1
    Reader open error

<ins>**Solution :**</ins> This error indicates that the user doesn't have the permission for accessing to USB device.
A simple workaround is to run Anago in **root** mode in a terminal, though this causes dumped ROM files to have root ownership/permissions :

   su
   ./anago d ../scripts/nrom.ad dumpfile.nes

or :

   sudo ./anago d ../scripts/nrom.ad dumpfile.nes

The proper solution is to create a udev rule to add the Kazzo device to a group with appropriate USB/serial permissions, such as uucp on Arch. 


## Core error

    Dump core script error

<ins>**Solution :**</ins> The files **dumpcore.nut** , **flashcore.nut** and **flashdevice.nut** must be into the same directory than the anago executable or into one of the directories :

  * ***${HOME}***/.config/anago/
  * ***CMAKE_INSTALL_PREFIX***/***SHARE_DIR***
  * ../scripts/

Example : In the build directory (**anago-master/build/**), run :

    $ cp ../scripts/*.nut .
    $ ./anago d ../scripts/nrom.ad dumpfile.nes

## Script Syntax error

    AN ERROR HAS OCCURRED [the index 'cpu_romsize' does not exist]
    
    CALLSTACK
    *FUNCTION [dump()] dumpcore.nut line [8]
    
    LOCALS
    [vram] 1
    [increase_ppu] 1
    [increase_cpu] 1
    [mappernum] XXX
    [d] USERPOINTER
    [this] TABLE

<ins>**Solution :**</ins> Rewrite the script with the syntax corresponding to the Anago version program.

If the script syntax was :

    board <- {
      mappernum = XXX,
      cpu_rom = {
        size_base = XXXXXX, size_max = XXXXXX, banksize = 0x4000
      },
      ppu_rom = {
        size_base = YYYYYY, size_max = YYYYYY, banksize = 0x2000
      },
      ppu_ramfind = false, vram_mirrorfind = true
    };

The valid syntax for this Anago version is :

    board <- {
      mappernum = XXX,
      cpu_romsize = XXXXXX, cpu_banksize = 0x4000,
      ppu_romsize = YYYYYY, ppu_banksize = 0x2000,
      ppu_ramfind = false, vram_mirrorfind = true
    };
