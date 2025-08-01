# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- When compiled with `USB_DONGLE_TS1301=1` or `USB_DONGLE_TS1301=1` interface now accepts serialport (not hardcoded anymore)
- When compiled with `HW_SPI=1`, serialport parameter is not available, so it was removed.

### Added

- Switches to diferentiate usb devkits: `USB_DONGLE_TS1301` and `USB_DONGLE_TS1302`.
- Generic Unix SPI device support.

### Fixed
