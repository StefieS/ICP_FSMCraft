Project name: FSMCraft
Description: Tool for visual editing, interpreting and monitoring flow of finite automata
Authors: Mikuláš Lešiga xlesigm00
         Alex Marinica xmarina00
         Filip Novák xnovakf00

GUI:
    The graphical user interface of FSMCraft is developed using Qt5 and emphasizes clarity and precise interaction with finite state machines.
    The main window is logically divided into two primary sections: a control panel on the left and the FSM canvas on the right.
    In the upper-left quadrant, users interact with a compact toolbar featuring essential control buttons:
    - Run FSM – initiates or resumes execution of the automaton.
    - New State – creates a new state node, prompting the user to provide a name and optional JavaScript-style action code.
    - Clear Canvas – resets the working canvas, removing all visual elements.
    - Save FSM – serializes the current automaton into a JSON file.
    - Upload – loads a previously saved FSM definition from a JSON file.

    Beneath the toolbar, users can specify the name of the current automaton, which is stored in the JSON export for identification.
    The internal variable section allows the declaration of internal state variables used within state action code.
    Each declared variable can be assigned an initial value and removed via the interface.
    Similarly, users may define input signals and output signals, which are integral to FSM logic and runtime behavior.
    Defined inputs are made available for real-time injection during execution.
    The right-hand side of the application hosts the FSM builder, an interactive canvas where states and transitions are visually composed.
    States can be manipulated via right-click context menus, offering the following actions:
    Set Initial – designates the selected state as the automaton's entry point.
    Connect – initiates transition creation from the selected state.
    Upon clicking a target state, the tool prompts the user to define the transition’s input trigger (linked to declared inputs),
    an optional guard condition (a boolean expression evaluating injected data), and an optional timeout in milliseconds.
    Edit Action – allows users to modify the JavaScript-based action logic tied to a specific state.
    Transitions are rendered as curved paths with dynamically positioned labels and directional arrowheads.
    They are visually updated to reflect FSM execution flow during runtime.
    At the bottom of the interface, a log view provides real-time feedback and diagnostics.
    This panel logs essential runtime events such as state activations, transitions, injected inputs, and evaluation results.
    It serves as a crucial aid for debugging and monitoring the execution of FSMs in simulation.


External specification:
    For external specification, specialised Json format was used,
    where all needed information about the FSM resign for loading into
    GUI, transforming to common representation or building the logic of the FSM.
    Classes JsonLoader and JsonMaker are responsible for seamless transforming.
    Examples of such saved automata can be found in /examples.

Action and condition language:
    An internal interpeter of JavaScript JSEngine is used, therefore 
    actions of the states can be written in this language.
    Conditions for the transitions have to evaluate to boolean.
    Internal variables are accessible.
    3 builtin functions were implemented:
        fsm.output("nameO", "valueO") -> sets the output nameO to value valueO
        fsm.defined("name") -> returns boolean whether such variable/input had been defined
        fsm.elapsed() -> returns time in ms which elapsed in current state

Interpetation of the run:
    A subclass of QStateMachine was implemented to handle the logic.
    Custom transitions and state actions perform the expected behaviour.

Message protocol:
    For sending TCP messages to communicate between the server and client,
    specialised Json protocol was used, having multiple types and associated 
    information needed for performing actions:
    enum:
         LOG,
         INPUT,
         STOP,
         JSON,
         ACCEPT,
         REJECT,
         EMPTY,
         REQUEST.

Controllers:
    Two controllers were implemented for reacting to the message:
        fsmController:
            Used for performing server-side actions and controlling automaton.
        guiController:
            Used for performing client-side actions and conrolling GUI.
        They handle various scenarios in performAction function bodies.

Requirements: 
    C++ Compiler: 
        g++
        Must support at least CMake 3.10.
    CMake: 
        Minimum Version: 3.10
    Qt 5:
        Qt5::Widgets
        Qt5::Qml

Functionality:
    1. Start the application with "make run"

    2. Now you can see the main window,
    there are three separate parts:
        Left-side -> a. buttons in order: run/stop, state, clear, save, upload, text array for file name of the automaton
                     b. Input/internalVar/Output layout: here you can declare outputs/inputs and define internal vars
                     c. inject input: used to inject input into running automata
        Right-topside -> whiteboard for constructing and viewing your automaton
        Right-bottomside -> displays logs

    3. If you exit gui, while some automaton is running, it keeps running and you can join it back 
    and display the automaton, as well as stop it and run again. Sadly, you have to restart it if
    you want it to run (the automaton).

    4. In GUI the transitions between states are done each after one second for comprehension purposes,
    but the timestamps are of real time.

    5. Helpful output is printed to terminal in case of debuging need.

    

