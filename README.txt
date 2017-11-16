$Id: README,v 1.1 2015-07-16 16:47:51-07 - - $
Student Name: Kenneth High
User ID: khigh
Email: khigh@ucsc.edu

gdraw is program which uses OpenGL to create simple 2-D graphics. It accepts instructions for what to draw either from stdin or through a text file included as an argument. Instructions can be used to define shapes of certain sizes under specific variable names, to draw those defined variables on the display in a specified color, or to create text messages in a specified font. You can also move a given shape in space on the display.

A text file with instructions is included as "test.txt" in the home directory.

mk-colors.perl generates the file colors.cppgen which is used to store a list of colors that are accessed when executing gdraw. colors.cppgen must exist for gdraw to complie.