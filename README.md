# snOS
### Sensor Node Operating System v0.1

## What Is snOS?
snOS is a multi-threaded events and service framework with IoT in mind for embedded systems.
I created snOS because I found myself constantly writing the boilerplate for IoT systems
for several projects to do the same thing over and over. I decided to write a decent version 
with more connectivity features. This project will develop and progress along with my other 
projects using it. 

## Why snOS?
snOS is intended to run on the barest of the bare microcontroller environments and architectures. 
snOS is optimized for having low-powered bare-metal microcontrollers communicate in a network by 
sending packets of data to and from eachother. These packets have the ability to interrupt and 
raise event flags and trigger thread executions on a remote system also running snOS. 

## How to use snOS
Setting up snOS for you environment is as simple as writing a couple of port subroutines.
### Setup Overview
<dl>
<dt>Layer 1: The Physical Layer</dt>
<pre>
    |--------|                           |--------|
    |   uC   |<----/SERIAL HARDWARE/---->|   uC   |                           
    |--------|             ^             |--------|
                           |
                           |------------> ...other controllers
</pre>

In this layer of setup the controllers are connected via a serial 
(UART/USART at configurable baudrate) connection (wireless or wired).
This can be as simple as connecting the common ground, RX, and TX pins
together on a network. In the wired case, electrical considerations like 
line impedance must be considered by the user. Consider switching to a wireless
setup like the XBee or BTLE, which can both be used on serial UART line.
snOS can also be used in a singular mode where it is the only present node
in a network. This mode is useful for having the controller run a simple
multi-threaded soft-timed operating system for simple data aggregation and 
everyday microcomputer tasks.

<dt>Layer 2: The Bare Metal Bootloader</dt>

Required:
snOS doesn't require much setup from the user, but a couple of port functions must
be written before hand. 
-- todo: write timer documentation --

Optional:
For the optional networking capabilities of snOS, a serial driver is required.
A non-interrupt or interrupt driven UART/USART driver must be 
written and tested to be working on that architecture before loading on snOS.
The required functions created to communicate over serial must include:

  1. A single byte receiving function with the following prototype:
  `uint8_t receive_byte(void);`
  2. A signle byte transmit function with the following prototype:
  `void send_byte(uint8_t byte);`
  
snOS thrives in a mult-sensor network environment. These sensors must of course
be configured to work for a given device. The drivers for these sensors must be written,
tested, and confirmed to be working in a bare-metal single-threaded control-loop environment 
before loading snOS onto the board.

<dt>Layer 3: The snOS Layer</dt>

Once the initializing bootloading functions that load the system drivers 
(interrupts, serial, and sensors) are called, a sequence of snOS calls 
must be executed in the following order:

  1. `snos_initialize()` -- initializes the data structures used by snOS
  2. `snOSError snos_new_task(snOSError (*task_handler)(void), snOSTaskRunType process_type)` -- creates a new "task"
     A task is simply a thread entry function. Tasks should be small, not really on hard timing, use static memory to
     retain any data that must be preserved. The prototype of a task is as follows:
     `snOSError task_handler_name(void);`
     
     The process_type parameter allows you to either have the task:
     1. `RUN_FOREVER` - runs the task as often as possible over and over in the main control loop of the processor
     2. `RUN_ONCE` - run the task just once; probably an initialization. Note: this disallows a packet to request this task
     3. `RUN_ON_REQUEST` - only runs the task when the task is requested either from an external process (received packet),
                         or by another thread task.
     
     The number of tasks on a system is only limited by the memory of the microcontroller snOS is running on, so
     repeat this step as desired.
     
   3. (optional -- required if using snOS Connect - the sensor node component)
      From within a task handler function definition, call:
      `snos_connect_initialize_channel(snOSTask *handler,uint8_t (*packet_byte_receiver)(void),void (*packet_byte_transmitter)(uint8_t));`
      handler - the task handler pointer returned by `snos_task_id()`
      packet_byte_receiver/packet_byte_transmitter - a pointer to the serial byte receive/transmit function defined in Layer 2

      Then call `snos_connect_start()`
      
   4. After setting up the snOS layer, simply call: `snos_start()`
      The system will now begin snOS and remain in this mode until reset or externally exited.
      
</dl>
      
## Current Setup

I have successfully used snOS on:

    Atmel AVR Atmega 328P - I will provide the complete port soon in this repository
      
## Future Development and TODO
    - (in progress) test multi-node network with several threads and test collisions
    - test on the MEGA
    - test on a PIC32
    - time thread switching
    - time packet trigger switching
    - measure memory usage in RAM/ROM
    - (in development) create an embedded linux client that can aggregate snOS network data to push to the cloud
## Contributing workflow

If you'd like to contribuate a change to this project:

1. [Fork this project][fork] to your account.
2. [Create a branch][branch] for the change you intend to make.
3. Make your changes to your fork.
4. [Send a pull request][pr] from your fork’s branch to our `master` branch.

[fork]: https://help.github.com/articles/fork-a-repo/
[branch]: https://help.github.com/articles/creating-and-deleting-branches-within-your-repository
[pr]: https://help.github.com/articles/using-pull-requests/

## License
[GNU GPLv3](./LICENSE).

## Author
<dl>
  <dt>snOS was created by Sargis S Yonan in 2017</dt>
</dl>
