default rel						; adresses relatives a RIP
section .text					; le code va dans la section .text
global ft_strdup				; rend le symbole visible depuis le C

extern malloc
extern ft_strlen
extern ft_strcpy

; char *ft_strdup(const char *s)
; rdi = s
ft_strdup:						; un label = une adresse (ici debut de la fonction)
	push rbx					; sauvegarde rbx (callee-saved) + aligne la pile
	mov rbx, rdi				; rbx = rdi
	
	call ft_strlen

	mov rdi, rax				; resulat de strlen
	inc rdi						; len + 1 pour \0
	;; rdi = nb octets voulus (1er argument de malloc)
	call malloc wrt ..plt

	test rax, rax				; malloc renvoie NULL ?
	jz .end						; si oui, on gere l'echec

	mov rdi, rax				; addresse de la memoire aloue pour dst
	mov rsi, rbx				; string a copier s (src)
	call ft_strcpy				; renvoie dst dans rax
	; rax = pointeur vers la memoire
.end:
	pop rbx 					; restaure rbx sur TOUS les chemins de sortie
	ret


; supprime un warning pour dire qu'on a pas besoin d'une pile executable
section .note.GNU-stack noalloc noexec nowrite progbits
