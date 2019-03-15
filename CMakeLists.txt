
cmake_minimum_required(VERSION 3.6)
find_package(PkgConfig REQUIRED)

pkg_check_modules(libphptrace REQUIRED IMPORTED_TARGET libphptrace)
pkg_check_modules(ncurses REQUIRED IMPORTED_TARGET ncurses)

add_executable(main php_trace_test.c)
target_link_libraries(main PUBLIC PkgConfig::libphptrace PkgConfig::ncurses)
