Project name: FSMCraft
Description: Tool for visual editing, interpreting and monitoring flow of finite automata
Authors: Mikuláš Lešiga xlesigm00
         Alex Marinica xmarina00
         Filip Novák xnovakf00

GUI:
 - Alex pridaj co vie spravit GUI vsetko

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
        fsm.output(nameO, valueO) -> sets the output nameO to value valueO
        fsm.defined(name) -> returns boolean whether such variable/input had been defined
        fsm.elapsed() -> returns time in ms which elapsed in current state

Interpetation of the run:
    A subclass of QStateMachine was implemented to handle the logic.
    Custom transitions and state actions perform the expected behaviour.

Message protocol:
    For sending TCP messages to communicate between the server