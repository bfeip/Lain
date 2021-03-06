# Lain
A compiler for a language I made up called Lain. It is currently almost completely broken. It's just here so I can move it around and stuff.

The language at the moment is something like C with a few missing features: classes currently don't work, arrays currently don't work, expressions do not follow the order of operations, certain operations haven't been implemented yet, and there are many other problems currently. However this is still in early development, and things have been moving pretty quickly so far so I expect that soon enough we will have a language in league with other C like languages.

I don't have any really good documentation on how the language works right now. the samples directory contains a couple of examples of what the lain language looks like.

## Planned Language Features
* Garbage collection
* C++ style classes
* Class and function templates
* Arbitrary declaration/definition order
* Embedded function and class declarations
* Replacement of many pre-processor features with language features
* Useful built-in data types, much like Python
* And many more

## Building
Haha, good luck...

The build system currently sucks and I'm sorry, but I'm just learning cmake so bear with me. This has only been done on Linux but I have no reason to believe that it won't work on Mac as well. Windows users, count yourselves lucky that you are spared from this hell.

You'll need the llvm source. If you didn't clone recursively you'll have to update the submodules.

```
git submodule update
```

In the Lain source directory make the build directory to build Lain in.

```
mkdir build
cd build
```

Make a directory to build llvm in.

```
mkdir llvm
cd llvm
```

Build llvm (use multithreading and go and have lunch or something because this takes a while)

```
cmake ../../third_party/llvm/
make -j4
```

Go back up to the top of the build directory and build Lain

```
cd ..
cmake ..
make
```
I know this system sucks. If you really really hate it maybe you would consider implementing a better system. I'm lazy and bad with build systems, so I'd appreciate it.

## Making Executables

I have included a handy dandy script at the top of the source directory that **should** be helpful.

While in the top of the source directory, and with Lain build in the build directory as it should be after following the steps in the *Building* section of this readme. Simply run the script like this:

```
python2 ./lain2bin.py path/to/your/file.lain
```
You **should** then have and executable in your build folder named a.out that can be run like you would run any other program
Since there is currently no way to print values from your program nor a way to easily debug it you should observe the return value with 'echo $?'. After you get everything working you should ask yourself why you wasted your time making an executable in a language that can't do much at the moment.