Example for Secure Boot on Nuvoton's TrustZone capable platform
###############################################################

Overview
********

This example shows the isolation which is similar to PSA level 1
- ``mcuboot``: runs in SPE for secure boot
- ``smp_svr``: runs in NSPE as SMP server

Support targets
===============

+----------------------+-------------------------------+-----------------------+
| Board                | Zephyr target                 |Comment                |
+======================+===============================+=======================+
| `NuMaker-M3351KI`_   | `numaker_m3351ki/m335xxx/s`_  |Secure board target    |
+----------------------+-------------------------------+-----------------------+
| `NuMaker-M3351KI`_   | `numaker_m3351ki/m335xxx/ns`_ |Non-Secure board target|
+----------------------+-------------------------------+-----------------------+

.. _NuMaker-M3351KI: https://docs.zephyrproject.org/latest/boards/nuvoton/numaker_m3351ki/doc/index.html
.. _numaker_m3351ki/m335xxx/s: `NuMaker-M3351KI`_
.. _numaker_m3351ki/m335xxx/ns: `NuMaker-M3351KI`_
.. _NuMaker-M3351KI board: `NuMaker-M3351KI`_

Hardware requirements
=====================

- `NuMaker-M3351KI board`_

.. hint:: This example needs to build and run on TrustZone capable platform.
   In this document, `NuMaker-M3351KI board`_ is taken for demo.

- USB type-c cable

Software requirements
=====================

- Host operating system: Windows 10 64-bit or afterwards

  Most users of Nuvoton's Cortex-M series SoC develop on Windows,
  so this document favors this environment.

  The command lines in this document are verified on Windows Git Bash environment.
  For other shell environments, check on how differently shells use line continuation,
  quotation marks, and escapes characters.
  For Bash, line continuation mark is "\\".

- `Zephyr development environment`_

  .. _Zephyr development environment: https://docs.zephyrproject.org/latest/develop/index.html

- `Git`_

  This document favors Git Bash as CLI environment.

  .. _Git: https://git-scm.com/

- Cross GCC compiler

  Use `Zephyr SDK toolchain`_ instead of `Arm GNU Toolchain`_ to avoid build failure
  caused by toolchain difference.

  .. _Zephyr SDK toolchain: https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-the-zephyr-sdk
  .. _Arm GNU Toolchain: https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain
  
- PyOCD

  Zephyr invokes pyOCD to download built binary.
  PyOCD doesn’t have built-in support for M3351 series MCU.
  Add the support via Nuvoton’s CMSIS Pack:
  - Download ``Nuvoton.NuMicroM33_DFP.x.y.z.pack`` where ``x.y.z`` is the most recent version.
  - Under west workspace directory named ``zephyrproject``,
  create one pyOCD YAML file named ``pyocd.yaml``
  which has the content as follows:

  .. code-block:: yaml 

    pack:
      - C:\Users\<USER>\Downloads\Nuvoton.NuMicroM33_DFP.<x.y.z>.pack

- `mcumgr-client`_

  This command-line tool is used to interact with ``smp_svr`` program
  for device management.

  .. _mcumgr-client: https://github.com/vouch-opensource/mcumgr-client/
    
    
Building and Running
********************

Preparing the source code
=========================

This example doesn't upstream to mainline,
and users need to switch to specific branch.

.. _Zephyr workspace application: https://docs.zephyrproject.org/latest/develop/application/index.html#zephyr-workspace-application

__ `Zephyr development environment`_

.. code-block:: console

    $ cd zephyrproject
    $ mkdir applications
    $ cd applications
    $ git clone https://github.com/OpenNuvoton/NuMaker-Zephyr-TFLM-KWS
    $ cd ..
    
Now, we get back to `zephyrproject`.
Add the `tflite-micro` module to your West manifest and pull it:

.. code-block:: console

    $ west config manifest.project-filter -- +tflite-micro
    $ west update

Building the example
====================

We need to build ``mcuboot`` one time and ``smp_svr`` two times.
Build ``mcuboot`` first:

.. code-block:: console

    $ west -v build \
    -b numaker_m3351ki/m335xxx/s \
    -d build_boot \
    --extra-conf `west topdir`/my_misc/mcuboot/my_extra_conf.conf \
    bootloader/mcuboot/boot/zephyr \
    -- \
    -DCONFIG_BOOT_SWAP_USING_SCRATCH=y \
    -DCONFIG_BOOT_SIGNATURE_TYPE_ECDSA_P256=y \
    -DCONFIG_BOOT_SIGNATURE_KEY_FILE='"root-ec-p256.pem"' \
    -DCONFIG_BOOT_ENCRYPT_IMAGE=y \
    -DCONFIG_BOOT_ENCRYPTION_KEY_FILE='"enc-ec256-priv.pem"' \
    -DCONFIG_BOOT_INTR_VEC_RELOC=y \
    -DCONFIG_MCUBOOT_DOWNGRADE_PREVENTION=y \
    -DCONFIG_MCUBOOT_DOWNGRADE_PREVENTION_SECURITY_COUNTER=y

Then build ``smp_svr`` of version ``v1.0.0``:

.. code-block:: console

    $ west -v build \
    -b numaker_m3351ki/m335xxx/ns \
    -d build_v1_0_0 \
    --extra-conf cdc.conf \
    --extra-conf shell-mgmt.conf \
    --extra-dtc-overlay usb.overlay \
    zephyr/samples/subsys/mgmt/mcumgr/smp_svr \
    -- \
    -DCONFIG_BUILD_WITH_TFM=n \
    -DCONFIG_BOOTLOADER_MCUBOOT=y \
    -DCONFIG_MCUBOOT_BOOTLOADER_MODE_SWAP_SCRATCH=y \
    -DCONFIG_MCUBOOT_SIGNATURE_KEY_FILE='"bootloader/mcuboot/root-ec-p256.pem"' \
    -DCONFIG_MCUBOOT_ENCRYPTION_KEY_FILE='"bootloader/mcuboot/enc-ec256-pub.pem"' \
    -DCONFIG_MCUBOOT_EXTRA_IMGTOOL_ARGS='"--security-counter auto"' \
    -DCONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION='"1.0.0"'

Finally rebuild ``smp_svr`` of version ``v1.0.1``:

.. code-block:: console

    $ west -v build \
    -b numaker_m3351ki/m335xxx/ns \
    -d build_v1_0_1 \
    --extra-conf cdc.conf \
    --extra-conf shell-mgmt.conf \
    --extra-dtc-overlay usb.overlay \
    zephyr/samples/subsys/mgmt/mcumgr/smp_svr \
    -- \
    -DCONFIG_BUILD_WITH_TFM=n \
    -DCONFIG_BOOTLOADER_MCUBOOT=y \
    -DCONFIG_MCUBOOT_BOOTLOADER_MODE_SWAP_SCRATCH=y \
    -DCONFIG_MCUBOOT_SIGNATURE_KEY_FILE='"bootloader/mcuboot/root-ec-p256.pem"' \
    -DCONFIG_MCUBOOT_ENCRYPTION_KEY_FILE='"bootloader/mcuboot/enc-ec256-pub.pem"' \
    -DCONFIG_MCUBOOT_EXTRA_IMGTOOL_ARGS='"--security-counter auto"' \
    -DCONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION='"1.0.1"'

Monitoring the example
======================

To monitor the example, we need to:

- Configure host terminal program with **115200/8-N-1**

First, flash ``mcuboot`` image:

.. code-block:: console

  $ west -v flash \
  -d build_boot

On host terminal, we would see **Unable to find bootable image**:

.. code-block:: console

    I: Starting bootloader
    I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Scratch: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Boot source: primary slot
    I: Image index: 0, Swap type: none
    E: Unable to find bootable image

Second, flash ``smp_svr`` of version ``v1.0.0``:

.. code-block:: console

    $ west -v flash \
    -d build_v1_0_0

The image ``smp_svr`` of version ``v1.0.0`` is chain-loaded:

.. code-block:: console

    I: Starting bootloader
    I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Scratch: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Boot source: primary slot
    I: Image index: 0, Swap type: none
    I: Bootloader chainload address offset: 0x80000
    I: Image version: v1.0.0
    I: Jumping to the first image slot
    I: Boot Non-Secure image (0x10080400)

Plug-in USB cable:

.. code-block:: console

    <inf> usbd_core: Actual device speed 1
    <inf> usbd_core: Actual device speed 1
    <inf> usbd_cdc_acm: Configuration enabled

In Windows **Device Manager**, we would find new COM port device detected.
Make a note as **<COM-SMP>**.

Upload the image ``smp_svr`` of ``v1.0.1`` to second slot:

.. code-block:: console

    $ mcumgr-client \
    --device <COM-SMP> \
    --mtu 128 \
    upload --slot 1 \
    build_v1_0_1/zephyr/zephyr.signed.encrypted.bin

List the images installed:

.. code-block:: console

    $ mcumgr-client --device <COM-SMP> list


Make a note of hash code of version ``v1.0.1`` as **<HASH-V1.0.1>**:

.. code-block:: console

    response: {
      "images": [
        {
          "image": 0,
          "slot": 0,
          "version": "1.0.0",
          "hash": "46fcc605efd88d5a15a2efdfa63fce06e30f597785b3cc92eee1452a4f2c5ae9",
          "bootable": true,
          "pending": false,
          "confirmed": true,
          "active": true,
          "permanent": false
        },
        {
          "image": 0,
          "slot": 1,
          "version": "1.0.1",
          "hash": "019bae01900ac03b6338ef86fd32013d2aa5e250a38789c5e772738293a70e71",
          "bootable": true,
          "pending": false,
          "confirmed": false,
          "active": false,
          "permanent": false
        }
      ],
      "splitStatus": "NotApplicable"
    }

Mark the uploaded image as pending but not confirmed for test boot:

.. code-block:: console

    $ mcumgr-client \
    --device <COM-SMP> \
    test --confirm false \
    <HASH-SMP>

.. note:: If ``--confirm true`` is specified, the uploaded image will
   become permanently active (without roll-back) after device reset
   in the following.

Reset the device for firmware update:

.. code-block:: console

    $ mcumgr-client --device <COM-SMP> reset

The version would change to ``v1.0.1``:

.. code-block:: console

    I: Starting bootloader
    I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Scratch: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Boot source: primary slot
    I: Image index: 0, Swap type: test
    I: Starting swap using scratch algorithm.
    I: Bootloader chainload address offset: 0x80000
    I: Image version: v1.0.1
    I: Jumping to the first image slot
    I: Boot Non-Secure image (0x10080400)

Re-reset the device:

.. code-block:: console

    $ mcumgr-client --device <COM-SMP> reset

The version ``v1.0.1`` doesn't get confirmed,
so the version ``v1.0.0`` is rolled back:

.. code-block:: console

    I: Starting bootloader
    I: Primary image: magic=good, swap_type=0x2, copy_done=0x1, image_ok=0x3
    I: Scratch: magic=bad, swap_type=0x1, copy_done=0x2, image_ok=0x2
    I: Boot source: none
    I: Image index: 0, Swap type: revert
    I: Starting swap using scratch algorithm.
    I: Bootloader chainload address offset: 0x80000
    I: Image version: v1.0.0
    I: Jumping to the first image slot
    I: Boot Non-Secure image (0x10080400)

Further reading
***************

Optimizing model using Vela
===========================

Trouble-shooting
****************
