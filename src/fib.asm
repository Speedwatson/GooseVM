[section __code]

__start:
	push q 1
	push q 1
	inp b n

	jmp cycle

cycle:
	; checking whether we've got the answer
	push b 2
	push *n
	jle answer

	; decreasing n
	push *n
	dec
	pop n b

	; calculating the next pair
	sav t q
	add
	push *t
	swp

	jmp cycle

answer:
	out q
	stop 0
	

[section __data]
n:	resb 1
t:	resq 1