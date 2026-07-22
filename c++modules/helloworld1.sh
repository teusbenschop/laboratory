#!/usr/bin/env bash


echo Building gcm.cache/std.gcm for namespace std once
g++-15 -std=c++23 -O3 -fmodules -fsearch-include-path bits/std.cc helloworld1.cpp

# -fsearch-include-path :
# Search for primary input/source files inside the standard #include paths,
# rather than just searching the current working directory.
# It finds library modules (like bits/std.cc files alongside system headers.

# This command creates a folder with the CMI for std,
# located in file ./gcm.cache/std.gcm (around 29MB) in the local directory.

echo Compiling helloworld again
g++-15 -std=c++23 -O3 -fmodules helloworld1.cpp -o helloworld

# It looks for the gcm.cache folder.
# Storing std.gcm elsewhere via a module.mapper file, one module per line, e.g.
#   std  gcm.cache/std.gcm
# Then pass this flag:
#   -fmodule-mapper=module.mapper

echo Ready
