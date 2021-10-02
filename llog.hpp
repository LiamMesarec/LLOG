#pragma once

#if __cplusplus < 202002L
  #error LLOG ERROR: C++20 Required
#endif

#include <string>
#include <string_view>
#include <concepts>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <source_location>

#if defined(_WIN32) && defined(LLOG_COLORS_ENABLED)
    #include "windows.h"
    #define LLOG_COLOR_WINDOWS
#endif

namespace llog
{
    template<typename T, typename ... U>
    concept IsAnyOf = (std::same_as<T, U> || ...);

    template<typename T>
    concept Iterable = std::forward_iterator<typename T::const_iterator>;

    template<typename T>
    concept Numeric = std::floating_point<T> || std::integral<T>;

    template<typename T>
    concept String = IsAnyOf<std::remove_cvref_t<std::remove_pointer_t<std::decay_t<T>>>, char, wchar_t, std::string, std::string_view>;

    template<typename T>
    concept Array = std::is_array_v<std::remove_reference_t<T>>;

    template<typename T>
    concept CanBeCout = requires (T x) { std::cout << x; };

    template<typename T>
    concept Printable = CanBeCout<T> && (!Array<T> || String<T>);

    template<typename T>
    concept NotString = !String<T>;

    template<typename T>
    concept Container = (Iterable<T> || Array<T>) && NotString<T>;

    enum class Color
    {
        #ifdef LLOG_COLOR_WINDOWS
            BLACK = 0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE, GREY, LIGHT_BLUE, 
            LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, HIGH_INTENSITY_WHITE, DEFAULT      
        #endif

        #if !defined(LLOG_COLOR_WINDOWS)
            BLACK = 30, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, HIGH_INTENSITY_WHITE, DEFAULT 
        #endif
    };

    struct PrintTemplate
    {
        const char* start;
        const char* delimiter;
        const char* end;
        Color color = Color::HIGH_INTENSITY_WHITE;   
    };

    namespace 
    {
        PrintTemplate m_printTemplate
        {
            .start = "",
            .delimiter = " ",
            .end = "\n",
        };

        PrintTemplate m_arrayTemplate
        {
            .start = "",
            .delimiter = "\n",
            .end = "\n",
        };

        #ifdef LLOG_ENABLED
            #if defined(LLOG_COLOR_WINDOWS) && defined(LLOG_COLORS_ENABLED)
                const HANDLE m_WindowsConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            #endif

            Color m_defaultColor = Color::HIGH_INTENSITY_WHITE;
            void SetColor([[maybe_unused]]Color color = m_defaultColor)
            {
                #if defined(LLOG_COLOR_WINDOWS) && defined(LLOG_COLORS_ENABLED)
                    SetConsoleTextAttribute(m_WindowsConsoleHandle, static_cast<int>(color));
                #endif

                #if !defined(LLOG_COLOR_WINDOWS) && defined(LLOG_COLORS_ENABLED)
                    std::cout << "\033[" << static_cast<int>(color) << "m";
                #endif
            }
            bool firstArg = false;
        #endif
    }
    
    void Print([[maybe_unused]]const PrintTemplate& pt, [[maybe_unused]]const Container auto& arr)
    {
        #ifdef LLOG_ENABLED
            for(auto&& x : arr)
            {
                std::cout << x << ' ';
            }
            std::cout << pt.delimiter;    
        #endif 
    }

    void Print([[maybe_unused]]const PrintTemplate& pt, [[maybe_unused]]const Container auto& arg, [[maybe_unused]]const Container auto&... args)
    {
        #ifdef LLOG_ENABLED
            SetColor(pt.color);

            if(firstArg == false)
            {
                firstArg = true;
                std::cout << pt.start;
            }
    
            if constexpr(sizeof...(args) > 0)
            {
                Print(pt, arg);
                Print(pt, args...);
            }

            if constexpr(sizeof...(args) == 1)
            {
                std::cout << pt.end;
                firstArg = false;
            }

            SetColor();
        #endif
    }

    void Print(const Container auto& arg, const Container auto&... args)
    {
        Print(m_arrayTemplate, arg, args...);
    }

    void Print([[maybe_unused]]const PrintTemplate& pt, [[maybe_unused]]Printable auto&& arg, [[maybe_unused]]Printable auto&&... args)
    {
        #ifdef LLOG_ENABLED
            SetColor(pt.color);

            std::cout << pt.start << std::forward<decltype(arg)>(arg);
            ((std::cout << pt.delimiter << std::forward<decltype(args)>(args)), ...);
            std::cout << pt.end;

            SetColor();
        #endif
    }

    void Print(Printable auto&& arg, Printable auto&&... args)
    {
        Print(m_printTemplate, arg, args...);
    }

    void PrintToFile([[maybe_unused]]std::ofstream& ofs, [[maybe_unused]]const PrintTemplate& pt, [[maybe_unused]]Printable auto&& arg, [[maybe_unused]]Printable  auto&&... args)
    {
        #ifdef LLOG_ENABLED
            ofs << pt.start << std::forward<decltype(arg)>(arg);
            ((ofs << pt.delimiter << std::forward<decltype(args)>(args)), ...);
            ofs << pt.end;
        #endif
    }

    void PrintToFile(std::ofstream& ofs, Printable auto&& arg, Printable auto&&... args)
    {
        PrintToFile(ofs, m_printTemplate, arg, args...);
    }

    void PrintToFile(const std::filesystem::path& path, Printable auto&& arg, Printable auto&&... args)
    {
        std::ofstream ofs(path);
        PrintToFile(ofs, m_printTemplate, arg, args...);
    }

    void PrintFromFile([[maybe_unused]]std::ifstream& ifs)
    {
        #ifdef LLOG_ENABLED
            if(ifs.is_open()) 
            {
                std::cout << ifs.rdbuf();
            }
        #endif
    }

    void PrintFromFile(const std::filesystem::path& path)
    {
        std::ifstream ifs(path);
        PrintFromFile(ifs);
    }

    [[nodiscard]]std::string Location(const std::source_location& location = std::source_location::current())
    {
        return "[FILE: " 
                + std::string(location.file_name()) 
                + " LINE: " 
                + std::to_string(location.line())  
                + "] ";
    }

    namespace pt
    {
        PrintTemplate error 
        {
            .start = "Error: ",
            .delimiter = " ",
            .end = "\n",
            .color = Color::RED
        };

        PrintTemplate warning 
        {
            .start = "Warning: ",
            .delimiter = " ",
            .end = "\n",
            .color = Color::YELLOW
        };

        PrintTemplate message 
        {
            .start = "Message: ",
            .delimiter = " ",
            .end = "\n",
            .color = Color::GREEN
        };      
    }
}
