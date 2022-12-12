# CMPE283-assignment-3
## Team Members:
1. Shubadha Paithankar(SJSU ID: 016013283)
2. Sai Manasa Yadlapalli (SJSU ID: 015999659)

## Assignment Questions
1. Subhadha : I implemented the VMX exit time duration counter for each VM exit part of assignment.3
I've measured the time duration between the start of the exit routine and it's exit.
Tested the workflow and debugged the implementation with a sample cpuid program in guest os vm and updated the readme file with required info

2. Manasa : I've implemented the VMX exit count per VM exit part of assignment.3
This exit counter array is implemented similar to the way it's mentioned in the instructor video
I've setup the dev environment with HostOS Ubuntu 2204 on VMWare workstation running on an Intel laptop.
Compiled the Linux kernel tree and installed the Guest VM based on virt-manager.

3. Comment on the frequency of exits – does the number of exits increase at a stable rate? Or are there
more exits performed during certain VM operations? Approximately how many exits does a full VMboot entail?
  - During the inner Guest VM boot sequeunce, we've seen a combined 10,626,554 total VM exits.
  - Some VM exit categories seem to be more frequent than the others and the rate of increase is not quite stable.
  - Exit reason count seem to be dependent on the type of VM operations performed.
  
4. Of the exit types defined in the SDM, which are the most frequent? Least?
  - Most frequent : <br>
    Exit type 48 : EPT violation accounts for ~89% of the total.
    Exit type 10 : CPUID invocation : Probably due to the test program
    Exit type 30 : I/O instructions
    Exit type 32 : WRMSR instruction
    Exit type 1 : External interrupts
    
  - Least frequent : <br>
    Exit type 29 : MOV DR instruction : Debug register
    Exit type 18 : VMCALL instruction by guest.

## Development Environment:
  1. Lenovo Ideapad Slimpro 14 laptop with Intel 11th Gen Intel(R) Core(TM) i5-11300H processor that supports VMX, VT-x,EPT and VT-d.
  2. Bare metal OS is Windows 11 with credential/device guard & virtualization based security disabled as per: https://kb.vmware.com/s/article/2146361
  3. Hypervisor is VMWare Workstation player 17
  4. Guest OS is Ubuntu 22.04

## Workflow Steps:
  1. Have a baseline host system running ubuntu 22.04 via VMware player or GCP instance
      
      Terminology:
      
     * Baremetal = Windows 11
     * Host OS   = Ubuntu 22.04 (via VMware player on windows 11)
     * Guest OS  = new VM instance of Ubuntu 22.04 using kvm,virt-mgr running on Host OS
      
      Install the following packages on host os for local linux kernel build. Choose default options if presented:
      
      `sudo apt install libncurses5 libncurses5-dev libncurses-dev qtbase5-dev-tools flex \
      bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf \
      fakeroot build-essential crash kexec-tools makedumpfile kernel-wedge`

      
  2. Clone the official Linux tree from github: https://github.com/torvalds/linux
  
      `git clone git@github.com:torvalds/linux.git`
      `cd linux`
  
  3. Switch to a kernel version branch similar to the current ubuntu host:
  
     ` git checkout "$(uname -r | cut -d. -f1-2)"`
  
  4. Create a baseline build config from existing host configuration:
  
      `make olddefconfig`
      
  5. Do the following customizations to the config file to get rid off trusted keys, debug information
     ``` scripts/config --set-str SYSTEM_TRUSTED_KEYS ""
      scripts/config --set-str SYSTEM_REVOCATION_KEYS ""
      scripts/config --undefine DEBUG_INFO
      scripts/config --undefine DEBUG_INFO_COMPRESSED
      scripts/config --undefine DEBUG_INFO_REDUCED
      scripts/config --undefine DEBUG_INFO_SPLIT
      scripts/config --undefine GDB_SCRIPTS
      scripts/config --set-val  DEBUG_INFO_DWARF5     n
      scripts/config --set-val  DEBUG_INFO_NONE       y
      
  Verify the changes via : `scripts/diffconfig .config{.old,}`
      
  6. Build the modules and kernel
  
      `make -j "($nproc)" modules && make -j "($nproc)"`
  
  7. Install the modules and the newly built kernel to host
  
      `sudo make INSTALL_MOD_STRIP=1 modules_install && sudo make install`
      
  8. Verify that you can see the new modules installed at: /var/lib/modules (should be 5.15.0+ dir here)
     new kernel image and init image here: /boot  (vmlinuz-5.15.0+ and initrd.img-5.15.0+)
     
  9. Switch to the newly installed kernel explicitly by kexec:
  
     `sudo kexec -l /boot/vmlinuz-5.15.0+ --initrd=/boot/initrd.img-5.15.0+ --reuse-cmdline`
     
     `sudo systemctl kexec`
  
  10. Assignment 2,3 are cpuid changes on kvm module. Exercising this requires a KVM based nested VM on host OS.
      We installed a ubuntu 2204 guest vm on host ubuntu 2204 running on the newly built kernel via:
      
      `sudo apt install libvirt-clients libvirt-daemon-system libvirt-daemon virtinst bridge-utils qemu qemu-kvm virt-manager`
      
  11. Check kvm installation status by running:
  
     `kvm-ok`
  
  12. To be able to install and run virt-manager as a regular user:
  
      `sudo chown username:username /var/run/libvirt/libvirt-sock`
      
  13. Downloaded, configured and installed a guest VM ubuntu2204 desktop image.
  
  14. Within the guest VM, installed the cpuid package to exercize the modified kvm behavior via e.g.
  
      `sudo apt install cpuid`
      ```
      assignment 2: cpuid -l 0x4ffffffc
                    cpuid -l 0x4ffffffd
      assignment 3: cpuid -l 0x4ffffffe -s exit_number
                    cpuid -l 0x4fffffff -s exit_number

  15. The code change, compile, run, check cycle once the host is running with modified kernel with guest os setup via virt-mgr is:
      
      `make -j "($nproc)" modules`
      
      `sudo make INSTALL_MOD_STRIP=1 modules_install`
      
      * Turn off guest VM to remove kvm dependency
      
      `sudo rmmod kvm_intel`
      
      `sudo rmmod kvm`
      
      `sudo modprobe kvm`
      
      `sudo modprobe kvm_intel`
      
      * Turn on guest VM and run cpuid to verify the changes
      

## Assignment Test Output over a VM boot:
1. Assignment 3 Leaf 0x4ffffffe:
  ![image](https://user-images.githubusercontent.com/87613567/207153831-b019ab44-b01a-4da1-a891-b5eaa1432478.png)
  ![image](https://user-images.githubusercontent.com/87613567/207154136-f8e095c1-748f-4908-b91b-c38affcbde68.png)

2. Assignment 3 Leaf 0x4fffffff:
  ![image](https://user-images.githubusercontent.com/87613567/207154308-ac472887-8201-4d63-9f21-4d96943cd3c1.png)
  ![image](https://user-images.githubusercontent.com/87613567/207154418-09caed63-d95e-448d-ae18-3e28faa0ac51.png)


## References: 
1. Building linux kernel from scratch on Ubuntu 2204: https://saveriomiroddi.github.io/A-precise-guide-to-build-a-custom-linux-kernel/
2. Booting to the custom built installed kernel via kexec: https://wiki.archlinux.org/title/kexec
3. Installing kvm based vm inside the ubuntu 2204: https://www.virtualizationhowto.com/2022/09/install-kvm-ubuntu-22-04-step-by-step/
4. KVM virtual manager permission setup for regular users: https://askubuntu.com/questions/932200/kvm-virt-manager-error-no-active-connection-to-installed-on
5. EXPORT_SYMBOL_GPL requirement when using shared variables : https://stackoverflow.com/questions/22712114/what-is-export-symbol-gpl-in-linux-kernel-code
