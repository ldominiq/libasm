default rel						; adresses relatives a RIP
section .text					; le code va dans la section .text
global ft_strcmp				; rend le symbole visible depuis le C

; int ft_strcmp(const char *s1, const char *s2)
; rdi = s1, rsi = s2
ft_strcmp:						; un label = une adresse (ici debut de la fonction)
	xor rdx, rdx				; i = 0 (rdx = 0 -> rdx XOR rdx vaut tjrs 0)
.loop:
	movzx eax, byte [rdi + rdx] ; lecture d'un octet non signe et
	movzx ecx, byte [rsi + rdx]	; mets des 0  dans le reste du registre
	test eax, eax				; on test si eax vaut 0
	je .end						; si == 0 -> on sort
	cmp eax, ecx				; on compare a - b 
	jne .end					; si different -> on sort
	inc rdx						; i++
	jmp .loop					; recommence
.end:
	sub eax, ecx				; a - b
	ret							; retourne eax

; supprime un warning pour dire qu'on a pas besoin d'une pile executable
section .note.GNU-stack noalloc noexec nowrite progbits
