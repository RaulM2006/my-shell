# mysh

A simple custom shell implemented in C.

## Project Structure

mysh/
├── include/           # Header files (.h)
│   ├── my_shell.h
│   └── dynamic_list.h
├── src/               # Source files (.c)
│   ├── my_shell.c
│   └── dynamic_list.c
├── main.c             # Entry point
├── Makefile           # Build instructions
└── README.md          # This file

## Build

Make sure you have `gcc` installed.

$ make

This will compile the source files and produce the executable named `mysh`.

## Run

$ ./mysh

This starts the shell.

## Clean

To remove compiled objects and the executable:

$ make clean

## Contributing

Feel free to fork and open pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

