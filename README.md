# Running on Windows:

1. First clone the `allolib_playground` library [here](https://github.com/AlloSphere-Research-Group/allolib_playground).
Make sure you can use 
    ```
    ./run.sh path/to/file.cpp
    ```

    before proceeding.

2. Then clone this repository anywhere inside `allolib_playground/`

    In order for cmake to find the `Gimmel` library, add the following into the root `allolib_playground/CMakeLists.txt` file:
    ```
    file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/path/to/Gimmel-Allolib-Tests/Gimmel/include/*.hpp)
    target_include_directories(${this_app_name} PRIVATE ${HEADER_FILES})
    ```
    where `${CMAKE_CURRENT_SOURCE_DIR}` is `allolib_playground/`. It should be in the indented block for the `BUILD_FILE` macro definition, starting on line 72. (I placed mine on line 111 at the time of writing this but it could have changed since then)

Now you can just use
```
./run.sh path/to/Gimmel-Allolib-Tests/file.cpp
```