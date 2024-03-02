# Websorter
Minimalistic web socket server in C++ that accepts requests for sorting data and responds with the corresponding result by applying parallel quicksort on the data. The server and client are written using the **UNIX** socket system calls. The server is using the `select` system call for multiplexing the client requests.

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
Both the server and client use the **UNIX** socket system calls to establish the connection. The server also uses the `select` system call for multiplexing the client requests instead of using `fork` for each requests, so that it does not overwhelm the system. The code is written according to the **C++ 20** standart so keep that in mind when building the project locally. **Strong exception guarantee** is impemented to disable sudden crashes of the server and the only time when an exception may shut down the server is when the server is unable to start due to network problems.

The server currently accepts 3 types of requests:
- `exit` - ends the connection between the client and the server;
- `help` - displays a message, showing all the available commands and their arguments;
- `quicksort [ARGS]...` - the arguments are the elements to be sorted. Elements are sorted via a parallel version of quicksort and currently they are sorted only lexicographically, because the sent data is interpreted only as text. The elements may be separated by any number of whitespace characters.

The parallel quicksort algorithm behaves the same was as the normal quicksort with the only difference being that, after the elements are partioned into two parts, if any of the parts contains more than `10 000` elements, then the recursive call for that part is done in a separate thread via `std::async`. For choosing the pivot there is no special algorithm - the pivot is chosen at **random**, which does not affect the time complexity of the algorithm. What is more, the partition is done with the `std::execution::par_unseq` directive which in some cases may lead to better performance due to the increased parallelism.

## License
The project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more information.

## Contacts
- GitHub: https://github.com/nigosto
- LinkedIn: https://www.linkedin.com/in/georgi-n-atanasov
- email: georgi.n.atanasov02@gmail.com