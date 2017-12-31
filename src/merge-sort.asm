[section __code]

__start:
	call input.readarr
	brk

input:
	.readarr:
		..readlen:
			inp d
			sav d len

		..readchar:
			; while len != 0
			dpl
			jez ..return

			dec	; --len;

			; arr[len] = int(input())
			dpl
			push &arr
			inp w *(pop) ; ofs16 = arr + pop

			jmp ..readchar

		..return:
			ret



	

[section __data]
len: resd 1	; LEN(gth) has type of DWORD, so the array can have up to 0x10000 elements

tb: resb 1
tw: resw 1
td: resd 1
tq: resq 1

arr: resw 0x10000 ; 0xFFFF + 1 - type of arr ELEMENTS is WORD
arr2: resw 0x10000 ; 0xFFFF + 1
