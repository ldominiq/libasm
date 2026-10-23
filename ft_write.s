default rel						; adresses relatives a RIP
section .text					; le code va dans la section .text
global ft_write					; rend le symbole visible depuis le C

extern __errno_location

; ssize_t ft_write(int fd, const void *buf, size_t count)
; rdi = fd, rsi = buf, rdx = count
ft_write:						; un label = une adresse (ici debut de la fonction)
	mov rax, 1					; le syscall va regarder le service demander (1 = write)
	syscall						; appel au noyau : write (rdi, rsi, rdx)
	test rax, rax
	js .error					; signe negatif -> erreur
	ret							; succes : rax = nb octets
.error:
	neg rax						; rax = -rax = code errno positif
	push rax					; on le garde (aligne la pile)
	call __errno_location wrt ..plt		; rax = &errno
	pop rdx						; le code errno
	mov [rax], edx				; errno = le code (edx: errno est un int, 32 bits)
	mov rax, -1					; valeur de retour -1
	ret


; supprime un warning pour dire qu'on a pas besoin d'une pile executable
section .note.GNU-stack noalloc noexec nowrite progbits
