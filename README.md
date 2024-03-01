# Websorter
Minimalistic Web Socket Server in C++ that accepts requests for sorting data and responds with the corresponding result by applying parallel quicksort on the data. The server and client are written using the **UNIX** socket system calls. The server is using the `select` system call for multiplexing the client requests.

## Table of contents
- [Prerequisites](#prerequisites)
- [Running the project](#running-the-project)
- [Documentation](#documentation)
- [License](#license)
- [Contacts](#contacts)

## Prerequisites
- OS: any UNIX-like and POSIX-complient operating system
- Compiler: clang 14.0.0 or newer/gcc 11.4.0 or newer

## Running the project
By default the project is built using `clang`. If you want to use any other compiler, you need to change the [Makefile](./Makefile)

Building the whole project:
```shell
make 
```

Building only the server:
```shell
make server.exe
```

Building only the client:
```shell
make client.exe
```

Running the server:
```shell
./server.exe
```

Running the client:
```shell
./client.exe
```

Cleaning the directory:
```shell
make clean
```

## Documentation:
TODO

## License
The project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more information.

## Contacts
- GitHub: https://github.com/nigosto
- LinkedIn: https://www.linkedin.com/in/georgi-n-atanasov
- email: georgi.n.atanasov02@gmail.com