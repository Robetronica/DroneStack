# Migration Guide: STM32N6570-DK to OpenMV N6

This document tracks the hardware differences and necessary software adjustments when porting code from the STM32N6570-DK Discovery kit to the OpenMV N6 module.

## Hardware Comparison & Required Changes

### 1. External Flash (XSPI)
| Feature | STM32N6570-DK | OpenMV N6 |
| :--- | :--- | :--- |
| **Part Number** | `66UW1G45GXDI00T` | `MX25UM25645GXDI00` |
| **Capacity** | 1 Gbit (128 MB) | 256 Mbit (32 MB) |

**Impact:**
*   **Linker Script:** The memory region for the external flash must be reduced from 128MB to 32MB to prevent the linker or runtime code from accessing non-existent memory.
*   **Initialization & Pins:**
    *   **Reset Pin:** The OpenMV N6 uses **PN12** for `XSPIM_P2_RST_INT#`. This reset line must be configured and asserted/de-asserted correctly during the XSPI initialization sequence.
    *   **Differential Clock:** The OpenMV N6 utilizes **PN7** for `XSPIM_P2_CLK_N` (the complementary clock).
        *   **Reason:** High-speed Octo-SPI (XSPI) often uses differential clocking (CLK and CLK_N) to improve signal integrity and reduce electromagnetic interference (EMI) when operating at high frequencies (especially in DTR/DDR modes).
    *   **Interrupt Pin:** The OpenMV N6 connects **PO5** to `XSPIM_P2_INT#`.
        *   **Purpose:** The `#` indicates an active-low interrupt. This is typically used by the flash memory to signal the host (STM32) when an internal operation (like an Erase or Program cycle) is complete, or to signal an error, allowing the CPU to perform other tasks instead of polling the flash status register.
    *   **Compatibility:** Verify if the command sets or timing requirements differ between the 1 Gbit and 256 Mbit parts, although both are typically compatible with the XSPI interface.

### 2. Debugging & Deployment (`launch.json`)
The change in flash memory requires updates to your debug configuration in VS Code.

**Impact:**
*   **Flash Loader/Algorithm:** The `launch.json` (or your debugger's configuration) may need to point to a different flash loader algorithm (`.elf` or `.stldr` file) that specifically supports the Macronix `MX25UM25645G`. Using a loader intended for the 1 Gbit Macronix part on the 256 Mbit part might cause verification errors or incorrect sector mapping.
*   **Memory Ranges:** Ensure any memory-watch or peripheral view configurations in `launch.json` reflect the reduced 32MB address space for the external flash to avoid debugger errors when trying to read out-of-bounds memory.
