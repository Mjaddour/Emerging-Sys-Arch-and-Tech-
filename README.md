# Emerging Sys Arch and Tech

# Summarize the project and what problem it was solving.
The project goal was to develop an embedded system prototype for a thermostat. It utilizes the TMP006 temperature sensor via I2C to read the room temperature. LED indicators are employed to display the thermostat's output, indicating heat status through GPIO. Additionally, two buttons enable users to increase and decrease the set temperature, utilizing GPIO interrupts. Finally, UART is used to simulate data transmission to the server over Wi-Fi.

# What did you do particularly well?
Effectively structured the main thread of the program, separating different tasks into functions, which makes the code modular and easier to understand.
Comments throughout the code helps to explain the purpose of each function and section, aiding readability.
Employed dynamic memory allocation (malloc) appropriately for allocating memory for TemperatureSetup and InterruptSetup structs.

# Where could you improve?
Error handling: Add more robust error handling mechanisms, especially for functions like malloc, UART2_open, I2C_open, and Timer_open, where failure could occur.
Adding more descriptive comments within functions to clarify complex operations or algorithms, making the code even more readable.
Ensure proper cleanup of resources allocated dynamically using free to avoid memory leaks.

# What tools and/or resources are you adding to your support network?
Incorporating additional error handling techniques, such as logging errors to a file or displaying them on a console, would enhance debugging capabilities.
Utilizing static code analysis tools and linters can help enforce coding standards, identify potential bugs, and improve code quality.

# What skills from this project will be particularly transferable to other projects and/or course work?
Breaking down complex tasks into smaller, manageable functions/modules.
Experience with initializing and using peripherals like UART, I2C, GPIO, and Timers, which are common in embedded systems development.
Understanding how to schedule and coordinate tasks in real-time systems, which is applicable in various embedded and multitasking environments.

# How did you make this project maintainable, readable, and adaptable?
By organizing code into separate functions for each task, you've made it easier to maintain and modify individual components without affecting others.
Variable and function names are clear and descriptive, contributing to readability.
Configuration options (such as GPIO pins and timer settings) are abstracted into a separate configuration file (ti_drivers_config.h), facilitating easy adaptation to different hardware setups.
Extensive use of comments and clear documentation within the codebase aids in understanding its functionality, making it more adaptable to future modifications.

