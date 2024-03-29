;부트로더
;disk 이미지를 0x10000에 로드함


[ORG 0X00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

TOTALSECTORCOUNT: dw 0x02
KERNEL32SECTORCOUNT dw 0x02
BOOTSTRAPPROCESSOR: db 0x01
STARTGRAPHICMODE: db 0x01

START:
    mov ax, 0x07C0
    mov ds, ax
    mov ax, 0xB800
    mov es, ax

    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE

    mov byte[ BOOTDRIVE ], dl

.SCREENCLEARLOOP:
    mov byte [ es: si ], 0

    mov byte [ es: si + 1], 0x0A

    add si, 2

    cmp si, 80 * 25 * 2

    jl .SCREENCLEARLOOP

    push MESSAGE1
    push 0
    push 0
    call PRINTMESSAGE
    add sp, 6

    push IMAGELOADINGMESSAGE
    push 1
    push 0
    call PRINTMESSAGE
    add sp, 6

RESETDISK:
    mov ax, 0
    mov dl, byte [BOOTDRIVE]
    int 0x13

    jc HANDLEDISKERROR

    mov ah, 0x08
    mov dl, byte [BOOTDRIVE]
    int 0x13
    jc HANDLEDISKERROR

    mov byte [LASTHEAD], dh
    mov al, cl
    and al, 0x3f

    mov byte [LASTSECTOR], al
    mov byte [LASTTRACK], ch

    mov si, 0x1000
    mov es, si
    mov bx, 0x0000

    ;mov di, word [ TOTALSECTORCOUNT ]
    mov di, 1146

READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

    mov ah, 0x02
    mov al, 0x1
    mov ch, byte [ TRACKNUMBER ]
    mov cl, byte [ SECTORNUMBER ]
    mov dh, byte [ HEADNUMBER ]
    mov dl, byte [BOOTDRIVE]
    int 0x13
    jc HANDLEDISKERROR

    add si, 0x0020

    mov es, si

    mov al, byte [ SECTORNUMBER ]
    add al, 0x01
    mov byte [ SECTORNUMBER ], al
    cmp al, byte [LASTSECTOR]
    jle READDATA

    add byte [ HEADNUMBER ], 0x01
    mov byte [ SECTORNUMBER ], 0x01

    mov al, byte [LASTHEAD]
    cmp byte [ HEADNUMBER ], al
    jg .ADDTRACK

    jmp READDATA

.ADDTRACK:
    mov byte [HEADNUMBER], 0x00
    add byte [ TRACKNUMBER ], 0x01
    jmp READDATA

READEND:
    push LOADINGCOMPLETEMESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    add sp, 6

    mov ax, 0x4F01
    mov cx, 0x117
    mov bx, 0x07E0
    mov es, bx
    mov di, 0x00
    int 0x10
    cmp ax, 0x004F
    jne VBEERROR

    cmp byte [STARTGRAPHICMODE], 0x00
    je JUMPTOPROTECTEDMODE

    mov ax, 0x4F02
    mov bx, 0x4117

    int 0x10
    cmp ax, 0x004F
    jne VBEERROR

    jmp JUMPTOPROTECTEDMODE

VBEERROR:
    push CHANGEGRAPHICMODEFAIL
    push 2
    push 0
    call PRINTMESSAGE
    add sp, 6
    jmp $

JUMPTOPROTECTEDMODE:
    jmp 0x1000:0x0000

HANDLEDISKERROR:
    push DISKERRORMESSAGE
    push 1
    push 20
    call PRINTMESSAGE

    jmp $

PRINTMESSAGE:
    push bp
    mov bp, sp
    
    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800

    mov es, ax

    mov ax, word [ bp + 6 ]
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [ bp + 4 ]
    mov si, 2
    mul si
    add di, ax

    mov si, word [bp + 8]

.MESSAGELOOP:
    mov cl, byte [ si ]

    cmp cl, 0
    je .MESSAGEEND

    mov byte [ es: di ], cl
    
    add si, 1
    add di, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

MESSAGE1: db 'MINT64 OS Boot Loader Start by jjangu', 0
DISKERRORMESSAGE: db 'DISK Error!!', 0
IMAGELOADINGMESSAGE: db 'OS Image Loading.....', 0
LOADINGCOMPLETEMESSAGE: db 'Complete', 0
CHANGEGRAPHICMODEFAIL: db 'Change Graphic Mode Fail', 0

SECTORNUMBER: db 0x02
HEADNUMBER: db 0x00
TRACKNUMBER: db 0x00

BOOTDRIVE: db 0x00
LASTSECTOR: db 0x00
LASTHEAD: db 0x00
LASTTRACK: db 0x00

times 510 - ( $ - $$ )   db  0x00
db 0x55
db 0xAA