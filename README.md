# HTTPServer

A simple HTTP server written in C with the least amount of libs (event system libs) as a challenge to myself.

Made in one week, it has no tests because I didn't have time to write them.

## Installation

To install the server, you need to clone the repository and compile the source code.

I use CMake & Ninja to compile the source code, so you need to have it installed or create a Makefile yourself.

## Usage

To start the server run the following command:

```bash
<executable> <port> <content-folder>
```

Example : 

```bash
./server 8080 /home/user/Downloads
```

## Contributing

Pull requests are welcome. 

For major changes, please open an issue first to discuss what you would like to change.

## Contact me

Email: altair61.dev@gmail.com

Discord : altakxs