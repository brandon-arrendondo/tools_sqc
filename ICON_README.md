# SQC Icon Installation

The SQC (Software Code Quality) program includes icon files that can be used in different environments.

## Icon Files

- `icon.png` - PNG format (256x256) for Linux desktop environments
- `icon.ico` - ICO format (multi-resolution) for Windows

## Where the Icon Appears

### 1. Terminal User Interface (TUI)
The icon is integrated into the TUI as a visual element:
- Header shows: `✓ { SQC }` with colored styling
- Footer shows: `✓{ SQC } - Software Code Quality`
- The checkmark (✓) is green and the brackets ({}) are cyan

### 2. Terminal Window Title
When running in interactive mode, the terminal window title is set to:
`sqc - Software Code Quality`

### 3. Linux Desktop Launcher (Optional)
To add SQC to your application menu:

```bash
# Copy the icon to system icons directory
sudo cp icon.png /usr/local/share/pixmaps/sqc.png

# Copy the desktop file
sudo cp sqc.desktop /usr/local/share/applications/

# Update desktop database
sudo update-desktop-database
```

### 4. Windows Executable (Windows only)
When built on Windows, the icon is embedded in the .exe file and will appear:
- In the taskbar when the program is running
- In the window title bar
- In Windows Explorer for the executable file

To build with icon on Windows:
```bash
cargo build --release
```

The `build.rs` script automatically embeds `icon.ico` during compilation on Windows.

## Icon Design

The icon features:
- Green rounded square background (representing code quality/success)
- White checkmark (representing validation/compliance)
- Code brackets `{ }` (representing C code checking)
- Simple, recognizable design at all resolutions (16x16 to 256x256)

## Limitations

- **Linux/WSL Terminal**: Terminal emulators don't support custom application icons in the same way GUI applications do. The icon is displayed within the TUI interface itself.
- **macOS Terminal**: Similar to Linux, the icon appears within the TUI interface
- **Windows**: Icon appears in window chrome and taskbar when running the compiled .exe

## Customization

To create your own icon:
1. Edit `icon.png` with your preferred image editor
2. Convert to .ico for Windows: `convert icon.png -define icon:auto-resize=256,128,64,48,32,16 icon.ico`
3. Rebuild the project: `cargo build --release`
