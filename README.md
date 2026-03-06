# nasi-loader

nano-sized asi loader for windows. no fancy features, just a simple LoadLibrary caller.

### Usage

Put both the nasi-loader DLL and the *.asi modules you want to load in the same folder as the executable you're launching.

Supported names for the nasi-loader DLL:
- `dinput.dll`
- `dinput8.dll`
- `dsound.dll`
- `version.dll`
- `winmm.dll`
