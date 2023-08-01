
A set of classes so to handle csv-like content, but split into chunks that one can edit and rearrange easily in different files.

It is also expected that some strings will be reused rather often, and should be replaced with an index in a global table in memory, so to fasten strings comparison.

# Requirements

## Compiling

It should work with any compiler supporting C++20. For the time being, use `build.gmake` and `gmake`. The `cmake`flavor is under work.

# Users tips and tricks

## ChunksFile

When using `ChunksFile::read_columns_order`:
- you are warned when reading a file which as unexpected columns, which are ignored.
- you are warned when reading a file which which lack some columns, in which case when you read the field your variable will stay untouched and keep its previous value, considered as the *default*.

# Development tips and tricks

## Testing

This project is developed with the help of Docker and [this recipe](https://github.com/chavid/MyDevTools/blob/main/DevCpp20/Dockerfile). Running the provided tests rely on the [oval script](https://github.com/chavid/MyDevTools/blob/main/bin/oval.py) .

## Naming rules

- `*.h` : header file
- `*.cc` : body file
- `*.cpp` : test file

## Few design principles

- I use strongly typed integers and arrays, so to ensure that I never combine an index with the wrong array. 
