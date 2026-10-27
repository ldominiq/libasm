default rel						; adresses relatives a RIP
section .text					; le code va dans la section .text
global ft_calloc				; rend le symbole visible depuis le C

extern __errno_location
extern malloc

; void *ft_calloc(size_t nmemb, size_t size)
; rdi = nmemb, rsi = size
ft_calloc:						; un label = une adresse (ici debut de la fonction)
	mov rax, rdi
	mul rsi						; rax * rsi sur 128 bits: moitie basse dans rax, haute dans rdx
	jc .overflow				; si la moitie haute est pas nulle, alors ca tiens pas sur 64 bits
	
	push rbx
	mov rdi, rax				; set le parametre a passer a malloc
	mov rbx, rax				; sauvegarde du total dans rbx

	call malloc wrt ..plt
	
	test rax, rax				; check si NULL
	jz .end

	; set a 0 tous les octets
	cld							; defini le directional flag a 0 (rdi++) (deja set par defaut)
	mov rdx, rax				; on save le pointeur pour apres
	mov rdi, rax				; stosb va ecrire a cette adresse rdi
	mov al, 0					; on set la valeur qu'on va ecrire 0
	mov rcx, rbx				; on defini le nb de repetitions
	rep stosb					; execution de la boucle d'ecriture auto
	mov rax, rdx				; on recupere le pointeur 
.end:
	pop rbx
	ret

.overflow:
	mov rax, 12							; set de la valeur d'erreur ENOMEM
	push rax
	call __errno_location wrt ..plt
	pop rdx
	mov [rax], edx						; errno = 12
	mov rax, 0							; valeur de return NULL
	ret


; supprime un warning pour dire qu'on a pas besoin d'une pile executable
section .note.GNU-stack noalloc noexec nowrite progbits
