
cmake_minimum_required(VERSION 3.6)
find_package(PkgConfig REQUIRED)

pkg_check_modules(libphptrace REQUIRED IMPORTED_TARGET libphptrace)
pkg_check_modules(ncurses REQUIRED IMPORTED_TARGET ncurses)

add_executable(demo php_trace_demo.c)
target_link_libraries(demo PUBLIC PkgConfig::libphptrace PkgConfig::ncurses)
