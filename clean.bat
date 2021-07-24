@echo off
del CMakeCache.txt cmake_install.cmake build.ninja .ninja_log .ninja_deps
rmdir /S /Q bin CMakeFiles