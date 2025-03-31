#For this homework, the method I applied is shown below:
#For binary conversion, we should use a loop to traverse every bit of the user input number. After each iteration, we print the reminder on the screen.  
#In MIPS, we can realize this by rightshifting bit, divide by 2, get the reminder, then print. First, assume the number is 32 bits (actually, it is, when considering about MIPS situation), if at first you right shift the 31 bits of the number, then divide the rest number by 2, you can get the first bit of the number. Then shift by 30 bits, you can get the second bit. (Because the bits beyond the first, when divide that by 2, can must be divided evenly)
#As we can find the regulation from the hexdecimal number: 4 bits (binary) can be converted to one bit (hex), then for quaternary, 2 bits can be converted to one bit. Octal: 3 bits can be converted to one bit.
#For quuaternary, we change the base number to 4. For octal, we change the base number to 8. 

#Here we define some strings which can be used in the next codes:
.data 
request_input:      .asciiz "Enter a number: "
display_input:      .asciiz "Input number is: "
binary:             .asciiz "\nBinary: "
quaternary:         .asciiz "\nQuaternary: "
octal:              .asciiz "\nOctal: "
if_continue:        .asciiz "\nContinue?(1=Yes/0=No) "    
farewell:           .asciiz "Bye!"
.text

main:  
    # Prompt for input
    li $v0, 4  # 4: print the string
    la $a0, request_input
    syscall

    # Read the number
    li $v0, 5  # 5: read the number
    syscall
    move $t0, $v0  # Store the user input number in $t0

    # Display the number
    li $v0, 4  
    la $a0, display_input
    syscall
    # load 32, which is the total times of iteration for binary, and total bits number of a number in MIPS number system. 
    li $t1, 32 
    move $a0, $t0
    li $v0, 1   
    syscall

    #Print the binary title
    li      $v0,    4
    la      $a0,    binary
    syscall
    # Jump to binary section
    j binary_section

binary_section:
    move $t2, $t0 # Move user input to t2
    li $t3, 2
    addiu $t1, $t1, -1 # Decrease the loop index 
    
    #divide the number by the base number
    srl $t4, $t2, $t1
    divu $t4, $t3
    # move high: move the value of t1's bit to the first bit
    mfhi $t5

    #Print the current bit of the number 
    move $a0, $t5
    li $v0, 1
    syscall
    
    bnez $t1, binary_section

    #To avoid printing many times in qua_section, we print the title here 
    la      $a0,    quaternary
    li      $v0,    4
    syscall

    # reload the bits number.
    li $t1, 32
    j qua_section
qua_section:
     
    # about the method to convert bianry number to quaternary number, I referenced https://blog.csdn.net/qq_18671415/article/details/114699804  
    move $t2, $t0
    li $t3, 4
    addiu $t1, $t1, -2

    #Divide the number by the base number
    srl $t4, $t2, $t1
    divu $t4, $t3
    mfhi $t5

    move $a0, $t5
    li $v0, 1
    syscall

    bnez $t1, qua_section
    la      $a0,    octal
    li      $v0,    4
    syscall 

    li $t1, 33 # Why 33? Because for octal, you have to move 3 bits every iteration to obtain a single bit.
    # But if set 32 bits, you can't get an integer-times loop. (Or we say, the last two bits can't be converted) 
    j octal_section  


octal_section:
    # load the initial parameters, the same reason to the previous codes. 
    move $t2, $t0
    li $t3, 8
    addiu $t1, $t1, -3 
    
    #Divide the number by the base number
    srl $t4, $t2, $t1
    divu $t4, $t3
    mfhi $t5

    #Print current bit
    move $a0, $t5
    li $v0, 1
    syscall 


    bnez $t1, octal_section
    la      $a0,    if_continue
    li      $v0,    4
    syscall

    j continue_or_not

continue_or_not:
    li      $v0,    5
    syscall
    beq $v0, 1, main

    la $a0, farewell
    li $v0, 4
    syscall
    j end_of_prog

end_of_prog:
    # Exit the program
    li $v0, 10  # 10: exit
    syscall