default rel						; adresses relatives a RIP
section .text					; le code va dans la section .text
global ft_strlen				; rend le symbole visible depuis le C

; size_t ft_strlen(const char *s)
; rdi = s
ft_strlen:						; un label = une adresse (ici debut de la fonction)
	xor rax, rax				; i = 0 (rax = 0 -> rax XOR rax vaut tjrs 0)
.loop:
	;(cmp: fait une soustraction, byte: on compare un seul octet (un char))
	cmp byte [rdi + rax], 0		; s[i] == '\0' ? 
	;(result de comparaison ZF=1 -> egal/zero)
	je .end						; oui -> on sort
	inc rax						; i++
	jmp .loop					; recommence
.end:
	ret							; retourne i (dans rax)

; supprime un warning pour dire qu'on a pas besoin d'une pile executable
section .note.GNU-stack noalloc noexec nowrite progbits
