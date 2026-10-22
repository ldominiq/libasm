/*
** Tests de ft_strlen et ft_strcpy.
** Principe : on appelle la vraie fonction (libc) et la notre sur les memes
** entrees, et on compare les resultats.
**
** Compilation :
**   make
**   cc -g -Wall -Wextra -Werror main.c -L. -lasm -o test
**   ./test
*/

#include <stdio.h>
#include <string.h>
#include "libasm.h"

/* Compteurs globaux pour afficher un resume a la fin */
static int g_ok = 0;
static int g_ko = 0;

/*
** Affiche [OK] ou [KO] selon la condition.
** "name" sert juste a savoir quel test a rate.
*/
static void check(int condition, const char *name)
{
	if (condition)
	{
		printf("  [OK] %s\n", name);
		g_ok++;
	}
	else
	{
		printf("  [KO] %s\n", name);
		g_ko++;
	}
}

/* ------------------------------------------------------------------ */
/*                              ft_strlen                             */
/* ------------------------------------------------------------------ */

/* Compare strlen et ft_strlen sur une chaine */
static void test_strlen_one(const char *s, const char *name)
{
	size_t expected = strlen(s);
	size_t got = ft_strlen(s);

	check(expected == got, name);
	if (expected != got)
		printf("       attendu %zu, obtenu %zu\n", expected, got);
}

static void test_strlen(void)
{
	/* Chaine longue : 9999 'a' + '\0'. static = pas sur la pile. */
	static char long_str[10000];

	memset(long_str, 'a', sizeof(long_str) - 1);
	long_str[sizeof(long_str) - 1] = '\0';

	printf("=== ft_strlen ===\n");
	test_strlen_one("hello", "chaine simple");
	test_strlen_one("", "chaine vide");
	test_strlen_one("a", "un seul caractere");
	test_strlen_one("hello\0world", "\\0 au milieu (doit s'arreter au 1er)");
	test_strlen_one("\xff\x80\xc3\xa9", "caracteres >= 128 (non-ASCII)");
	test_strlen_one(long_str, "chaine de 9999 caracteres");
}

/* ------------------------------------------------------------------ */
/*                              ft_strcpy                             */
/* ------------------------------------------------------------------ */

/*
** Teste ft_strcpy sur une chaine src.
**
** Astuce : on remplit les deux buffers de 'X' AVANT la copie.
** Apres la copie, on compare les buffers ENTIERS avec memcmp :
**  - si ft_strcpy oublie le '\0', la difference se voit ;
**  - si ft_strcpy ecrit trop loin, les 'X' apres le '\0' changent,
**    et la difference se voit aussi.
*/
static void test_strcpy_one(const char *src, const char *name)
{
	static char expected[10100];
	static char got[10100];
	char *ret;

	memset(expected, 'X', sizeof(expected));
	memset(got, 'X', sizeof(got));

	strcpy(expected, src);
	ret = ft_strcpy(got, src);

	check(memcmp(expected, got, sizeof(got)) == 0, name);
	/* strcpy doit retourner dst (la meme adresse que le buffer) */
	check(ret == got, "  -> retourne bien dst");
}

static void test_strcpy(void)
{
	static char long_str[10000];
	char small[20];

	memset(long_str, 'b', sizeof(long_str) - 1);
	long_str[sizeof(long_str) - 1] = '\0';

	printf("\n=== ft_strcpy ===\n");
	test_strcpy_one("hello", "chaine simple");
	test_strcpy_one("", "chaine vide (dst[0] doit etre '\\0')");
	test_strcpy_one("a", "un seul caractere");
	test_strcpy_one("\xff\x80\xc3\xa9", "caracteres >= 128 (non-ASCII)");
	test_strcpy_one(long_str, "chaine de 9999 caracteres");

	/*
	** Ecraser une chaine plus longue par une plus courte :
	** apres la copie, dst doit valoir exactement "hi".
	*/
	strcpy(small, "abcdefghij");
	ft_strcpy(small, "hi");
	check(strcmp(small, "hi") == 0, "ecrase une chaine plus longue");

	/* Affichage pour verifier a l'oeil */
	printf("  resultat visible : \"%s\"\n", ft_strcpy(small, "hello 42"));
}

/* ------------------------------------------------------------------ */

int main(void)
{
	test_strlen();
	test_strcpy();

	printf("\n%d OK, %d KO\n", g_ok, g_ko);
	/* Code de sortie 1 si au moins un test a rate (pratique en script) */
	return (g_ko != 0);
}
