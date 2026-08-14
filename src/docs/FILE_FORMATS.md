# CAN log and trace formats

`FrameFileIO` is SavvyLens's high-level load/save facade. It presents file dialogs, persists the last-used file location, dispatches to format-specific readers/writers, and supports automatic format detection for many inputs.

## Supported workflows

SavvyLens supports loading and/or saving a range of CAN-related formats, including native CSV, CRTD, generic CSV, BusMaster, Microchip, Vector trace/BLF, IXXAT, CAN-DO, Vehicle Spy, candump/Kayak, PCAN, Kvaser, CANalyzer, CARBUS Analyzer, CANHacker, Cabana, CANOpen Magic, Tesla Autopilot snapshots, CLX000, CANServer logs, and Wireshark PCAP/PCAPNG variants.

Exact read/write coverage varies by format. A format shown in the load dialog is not necessarily implemented for export.

## Auto-detection

Auto-detection tries format-specific identification routines before loading. If auto-detection is wrong or inconclusive, select the known format explicitly in the load dialog.

## Native CSV and bookmarks

Native CSV load/save may include bookmark persistence. Imported bookmark records are re-associated with frames using stored index, bus, frame ID, and timestamp information when possible.

## BLF and PCAP

- BLF support is implemented through `BLFHandler`.
- PCAP/PCAPNG support uses an internal lightweight reader for offline captures.
- Treat format support as pragmatic interoperability rather than a full implementation of every vendor specification.

If a file fails to load, preserve the original, try an explicit format selection, and capture the error/log information before modifying the parser.