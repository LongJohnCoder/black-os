<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/black_os_logo_trim.png" width="200">

# Changelog

All notable changes to this project will be documented in this file. 

## [Unreleased]

## [v1.3] - Cleanup

### Changed

- Massive cleanup in source code
- Added license on all files
- Applications can now be programmed via the BlackOS programmer [here]
- Applications can be launched from the SD card with the command "run"

Example
```
cd /Path to application
run application.exe
```

[here]: https://github.com/bjornbrodtkorb/BlackOS-Programmer/releases/tag/v1.1

## [v1.2] - Thread exit fix

### Changed

- Fixed kernel crashing after thread returns
- Fixed some file system formatting
- Kernel now stable with every normal C code

## [v1.1] - Application support

### Changed

- Added a dynamic loader with global offset table relocation
- Added application support

## [v1.0] - Kernel stable

### Changed

- First stable kernel with new queue system
- Added runtime statistics support


[Unreleased]: https://github.com/bjornbrodtkorb/BlackOS/compare/v1.3...HEAD
[v1.0]: https://github.com/bjornbrodtkorb/BlackOS/releases/tag/v1.0
[v1.1]: https://github.com/bjornbrodtkorb/BlackOS/releases/tag/v1.1
[v1.2]: https://github.com/bjornbrodtkorb/BlackOS/releases/tag/v1.2
[v1.3]: https://github.com/bjornbrodtkorb/BlackOS/releases/tag/v1.3
