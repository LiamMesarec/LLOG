# LLOG
A C++20 std::cout wrapper.

LLOG is a header-only simple to use library that provides function wrappers for std::cout. Also adds colors on Windows and Linux (and other systems that support ANSI Escape Codes).

EXAMPLE USAGE: 

```cpp
#define PRINT_ENABLED
#define PRINT_COLORS_ENABLED
#include "llog.hpp"

int main()
{
    int array[10] {2,2,3,2,32,3,34,1,2,3};

    llog::Print(llog::pt::error, "error at location: ", llog::Location());
    llog::Print(llog::pt::warning, "warning!", 123, 123.5, "test!");
    llog::Print(llog::pt::message, "message: ", "hi");

    llog::Print(array);
    llog::Print(
        {"\nSTART ARRAY PRINTER \n", "\n", "END ARRAY PRINTER\n\n", llog::Color::BLUE}, 
        array, array, array
    );

    llog::Print("the end");

    llog::Print(
        {"", "      ", "\n", llog::Color::RED}, 
        1, 2, 3
    );

    llog::Print("the end");
}
```