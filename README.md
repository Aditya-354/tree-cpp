# Tree CLI tool (c++ learning project)

A simple CLI tool that mimics the Linux 'tree' command using modern c++ features like std::filesystem and std::visit, etc.

## Acknowledgements

This project has been built by following a tutorial from [https://youtu.be/Wrstsv67_Zo].

I implemented the code myself while learning concepts like:
- std::optional
- std::visit
- std::variant
- std::filesystem
- std::span
- recursion
- directory traversal

## Usage:

tree \[-L / --path=\]\
-L INTEGER,\
                specifies the depth of branches to display in the output.\
--path=/path/to/dir,\
                specifies the path of the directory to display.
