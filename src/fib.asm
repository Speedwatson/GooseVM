[section __code]

__start:
	push 1
	push 1
	inp b n

	jmp cycle

cycle:
	; checking whether we've got the answer
	push 2
	push *n
	jle answer

	; decreasing n
	push *n
	dec
	pop n

	; calculating the next pair
	sav t
	add
	push *t
	swp

	jmp cycle

answer:
	out
	stop 0
	

[section __data]
n:	resb 1
t:	resq 1