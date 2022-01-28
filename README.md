
A set of classes so to handle csv-like content, but split into chunks that one can edit and rearrange easily in different files.

It is also expected that some strings will be reused rather often, and should be replaced with an index in a global table in memory, so to fasten strings comparison.

# Requirements

## Compiling

It should work with any compiler supporting C++20.

## Development and testing

This project is developed with the help of Docker and [this recipe](https://github.com/chavid/MyDevTools/blob/main/DevCpp20/Dockerfile). Running the provided tests rely on the [oval script](https://github.com/chavid/MyDevTools/blob/main/bin/oval.py) .

# Conventions

- `*.h` : header file
- `*.cc` : body file
- `*.cpp` : test file

# Few design principles

I use strongly typed integers and arrays, so to ensure that I never mix an index
with the wrong array. 
