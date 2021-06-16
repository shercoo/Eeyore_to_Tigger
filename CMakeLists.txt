cmake_minimum_required(VERSION 3.19)
project(Eeyore_to_Tigger)

set(CMAKE_CXX_STANDARD 17)

add_executable(Eeyore_to_Tigger Eeyore.cpp Enode.h EsymTable.h lex.yy.cpp Eeyore.tab.cpp Eeyore.tab.hpp)