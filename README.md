# Standalone Super Sixteen Sequencer

This repository contains a hardware project built in KiCad—a reimagined and streamlined version of the [Super Sixteen project](https://github.com/matthewcieplak/super-sixteen). The goal is to create a standalone sequencer suitable for Eurorack and other synthesizer setups.

## Project Overview

The original Super Sixteen design has been reworked into a modular, three-board system:
- **7 Segment Display Board:** Manages visual output.
- **DAC and Mono Jack Board:** Provides digital-to-analog conversion and audio interfacing.
- **16-Step Keyboard Board:** Features 16 Cherry MX switches paired with reverse-mount RGB LEDs for step sequencing.

> All pins are broken out for flexible connectivity. The boards are designed to work together or independently based on your setup.

## Key Modifications

- **Simplified Design:** Removed the IO expander from the original Super Sixteen to reduce complexity.
- **Modular Approach:** Each board can function as part of a larger system or as a standalone unit.
- **Fabrication Update:** The PCBs are currently in fabrication. This project is a work in progress (WIP).

## Getting Started

1. **Hardware Designs:** Navigate to the appropriate subfolder in the `hardware` directory to access the KiCad schematic and PCB layout files.
2. **Software Development:** The `software` folder is currently empty. Firmware or control software will be added as the project develops.
3. **Documentation:** Detailed build instructions, assembly guides, and design notes will be provided in the `documentation` folder over time.

## Inspiration

This project draws inspiration from the original [Super Sixteen project](https://github.com/matthewcieplak/super-sixteen). Visit their repository to learn more about the design and evolution of the sequencer.

## License

Code: GPL3.0 Hardware: cc-by-sa-3.0

## Contributing

Contributions, issues, and feature requests are welcome. Please feel free to open an issue or submit a pull request.
