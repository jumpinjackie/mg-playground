@echo off
SET CC=emcc
rem %CC% -DEMSCRIPTEN --bind -I. -IEmscripten -IMdfModel -IStylization -o mgstylization.js MgStylization.cpp
%CC% -DEMSCRIPTEN -I. -IEmscripten -IMdfModel -IStylization -o mgstylization.js MgStylization.cpp