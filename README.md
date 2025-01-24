
### Introduction

This program is a simple real time ray tracer. This project started as a regular ray tracer for a grade, but was expanded into a real time ray tracer both for fun and for the challenge. The entire backend is built in C, while the front end is built in Java.

This programs works by running several threads in the background, constantly generating pixels for each frame. The frames are then sent to the front end, which displays them on the screen.

This program is far from finished, but is fully functional. As such, there are some features yet to be implemented.

### Java?

Originally, I planned to have the entire program in C, using Linux system calls. Threads would write to a double buffer, which in turn would be written directly to the screen. However, due to borrowing someone else's Linux machine, I was unable to gain permission to access the frame buffer.

Instead, Java is used to provide an image to the screen. It had the following consequences:
- Java does not allow pipes, so the two programs have to be completely separate
- Java doesn't allow signals from other processes
- Java does not allow sharing memory maps with other processes (however C doesn't mind, so it can hijack Java's memory maps)

Although this makes it difficult, it was an exciting challenge. Several design choices were made based on these restrictions.

### Usage

There are essentially 3 programs required to run the ray tracer.
- Safety: Creates all memory maps and prepares communication between the backend and frontend
- Main.class (Java): Runs the front end and accepts user input
- Raytracer: Traces frames, manages threads, and keeps the front end timed correctly

To run the program, do the following:
1. Compile by using 'make' with the given Makefile
2. Run the safety program as follows. It will run to completion.
```
./safety
 ```
3. Run the Java front end. This is dependent on the user's JDK. The Java will print out "Ready for Backend".
4. Once the front end is ready, start the backend:
```
./raytracer
 ```

Once both pieces are up, you can interact with the program by using the following controls:
- W: Forward
- S: Backward
- A: Strafe Left
- D: Strafe Right
- Q: Turn Left
- E: Turn Right

### Cautions

- Beware: This program is not for small computers. C uses 5 total threads (1 main, 4 generators), and Java uses 2 (around 2, between the keyListener and the main thread and the JVM).
- Failing to run the safety can cause flicker, errors, or crashing. This is because the shared memory was not correctly cleared. Simply stop both the front and back ends, run safety, and restart the process.

### Future Work

- State Machine to standardize frame rate
- Test the speed of sockets, to see if that would work better/faster than shared memory (seems unlikely, since we're caching significant amounts of data, and accessing it directly. However, it might work better and be more efficient for syncing between C and Java.
- The cubes currently don't do anything, other than fly around. A good next step would be adding collisions and making them do something
- Reflections could be added back in. They were removed not because they were too complex or expensive, but because I wasn't sure where to put them, without making the user sick (although that ... sometimes still happens)
- A few optimizations were cut for the sake of development time, but could be put back in
- General code clean up and maintenance
- Spheres are implemented, but there are none

Please feel free to email me with questions, feedback, bugs, or concerns.
devinfrandsen2@gmail.com

###### Created by Devin Frandsen; April 2024
