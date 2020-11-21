# [User Guide] Device Mapper Proxy

[На русском языке User-Guide доступен по этой ссылке](./RU-GUIDE.md)

---

## Kernel module compile, install & testing
### Step 0: Prerequisites

This module is developed and tested in **Fedora 33**.  
Kernel used in tests is `Linux 5.9.8-200.fc33.x86_64`  

For successful module compilation you need `gcc` and
`make` tools installed. Use `dnf install gcc make` to
install it. 

Also corresponding version of *linux kernel headers is needed*. 
In Fedora they are provided by `kernel-devel` package. It can be 
installed with the command `dnf install kernel-devel`.

### Step 1: Build
Clone this repository and navigate into its directory. 
Use `make` command to compile module:

```
    $ make
```

### Step 2: Install
Use `insmod` command to insert module into a running
kernel. In this and next steps superuser permissions 
is required, use `sudo` or switch to root using `su`.

```
    $ su
    $ insmod device_mapper_proxy.ko
```

If something went wrong you can check log using 
`journalctl` or `dmesg` for kernel or module error
messages.

Check module is installed by running `lsmod`: 
```
    $ lsmod | grep "device_mapper_proxy"
```

### Step 3: Module testing
You can export device size in sectors for convenience
reasons:

```
	$ size=20000
```

Create a new test block device:
```
    $ dmsetup create zero1 --table "0 $size zero"
```

Create a new device mapper proxy device:
```
    $ dmsetup create dmp1 --table "0 $size dmp /dev/mapper/zero1"
```

Check devices listed in /dev/mapper:
```
    $ ls -al /dev/mapper
```

Make some reading and writing using `dd` utility:
```
    $ dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=1
    $ dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=1
```

Statistics available through sysfs:
```
	$ cat /sys/module/device_mapper_proxy/stat/volumes
```

Example output looks like this:
```
read:
 reqs: 549
 avg size: 916
write:
 reqs: 8
 avg size: 512
total:
 reqs: 557
 avg size: 916
```

For comparison, I/O request stats for /dev/mapper/dmp1 
(write and read request number should be equal):
```
    $ cat /sys/block/dm-1/stat
```
