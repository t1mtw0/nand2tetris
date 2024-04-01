// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

    @i
    M=0
(LOOP)
    @i
    D=M
    @8192
    D=D-A
    @LAST
    D;JEQ
    @KBD
    D=M
    @BLACKEN
    D;JNE
    @CLEAR
    D;JEQ
    @LOOP
    0;JMP
(CLEAR)
    @i
    D=M
    @8192
    D=D-A
    @LOOP
    D;JGE
    @SCREEN
    D=A
    @i
    A=D+M
    M=0
    @i
    M=M+1
    @LOOP
    0;JMP
(BLACKEN)
    @i
    D=M
    @8192
    D=D-A
    @LOOP
    D;JGE
    @SCREEN
    D=A
    @i
    A=D+M
    M=-1
    @i
    M=M+1
    @LOOP
    0;JMP
(LAST)
    @i
    M=0
    @LOOP
    0;JMP
(END)
    @END
    0;JMP