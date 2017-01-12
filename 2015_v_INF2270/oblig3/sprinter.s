.extern add_int
.extern my_ftoa

.globl sprinter
.globl _sprinter

#Name: sprinter
#C sign.: int sprinter (unsigned char *res, unsigned char *format,...);
#Registers:
#eax: argument
#ebx: start of the position of the string to save the result
#ecx: format
#edi: length
#(esi, edx for temporary savings)


sprinter:
_sprinter:
    pushl   %ebp                #Standard
    movl    %esp, %ebp          #funksjonsstart

    movl    8(%ebp), %ebx       #storage
    movl    12(%ebp), %ecx      #format
    leal    16(%ebp), %eax      #next arg (potential garbage)
    movl    $0, %edi            #init. counter

loop:
    cmpb    $0, 0(%ecx)         #checks for 0-byte
    je      exit_loop           #exit if 0-byte found
    movb    0(%ecx), %dl        #1. char from format to edx (temporary)
    cmpb    $37, %dl            #check if % (37 = decimal ascii for '%')
    je      format              #if true: jump to format
loop_back:
    movb    %dl, 0(%ebx)        #edx to 1. char in storage
    addl    $1, %ecx            #format++
    addl    $1, %ebx            #storage++
    addl    $1, %edi            #counter++    
    jmp     loop

exit_loop:
    movb    $0, %dl             #0-byte temp. in edx
    movb    %dl, (%ebx)         #0-byte to storage
    movl    %edi, %eax          #return value to eax 
    popl    %ebp                #Standard
    ret                         #retur.

format:
    addl    $1, %ecx            #format++
    movb    0(%ecx), %dl        #next char from format to temp. edx 
    cmpb    $37, %dl            #if char == %
    je      percent_true
    cmpb    $99, %dl            #if char == c 
    je      char_true           
    cmpb    $115, %dl           #if char == s
    je      string_true
    cmpb    $100, %dl           #if char == d
    je      decimal_true
    cmpb    $120, %dl           #if char == x
    je      hex_true
    cmpb    $102, %dl           #if char == f
    je      float_true
    jmp     loop_back           #exit

percent_true:
    jmp     loop_back           

char_true: 
    movb    (%eax), %dl         #next char temp. to edx
    addl    $4, %eax            #next arg
    movb    %dl, (%ebx)         #char from format to storage
    addl    $1, %edi            #counter++
    addl    $1, %ecx            #format++
    addl    $1, %ebx            #storage++
    jmp     loop               

string_true:
    addl    $1, %ecx            #format++
    movl    (%eax), %esi        #de-referencing to arg-string
    addl    $4, %eax            #next arg
string_loop:
    cmpb    $0, 0(%esi)         #check for 0-byte
    je      loop                #exit if found
    movb    (%esi), %dl         #char by char to storage
    movb    %dl, (%ebx)         #via temp. edx
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    addl    $1, %esi            #arg++
    jmp     string_loop


decimal_true: 
    movl    (%eax), %edx
    addl    $4, %eax            #next arg
    
                                #.if %edx < 0, since add_int function can only handle numbers >= 0
                                #we have to add a '-' in the string and call add_int(-%edx)
    test    %edx, %edx
    jns     decimal_start

    movb    $45, (%ebx)         #a negative sign
    addl    $1, %ebx
    addl    $1, %edi
    
    cmpl    $-2147483648, %edx  #special case %edx == -2147483648
                                #.if it is negative it (2147483648) cannot be stored in the integer
                                #thus do it explicitely
    jne     negative
                                #otherwise, copy the string into it directly
    movb    $50, (%ebx)         # 2
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $49, (%ebx)         # 1
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $52, (%ebx)         # 4
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $55, (%ebx)         # 7
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $52, (%ebx)         # 4
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $56, (%ebx)         # 8
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $51, (%ebx)         # 3
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $54, (%ebx)         # 6
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $52, (%ebx)         # 4
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    movb    $56, (%ebx)         # 8
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++

    addl    $1, %ecx            #format++
    jmp     loop
    
negative:
    neg     %edx            

decimal_start:
    pushl   %eax                #save caller-saved registers
    pushl   %ecx
    pushl   %ebx                #allocate space on the stack for the first parameter
    movl    %esp, %eax
    pushl   %edx                #push parameters onto the stack
    pushl   %eax
    call    add_int             
    addl    $8, %esp

    popl    %eax
    subl    %ebx, %eax
    addl    %eax, %ebx
    addl    %eax, %edi

    popl    %ecx                #pop the saved registers
    popl    %eax

    addl    $1, %ecx            #format++
    jmp     loop

float_true:
    pushl   %ecx
    movl    (%eax), %edx
    movl    4(%eax), %ecx
    addl    $8, %eax
    pushl   %eax                #save caller-saved registers
    pushl   %ecx                #push parameters onto the stack
    pushl   %edx
    pushl   %ebx
    call    my_ftoa
    popl    %eax
    addl    $8, %esp
    subl    %ebx, %eax          #increase start of the string and length of the string
    addl    %eax, %ebx
    addl    %eax, %edi
    popl    %eax                #pop saved registers
    popl    %ecx
    addl    $1, %ecx            #format++
    jmp     loop

hex_true:
    movl    (%eax), %edx        
    addl    $4, %eax            #next arg
    pushl   %eax                #save caller-saved registers
    pushl   %ecx                #push parameters onto stack
    movl    %esp, %eax          #use stack to store the intermediate characters when converting
    cmpl    $0, %edx            #run algorithm to convert positive integers (0) into a hex
    jne     convert_loop
    pushl   $0                  #single 0
    jmp     convert_done

convert_loop:
    cmpl    $0, %edx
    je      convert_done
    movl    %edx, %ecx
    andl    $15,  %ecx          #get last 4 bits
    pushl   %ecx
    shrl    $4, %edx            #shift right operation
    jmp     convert_loop

convert_done:                   #pop intermediate character, save into result string
copy_loop:
    cmpl    %esp, %eax
    je      copy_done
    popl    %edx
    cmpl    $9, %edx
    jg      ten_more
    #the digit is between 0 and 9, save it directly
    addl    $48, %edx
    jmp     save_digit

ten_more:
    #convert 10 to 'a', 11, to 'b',...
    subl    $10, %edx
    addl    $97, %edx

save_digit:
    movb    %dl, (%ebx)
    addl    $1, %edi            #counter++
    addl    $1, %ebx            #storage++
    jmp     copy_loop

copy_done:
    popl    %ecx
    popl    %eax
    addl    $1, %ecx            #fomat++
    jmp     loop


