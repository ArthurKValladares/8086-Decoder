8086 decoder, inspired by Casey Muratori's decoder.

Takes an 8086 `.asm` file as input, compiles using `nasm`, parses/decodes the compiled binary into a new human-readable `.asm` file, them compiles that and compares the resulting binary to the original compiled binary to see if they match perfectly.

While the resulting binaries match, the human-readable `.asm` files can have synthetic differences, as shown by the comparisson below:

## Input
```
bits 16
add bx, [bx+si]
add bx, [bp]
add si, 2
add bp, 2
add cx, 8
add bx, [bp + 0]
add cx, [bx + 2]
add bh, [bp + si + 4]
add di, [bp + di + 6]
add [bx+si], bx
add [bp], bx
add [bp + 0], bx
add [bx + 2], cx
add [bp + si + 4], bh
add [bp + di + 6], di
add byte [bx], 34
add word [bp + si + 1000], 29
add ax, [bp]
add al, [bx + si]
add ax, bx
add al, ah
add ax, 1000
add al, -30
add al, 9
sub bx, [bx+si]
sub bx, [bp]
sub si, 2
sub bp, 2
sub cx, 8
sub bx, [bp + 0]
sub cx, [bx + 2]
sub bh, [bp + si + 4]
sub di, [bp + di + 6]
sub [bx+si], bx
sub [bp], bx
sub [bp + 0], bx
sub [bx + 2], cx
sub [bp + si + 4], bh
sub [bp + di + 6], di
sub byte [bx], 34
sub word [bx + di], 29
sub ax, [bp]
sub al, [bx + si]
sub ax, bx
sub al, ah
sub ax, 1000
sub al, -30
sub al, 9
cmp bx, [bx+si]
cmp bx, [bp]
cmp si, 2
cmp bp, 2
cmp cx, 8
cmp bx, [bp + 0]
cmp cx, [bx + 2]
cmp bh, [bp + si + 4]
cmp di, [bp + di + 6]
cmp [bx+si], bx
cmp [bp], bx
cmp [bp + 0], bx
cmp [bx + 2], cx
cmp [bp + si + 4], bh
cmp [bp + di + 6], di
cmp byte [bx], 34
cmp word [4834], 29
cmp ax, [bp]
cmp al, [bx + si]
cmp ax, bx
cmp al, ah
cmp ax, 1000
cmp al, -30
cmp al, 9
test_label0:
jnz test_label1
jnz test_label0
test_label1:
jnz test_label0
jnz test_label1
label:
je label
jl label
jle label
jb label
jbe label
jp label
jo label
js label
jne label
jnl label
jg label
jnb label
ja label
jnp label
jno label
jns label
loop label
loopz label
loopnz label
jcxz label
```

## Output

```
bits 16
ADD bx, [bx + si]
ADD bx, [bp]
ADD si, 2
ADD bp, 2
ADD cx, 8
ADD bx, [bp]
ADD cx, [bx + 2]
ADD bh, [bp + si + 4]
ADD di, [bp + di + 6]
ADD [bx + si], bx
ADD [bp], bx
ADD [bp], bx
ADD [bx + 2], cx
ADD [bp + si + 4], bh
ADD [bp + di + 6], di
ADD byte [bx], 34
ADD word [bp + si + 1000], 29
ADD ax, [bp]
ADD al, [bx + si]
ADD ax, bx
ADD al, ah
ADD ax, 1000
ADD al, 226
ADD al, 9
SUB bx, [bx + si]
SUB bx, [bp]
SUB si, 2
SUB bp, 2
SUB cx, 8
SUB bx, [bp]
SUB cx, [bx + 2]
SUB bh, [bp + si + 4]
SUB di, [bp + di + 6]
SUB [bx + si], bx
SUB [bp], bx
SUB [bp], bx
SUB [bx + 2], cx
SUB [bp + si + 4], bh
SUB [bp + di + 6], di
SUB byte [bx], 34
SUB word [bx + di], 29
SUB ax, [bp]
SUB al, [bx + si]
SUB ax, bx
SUB al, ah
SUB ax, 1000
SUB al, 226
SUB al, 9
CMP bx, [bx + si]
CMP bx, [bp]
CMP si, 2
CMP bp, 2
CMP cx, 8
CMP bx, [bp]
CMP cx, [bx + 2]
CMP bh, [bp + si + 4]
CMP di, [bp + di + 6]
CMP [bx + si], bx
CMP [bp], bx
CMP [bp], bx
CMP [bx + 2], cx
CMP [bp + si + 4], bh
CMP [bp + di + 6], di
CMP byte [bx], 34
CMP word [4834], 29
CMP ax, [bp]
CMP al, [bx + si]
CMP ax, bx
CMP al, ah
CMP ax, 1000
CMP al, 226
CMP al, 9
JNE $+2+2
JNE $+2-4
JNE $+2-6
JNE $+2-4
JE $+2-2
JL $+2-4
JLE $+2-6
JB $+2-8
JBE $+2-10
JP $+2-12
JO $+2-14
JS $+2-16
JNE $+2-18
JNL $+2-20
JG $+2-22
JNB $+2-24
JA $+2-26
JNP $+2-28
JNO $+2-30
JNS $+2-32
LOOP $+2-34
LOOPZ $+2-36
LOOPNZ $+2-38
JCXZ $+2-40
```
