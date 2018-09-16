cmake_minimum_required(VERSION 3.5)

project(screenshoot)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

subdirs(src)

install(FILES screenshoot.desktop DESTINATION share/applications)