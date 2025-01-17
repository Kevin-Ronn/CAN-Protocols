# CAN-Protocols

Av: Kevin Rönn

Syfte:

Simulate a motor controller embedded board,
that sends a message every second over queue or queue set
to 2 other simulations (ventilation and fuel) asking if all ok or
there are errors and collect info from them. This controller
does self-check to motor and gearbox on the same
frequency (every second), and it is also responsible to write
info on the shared dashboard (here will be Arduino Serial
monitor).



Tasks:

Motor Controller:
Sends a message every second to the ventilation and fuel, asking for status.
Performs self-check on the motor and gearbox and logs results.
Writes outputs to the dashboard (Serial Monitor).

Ventilation System:
Responds to the motor controller's queries.
Writes its status to the dashboard.

Fuel System:
Responds to the motor controller's queries.
Logs fuel status to the dashboard.

Inter-Task Communication:
Uses a Queue Set for handling message between tasks.
