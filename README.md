# FSMCraft

A visual editor for creating, interpreting, and monitoring finite state machines with real-time execution and JavaScript-based actions.

## 👥 Authors

- Mikuláš Lešiga (xlesigm00)
- Alex Marinica (xmarina00)
- Filip Novák (xnovakf00)

## 📋 What It Does

FSMCraft is a Qt5-based graphical tool for working with finite state machines:
- **Visual Editor** - Draw states and transitions with interactive canvas
- **JavaScript Actions** - Write state actions and guard conditions in JavaScript
- **Real-time Execution** - Run and monitor automata with live feedback
- **TCP Communication** - Client-server architecture for FSM control
- **JSON Import/Export** - Save and load automata definitions

## 🎨 Features

### GUI Components

**Left Panel:**
- **Control Buttons**: Run/Stop, New State, Clear Canvas, Save, Upload
- **FSM Configuration**: Name your automaton
- **Variables**: Define internal variables with initial values
- **Signals**: Declare input and output signals
- **Input Injection**: Send inputs to running automata

**Right Panel:**
- **FSM Canvas**: Interactive whiteboard for building automata
- **State Operations** (right-click menu):
  - Set Initial - Mark entry point
  - Connect - Create transitions with triggers, guards, and timeouts
  - Edit Action - Modify JavaScript action code
- **Log View**: Real-time execution feedback and diagnostics

### JavaScript Support

Built-in functions for state actions:
```javascript
fsm.output("name", "value")  // Set output signal
fsm.defined("name")           // Check if variable/input exists
fsm.elapsed()                 // Get time in ms spent in current state
```

### Message Protocol

TCP-based JSON protocol with message types:
- LOG, INPUT, STOP, JSON
- ACCEPT, REJECT, EMPTY, REQUEST

## 📁 Project Structure

```
FSMCraft/
├── src/                # Source code
│   ├── fsmController   # Server-side FSM control
│   └── guiController   # Client-side GUI control
├── examples/           # Example automata (JSON files)
├── CMakeLists.txt      # CMake build configuration
├── Makefile            # Make build file
├── Doxyfile            # Documentation configuration
├── FSMCraftClass.pdf   # Class diagram
└── r.sh                # Run script with Qt configuration
```

## 🔨 How to Build

### Quick Start

```bash
make run
```

### Manual Build (with custom Qt path)

```bash
./r.sh
```

Or configure manually:
```bash
export Qt5_DIR=/path/to/Qt/lib/cmake/Qt5
export CMAKE_PREFIX_PATH=/path/to/Qt/lib/cmake
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH
make -j$(nproc)
./src/fsmtool
```

## 🚀 Available Commands

```bash
make build    # Build the project
make run      # Build and run the editor
make clean    # Remove build files
make doxygen  # Generate documentation
make pack     # Create submission zip file
```

## 📝 Usage Guide

1. **Start the application**
   ```bash
   make run
   ```

2. **Create your automaton:**
   - Add states using "New State" button
   - Right-click states to set initial state or create transitions
   - Define transitions with input triggers, guard conditions, and timeouts
   - Write JavaScript actions for each state

3. **Configure signals:**
   - Declare input signals in the left panel
   - Define output signals
   - Set internal variables with initial values

4. **Run and test:**
   - Click "Run" to start execution
   - Inject inputs during runtime
   - Monitor execution in the log view
   - Transitions animate with 1-second delays for visualization

5. **Save/Load:**
   - Save automata as JSON files
   - Load examples from `examples/` folder
   - Export for later use

## 🔧 Advanced Features

- **Persistent Execution**: Exit GUI while automaton runs, reconnect later
- **Real-time Monitoring**: View state transitions and timing information
- **Debug Output**: Terminal logs for debugging
- **Guard Conditions**: Boolean expressions for conditional transitions
- **Timeout Transitions**: Time-based state changes

## 📚 Documentation

Generate full documentation:
```bash
make doxygen
```

View class diagram: `FSMCraftClass.pdf`

## 🎓 Course Info

**Course**: Formal Languages and Compilers  
**School**: FIT VUT Brno  
**Project**: FSM Visual Editor and Interpreter

## ⚙️ Requirements

- C++ compiler (g++)
- CMake 3.10 or later
- Qt 5 (Qt5::Widgets, Qt5::Qml)
- Make
- Doxygen (for documentation)

## 📦 File Format

FSMCraft uses a specialized JSON format for saving automata. Example files can be found in the `examples/` directory. The format includes:
- States with actions
- Transitions with guards and timeouts
- Variable and signal definitions
- FSM metadata

---

**Note**: Academic project - follow your school's academic integrity policy.
