[section __code]

__start:
	push 1
	push 1
	nbinp
	pop &n

	jmp cycle

cycle:
	; checking whether we've got the answer
	push 2
	push n
	jle answer

	; decreasing n
	push n
	dec
	pop n

	; calculating the next pair
	sav t
	add
	push t
	swp

	jmp cycle

answer:
	out
	push 0
	stop
	

[section __data]
n:	resb 1
t:	resq 1