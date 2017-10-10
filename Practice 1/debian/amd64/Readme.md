### COM 341, Operating Systems
# Practice Task #1, Debian for AMD64 CPUs

### Installing Debian

Follow the steps to install the 64-bit AMD64 Debian Linux in QEMU.

1. Move into the project directory `./Practice 1/`.

        cd 'Practice 1/'

2. Download the hardware emulator QEMU for your operating system.

        # On Windows (from the Bash shell bundled with Git)
        ./utilities/download_qemu_win64.sh # or download_qemu_win32.sh on a
                                           # 32-bit Windows
        export PATH=$PATH:`cd qemu; pwd`

        # On macOS with [Homebrew](http://brew.sh)
        brew install qemu

        # On macOS with [The MacPorts Project](https://www.macports.org)
        sudo port install qemu

        # On Ubuntu Linux
        sudo apt install qemu-system-x86_64

3. Move into the directory `./debian/amd64`.

        cd 'debian/amd64/'

4. Download the Debian installation disk.

        ./utilities/download_debian.sh

5. Download the UEFI image for QEMU.

        ./utilities/download_uefi.sh

6. Build a flash image of the UEFI firmware and another one to store its
   settings.

        ./utilities/prepare_uefi_images.sh

7. Create a new hard drive image file to store the installed system.

        ./utilities/create_disk_image.sh

8. Start the emulator and the installation process.

        ./start_installation.sh

9. On Windows, switch to the serial console by pressing `CTRL+ALT+2` in QEMU.
   Use `Backspace` to select items on the screen.

10. Follow the steps to install the system.

![Step 1](http://i.imgur.com/qFWEBRp.png)
![Step 2](http://i.imgur.com/wuUwjYz.png)
![Step 3](http://i.imgur.com/UcfNHDs.png)
![Step 4](http://i.imgur.com/HuPwlLe.png)
![Step 5](http://i.imgur.com/I4lLGED.png)
![Step 6](http://i.imgur.com/o3zlG0H.png)
![Step 7](http://i.imgur.com/GpeXx8m.png)
![Step 8](http://i.imgur.com/jBDXiGE.png)
![Step 9](http://i.imgur.com/pu2swUv.png)
![Step 10](http://i.imgur.com/Oo0xgzs.png)
![Step 11](http://i.imgur.com/vA5nRSH.png)
![Step 12](http://i.imgur.com/4wwBcwJ.png)
![Step 13](http://i.imgur.com/O06jM2R.png)
![Step 14](http://i.imgur.com/B6mmMr2.png)
![Step 15](http://i.imgur.com/0tah7ra.png)
![Step 16](http://i.imgur.com/CQXrfxl.png)
![Step 17](http://i.imgur.com/ODF8UsM.png)
![Step 18](http://i.imgur.com/petlgcE.png)
![Step 19](http://i.imgur.com/bD3GQt0.png)
![Step 20](http://i.imgur.com/Dtjfa0Q.png)
![Step 21](http://i.imgur.com/7SihlVj.png)
![Step 22](http://i.imgur.com/dGcs0t1.png)
![Step 23](http://i.imgur.com/AZmvLXI.png)
![Step 24](http://i.imgur.com/S8V6Vdu.png)
![Step 25](http://i.imgur.com/sGkrul7.png)
![Step 26](http://i.imgur.com/YjefqIY.png)
![Step 27](http://i.imgur.com/GZBC9L4.png)
![Step 28](http://i.imgur.com/r0YCQys.png)

### Starting and Stopping the System

If the system is not running, you can start the QEMU emulator and Debian with
the following command

```bash
./start.sh
```

If you can not start the emulator on Windows, ensure the path to the QEMU
executable is in the $PATH environment variable.

```bash
echo $PATH

# If the QEMU directory is not there, from `debian/amd64/` execute
export PATH=$PATH:`cd ../../qemu; pwd`
```

To shutdown the system and exit from QEMU

```bash
# After you have started the system and logged-in with the user name
# and password specified during the installation process
sudo poweroff
```

### Installing Additional Software

1. Start the system and log in.

2. Install the GNU C compiler, the GNU Make build system, the GNU Debugger, and
   the Git version control system.

        sudo apt-get install gcc make gdb git

3. Shutdown the system.

### Fixing Networking Issues

1. If you experience connectivity issues in the emulated environment, try
   disabling the IPv6 support in the kernel.

        sudo nano /etc/default/grub

2. Append `ipv6.disable=1` to the `GRUB_CMDLINE_LINUX` variable and save the
   file.

3. Update the GRUB boot loader.

        sudo update-grub

4. Restart the system.

        sudo reboot

### Cleanup

1. Compress the hard drive image file.

        ./utilities/compress_disk_image.sh

2. Remove the old disk image.

        ./utilities/remove_old_files.sh
