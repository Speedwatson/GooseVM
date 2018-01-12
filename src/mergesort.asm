[section __data]

len:    resd 1
start:  resd 1
end:    resd 1

i:      resd 1  
j:      resd 1
k:      resd 1
secstart: resd 1

; We have 2^15 addressable elements (word is signed). 28 out of them should be reserved for the values mentioned above.
; If we divide the rest between two arrays, we get 16370.
arr: resw 16370  
arrm: resw 16370   

[section __code]

__start:
	call io.readarr

	push b 0
	push d *len
	dec
	call mergesort 2 	; mergesort(0, len - 1)

	call io.printarr
	stop 0


io:
	.readarr:
		..readlen:
			inp d *len
			push b 0 ; int i

		..readchar:
			; while i != 0
			dpl
			push d *len
			je io.readarr.return ; RESOLVING ..return IS NOT WORKING HERE, YURY DMITRIEVICH!

			; arr[i] = int(input())
			dpl
			push b 2
			mlt	; i *= sizeof(word) to calculate offset in bytes
			push w arr ; pushing OFFSET of arr ("&arr")
			add
			inp w *(pop) ; destOfs = (*(pop) arr ADD) in Reverse Polish Notation

			inc	; ++i;
			jmp io.readarr.readchar

		..return:
			ret


	.printarr:
		push b 0 ; byte i = 0

		..printchar:
			; while i != len
			dpl
			push d *len
			je io.printarr.return

			; print(arr[i])
			dpl
			push b 2
			mlt
			push w arr
			add
			out w *(pop)

			call io.printspace			

			inc ; ++i;
			jmp io.printarr.printchar

		..return:
			call io.printn
			ret

	.printspace:
		push b 32
		out c
		ret

	.printn:
		push b 10
		out c
		ret


mergesort:;(word start, word end):
	dpl 2
	je mergesort.return ; if (start == end): return

	dpl 2
	pop d *end
	sav d *start
	push d *end 

	call utils.mid 2 ; [start][end][mid(start, end)] should be on the stack now
	dpl
	push d *start
	swp ; [start][end][mid(start, end)][start][mid(start, end)]
	call mergesort 2 ; calling mergesort(start, mid(start, end))


	inc ; [start][end][mid(start, end) + 1]
	swp
	sav d *end
	swp ; [start][end][mid(start, end) + 1]
	push d *end
	call mergesort 2 ; calling mergesort(mid(start, end) + 1, end)

	call merge 2 ; calling merge(start, end)

	.return:
	ret


merge:;(dword start, dword end):
	push d 0
	pop d *k ; k = 0

	swp
	sav d *i ; i = start
	swp

	dpl 2
	call utils.mid 2
	inc ; [start][end][mid(start, end) + 1]

	sav d *secstart ; secstart = mid(start, end) + 1

	pop d *j ; [start][end]

	.iteration:
		; assuming there's [start][end] on the stack
		dpl 2
		sub
		inc ; [start][end][(end - start) + 1]

		push d *k ; [start][end][(end - start) + 1][k]
		; swp	; [start][end][k][(end - start) + 1]

		jge merge.endloop ; if (k >= (end - start) + 1): break

		; [start][end]

		push d *i
		push d *secstart
		je merge.iteration.sel.second ; if i == mid(start, end) + 1 => sel.second

		dpl
		inc
		push d *j ; [start][end][end + 1][j]
		je merge.iteration.sel.first ; elif j == end + 1 => sel.first

		push w arr
		push d *j
		call utils.index 2 ; [start][end][arr[j * 2]]

		push w arr
		push d *i
		call utils.index 2 ; [start][end][arr[j * 2]][arr[i * 2]]

		jl merge.iteration.sel.first ; elif arr[i] < arr[j] => sel.first

		jmp merge.iteration.sel.second ; else => sel.second

		..sel:
			...first:
				push w i
				call merge.iteration.sel.func 1

				jmp merge.iteration.endif

			...second:
				push w j
				call merge.iteration.sel.func 1
				
				jmp merge.iteration.endif	

			...func:;(dword selectedIndex as si):
				dpl
				push d *(pop)
				push w arr
				swp ; [&si][arr][si]
				call utils.index 2  ; [&si][arr[si]]

				push w arrm
				push d *k
				push b 2
				mlt ; [&si][arr[si]][arrm][k * 2]
				add ; [&si][arr[si]][arrm + k*2]

				pop w *(pop) ; b[k] = arr[si]

				dpl	
				push d *(pop) ; [&si][si]
				inc	; [&si][si + 1]
				swp
				pop d *(pop) 

				ret

		..endif:
			push d *k
			inc
			pop d *k ; k += 1
			jmp merge.iteration

	.endloop:
		push b 0
		pop d *k ; k = 0

		..iteration:
			; assuming there's [start][end] on the stack
			dpl 2
			sub
			inc ; [start][end][(end - start) + 1]

			push d *k  ; [start][end][(end - start) + 1][k]
			; swp	; [start][end][k][(end - start) + 1]

			jge merge.endloop.return ; if (k >= (end - start) + 1): break

			swp
			sav d *start
			swp

			push w arrm
			push w *k
			call utils.index 2 ; [start][end][arrm[k * 2]]

			push w arr
			push d *k
			push d *start
			add ; [start][end][arr][k + start]
			push b 2
			mlt ; [start][end][arrm[k * 2]][arr][(start + k) * 2]
			add ; [start][end][arrm[k * 2]][arr + (start + k)*2]

			pop w *(pop) ; arr[start + k] = b[k]

			push d *k
			inc
			pop d *k ; k += 1
			jmp merge.endloop.iteration

		..return:
			ret


utils:
	.mid:;(dword start, dword end) -> dword:
		dpl 2
		sub ; [start][end][end - start]

		push b 2
		swp
		div ; [start][end][(end - start) // 2]

		swp
		pop ; [start][(end - start) // 2]
		add ; [start + (end - start) // 2]

		ret 1

	.index:;(word startofs, dword ind) -> dword:
		push b 2
		mlt ; [startofs][ind * 2]
		add ; [startofs + ind*2]
		push w *(pop) ; [startofs[ind*2]]

		ret 1


; Mistakes I made during writing of this algorithm:
; 	1) Missing dereference when it's needed - be careful to the asterisk when pushing/popping stuff. Consider it every time you write a label.
; 	2) Wrong comparator - make sure you write exactly the one you need or it'll screw everything up. Check that the right operands are on stack and they're in the right oreder.
; 	3) Forgot to increase counter in cycle - no comments here
; 	4) Wrong type specified - WHEREVER you specify a type be ABSOLUTELY sure what value (semantically) are you working with and what its type is.