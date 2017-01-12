.extern add_int
.extern my_ftoa

.globl sprinter
.globl _sprinter

#Navn: sprinter
#C-sign.: int sprinter (unsigned char *res, unsigned char *format,...);
#Registere:


sprinter:
_sprinter:
    pushl   %ebp                #Standard
    movl    %esp, %ebp          #funksjonsstart

    movl    8(%ebp), %ebx       #storage
    movl    12(%ebp), %ecx      #format
    leal    16(%ebp), %eax      #neste arg (potensielt garbage)
    movl    $0, %edi            #init. teller

loop:
    cmpb    $0, 0(%ecx)         #sjekker for 0-byten
    je      exit_loop           #exit hvis 0-byte funnet
    movb    0(%ecx), %dl        #flytter 1. char fra format til edx for mellomlagring
    cmpb    $37, %dl            #sjekker om % (37 er decimal ascii for '%')
    je      format              #if true: jump to format:
loop_back:
    movb    %dl, 0(%ebx)        #flytter edx til 1. char i ebx (storage)
    addl    $1, %ecx            #flytter pekeren til format
    addl    $1, %ebx            #flytter pekeren til storage
    addl    $1, %edi            #teller characters    
    jmp     loop

exit_loop:
    movb    $0, %dl             #legger til 0-byte
    movb    %dl, (%ebx)         #0-byte til storage
    movl    %edi, %eax          #flytter returverdi til eax
    popl    %ebp                #Standard
    ret                         #retur.

format:
    addl    $1, %ecx            #flytter format-pekeren videre
    movb    0(%ecx), %dl        #neste char fra format inn i edx (blir overskrevet)
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
    jmp     loop_back           #hopper tilbake til loop_back i loop og fullfører

char_true: 
    movb    (%eax), %dl         #neste char inn i edx (dl)
    addl    $4, %eax            #neste n arg
    movb    %dl, (%ebx)         #char fra format to storage
    addl    $1, %edi            #teller++
    addl    $1, %ecx            #flytter pekeren til format
    addl    $1, %ebx            #flytter pekeren til storage
    jmp     loop                #fortsetter å loope

string_true:
    addl    $1, %ecx            #format++
    movl    (%eax), %esi        #dereferenser til arg-string
    addl    $4, %eax            #neste args
string_loop:
    cmpb    $0, 0(%esi)         #sjekker for 0-byten
    je      loop                #exit hvis 0-byte funnet
    movb    (%esi), %dl         #char for char til storage
    movb    %dl, (%ebx)
    addl    $1, %edi            #teller++
    addl    $1, %ebx            #storage++
    addl    $1, %esi            #arg++
    jmp     string_loop

decimal_true: 
    movl    (%eax), %edx
    addl    $4, %eax            #next arg
    
                                #.if %edx < 0, since add_int function can only handle numbers >= 0
                                #we have to add a '-' in the string and call add_int(-%edx)
    test    %edx, %edx
    jns     decimal_start       #jump if pos.

    movb    $45, (%ebx)         #a negative sign
    addl    $1, %ebx
    addl    $1, %edi
    
    cmpl    $-2147483648, %edx  #special case %edx == -2147483648
                                #.if it is negative it (2147483648) cannot be stored in the integer
                                #thus do it explicitely
    jne     negative            #jump if not equal
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
    neg     %edx                #negative (two's-comp)

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
    shrl    $4, %edx            #shift logical right
    jmp     convert_loop

convert_done:                   #pop intermediate character, save into result string
copy_loop:
    cmpl    %esp, %eax
    je      copy_done
    popl    %edx
    cmpl    $9, %edx
    jg      ten_more            #jump if greater
    #digit = between 0 - 9, save directly
    addl    $48, %edx
    jmp     save_digit

ten_more:                       #convert 10 --> 'a', 11 --> 'b',...
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