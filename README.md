# FSMCraft

A visual editor for creating and editing finite state machines (automata).

## 📋 What It Does

FSMCraft is a graphical tool for working with automata:
- Visual editor for creating state machines
- Draw states and transitions
- Test automata with input strings
- Convert between different automata types
- Export and import automata

## 📁 Project Structure

```
FSMCraft/
├── src/                # Source code
├── examples/           # Example automata
├── CMakeLists.txt      # CMake build configuration
├── Makefile            # Make build file
├── Doxyfile            # Documentation configuration
├── FSMCraftClass.pdf   # Class diagram
└── r.sh                # Run script
```

## 🔨 Available Commands

```bash
make build    # Build the project
make run      # Build and run the editor
make clean    # Remove build files
make doxygen  # Generate documentation
make pack     # Create submission zip file
```

### Using Make

```bash
make
```

## 🚀 How to Run

### Quick Start

```bash
make run
```

This will automatically build and run the editor.

### Manual Build (if Qt path needs configuration)

If you need to specify Qt installation path:

```bash
./r.sh
```

## 🎨 Features

- **Visual Editor** - Draw states and transitions with mouse
- **State Management** - Add, remove, and modify states
- **Transition Editor** - Define transitions between states
- **Testing** - Test your automata with input strings
- **Examples** - Pre-made automata in the `examples/` folder

## 📚 Documentation

Generate documentation:
```bash
doxygen Doxyfile
```

View class diagram: `FSMCraftClass.pdf`

## 🎓 Course Info

**Course**: Formal Languages and Compilers  
**School**: FIT VUT Brno  
**Project**: Automata Visual Editor

## ⚙️ Requirements

- C++ compiler (C++11 or later)
- CMake
- Make
- Qt 5.5 or later
- Doxygen (for documentation)

## 👥 Contributors

This project has 3 contributors.

---

**Note**: Academic project - follow your school's academic integrity policy.
