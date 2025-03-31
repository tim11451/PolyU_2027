For this homework, the method I applied is shown below:
For binary conversion, we should use a loop to traverse every bit of the user input number. After each iteration, we print the reminder on the screen.  
In MIPS, we can realize this by rightshifting bit, divide by 2, get the reminder, then print. First, assume the number is 32 bits (actually, it is, when considering about MIPS situation), if at first you right shift the 31 bits of the number, then divide the rest number by 2, you can get the first bit of the number. Then shift by 30 bits, you can get the second bit. (Because the bits beyond the first, when divide that by 2, can must be divided evenly)
As we can find the regulation from the hexdecimal number: 4 bits (binary) can be converted to one bit (hex), then for quaternary, 2 bits can be converted to one bit. Octal: 3 bits can be converted to one bit.
For quuaternary, we change the base number to 4. For octal, we change the base number to 8. 
