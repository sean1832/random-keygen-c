# Random Key generator C

This is a simple random key generator written in C. It generates a random key of a given length and prints it to the console.
It is a remake of the [C# version](https://github.com/sean1832/random-keygen) for smaller executable file size and faster execution.

## Usage
```shell
randkey <length> [options]
```

## Options
- `-s` or `--seed`: Set the seed for the random number generator
- `-t` or `--type`: Set the type of the key (default: `b64`)
  - `b64`: Base64
  - `hex`: Hexadecimal
- `-h` or `--help`: Show help message
- `-v` or `--version`: Show version

