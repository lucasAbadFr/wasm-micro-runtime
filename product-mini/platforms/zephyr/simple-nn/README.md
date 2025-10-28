# WASI NN sample

> 🧠 *This README was initially generated with AI assistance and serves as a structured placeholder. It will be refined as the project evolves and more details become available.*

## Overview

This project explores the integration of the **WASI-NN API** within **Zephyr RTOS** through the **WebAssembly Micro Runtime (WAMR)**.  
The goal is to demonstrate that neural network inference through **TensorFlow Lite Micro (TFLM)** can be invoked from a WebAssembly module running on a constrained embedded system.

This is an experimental effort toward bringing the WASI-NN specification to microcontroller-class devices.

## Objectives

1. **Enable WASI-NN within WAMR** on Zephyr targets.
2. **Implement a lightweight backend** based on TensorFlow Lite Micro.
3. **Demonstrate execution** of a minimal inference workload from a WASM module.
4. Investigate portability, resource usage, and API compliance with WASI-NN.

## Environment

| Component | Version / Branch | Notes |
|------------|------------------|-------|
| **WAMR** | `main` + `fix/zephyr_product_mini` | Local branch to fix Zephyr build issues |
| **Zephyr** | `v3.7.0` (LTS) | Stable long-term branch |
| **WASI SDK** | `24.0` | Used for compiling WASM sample applications |
| **TensorFlow Lite Micro** | Commit `48613f7ba1ffbda46ad771a77a35408f48f922e9` | Pulled from Zephyr’s optional modules |

## Current Status

| Area | Status | Details |
|------|---------|----------|
| Build integration | ✅ | WAMR + Zephyr compile successfully with TFLM support enabled |
| WASI-NN orchestration | ⚙️ In progress | Need to decouple backend API and orchestration logic |
| TensorFlow Lite Micro backend | 🚧 Planned | New backend target: `WAMR_BUILD_WASI_NN_TFLITE_MICRO` |
| WASM sample inference | ⏳ Not started | Will replicate a small TFLM example as first workload |
| Shared-lib build flow | ⚠️ Adaptation required | `WASI_EPHEMERAL_NN` originally assumes shared library build; Zephyr will link statically |

## Next Steps

1. **Refactor WASI-NN build flow** for static integration under Zephyr.  
2. **Define backend interface** for `TFLITE_MICRO` targeting embedded builds.  
3. **Create and compile a minimal WASM sample** performing inference via WASI-NN.  
4. **Benchmark and debug** stack/heap usage and performance on hardware.  
5. **Evaluate modularization** and possible reduction of Zephyr dependencies (network, FS, etc.).

## Notes
- The configuration currently enables both filesystem and networking stacks required by `WAMR_BUILD_LIBC_WASI`, though these may be stripped down later.
- The project builds and flashes on **FRDM-MCXN947 (NXP)** using the `linkserver` runner.
- Logging and debug output use Zephyr’s `printk` and `LOG` APIs.
