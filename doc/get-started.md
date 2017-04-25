# Get Started



## Get Source Code

```shell
git clone --recursive https://github.com/morrow1nd/luax.git
```

Clone our git repository, or download the [latest code](https://github.com/morrow1nd/luax/archive/master.zip). If you download the latest code, you must download luax's dependences manually. Luax uses only one extra lib [uthash](https://github.com/troydhanson/uthash/archive/master.zip), download it, rename it's name to `hash` and place it in path `luax-master/src/hash`.



## Compile the Code


### Windows User

1. Use [Cmake](https://cmake.org/download/) to generate Visual Studio project.

 Open `path/to/cmake_install_dir/bin/cmake-gui.exe`. Click the `Browse Source...` button and select the path of luax.(For example: `D:/luax/`). 

 Next step, fill in `Where to build the binaries` with the path of luax's subdir `build`.(For example: `D:/luax/build/`). 

 Final, click `Configure` and `Generate` one by one. The VS project is in `///luax/build/luax.sln`.

2. Open VS project and compile it


### Linux User

```shell
cd path/to/luax/
mkdir build
cd build/
cmake ..
make && make test
```



## Run Luax Code

 After building the project, you will get a command line program - luax_exec.

```
$ luax_exec.exe -h
usage: luax_exec.exe [option] [script]
Available options are:
  -o path        specify output file path
  -c             complie luax code to opcode
  -r             run opcode
  -h --help      show help info
  --version      version info

$ luax_exec.exe app.luax  # run app.luax
$ luax_exec.exe -c -o opcode.luaxo app.luax  # compile luax code to opcode
```
