default rel						; adresses relatives a RIP
section .text					; le code va dans la section .text
global ft_strcpy				; rend le symbole visible depuis le C

; char *ft_strcpy(char *dst, const char *src)
; rdi = dst, rsi = src
ft_strcpy:						; un label = une adresse (ici debut de la fonction)
	xor rdx, rdx				; i = 0 (rdx = 0 -> rdx XOR rdx vaut tjrs 0)
	mov rax, rdi				; copie du registre rdi dans rax
.loop:
	mov cl, [rsi + rdx]			; move src[i] in cl
	mov [rax + rdx], cl			; move cl in dst[i]
	;(result de comparaison ZF=1 -> egal/zero)
	test cl, cl					; on test si cl == '\0'
	je .end						; si oui -> on sort
	inc rdx						; i++
	jmp .loop					; recommence
.end:
	ret							; retourne dst (dans rax)

; supprime un warning pour dire qu'on a pas besoin d'une pile executable
section .note.GNU-stack noalloc noexec nowrite progbits
