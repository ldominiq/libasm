/*
** Compilation :
**   make
**   cc -g -Wall -Wextra -Werror main.c -L. -lasm -o test
**   ./test
*/

#include <stdio.h>
#include <stdlib.h>   // free
#include <stdint.h>
#include <string.h>
#include <unistd.h>   /* write, read, pipe, close */
#include <fcntl.h>    /* open, O_WRONLY */
#include <errno.h>    /* errno, EBADF, EFAULT */
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

/*                              ft_strcmp                             */
/* ------------------------------------------------------------------ */
 
/*
** Retourne -1, 0 ou 1 selon le signe de x.
** (x > 0) vaut 1 ou 0, (x < 0) vaut 1 ou 0 : la soustraction donne le signe.
*/
static int sign(int x)
{
	return ((x > 0) - (x < 0));
}
 
/*
** Le man de strcmp ne garantit que le SIGNE du resultat (< 0, 0 ou > 0),
** pas sa valeur exacte. On compare donc les signes : c'est ce test qui
** compte. On affiche aussi les deux valeurs pour info.
*/
static void test_strcmp_one(const char *s1, const char *s2, const char *name)
{
	int expected = strcmp(s1, s2);
	int got = ft_strcmp(s1, s2);
 
	check(sign(expected) == sign(got), name);
	if (sign(expected) != sign(got))
		printf("       attendu %d, obtenu %d\n", expected, got);
	else if (expected != got)
		printf("       (meme signe, valeurs differentes : libc %d, toi %d)\n",
			expected, got);
}
 
static void test_strcmp(void)
{
	static char long1[10000];
	static char long2[10000];
 
	/* Deux longues chaines identiques... */
	memset(long1, 'c', sizeof(long1) - 1);
	long1[sizeof(long1) - 1] = '\0';
	memcpy(long2, long1, sizeof(long1));
 
	printf("\n=== ft_strcmp ===\n");
	test_strcmp_one("hello", "hello", "chaines identiques");
	test_strcmp_one("", "", "deux chaines vides");
	test_strcmp_one("", "a", "vide vs non vide");
	test_strcmp_one("a", "", "non vide vs vide");
	test_strcmp_one("abc", "abd", "difference au dernier caractere");
	test_strcmp_one("abd", "abc", "difference au dernier (inverse)");
	test_strcmp_one("xbc", "abc", "difference au premier caractere");
	test_strcmp_one("abc", "abcd", "s1 est un prefixe de s2");
	test_strcmp_one("abcd", "abc", "s2 est un prefixe de s1");
	test_strcmp_one("A", "a", "majuscule vs minuscule");
	test_strcmp_one("hello\0abc", "hello\0xyz", "s'arrete au premier \\0");
 
	/*
	** Le piege du signe : '\xff' vaut 255 en unsigned char, mais -1 en
	** char signe. strcmp compare en UNSIGNED : "\xff" > "a".
	** Si tu lis avec mov au lieu de movzx, ces tests echouent.
	*/
	test_strcmp_one("\xff", "a", "caractere 255 vs 'a' (unsigned !)");
	test_strcmp_one("a", "\xff", "'a' vs caractere 255 (unsigned !)");
	test_strcmp_one("\x80", "\x7f", "128 vs 127 (unsigned !)");
	test_strcmp_one("\xc3\xa9", "e", "'e accent' UTF-8 vs 'e'");
 
	test_strcmp_one(long1, long2, "longues chaines identiques");
	long2[sizeof(long2) - 2] = 'd'; /* ... qui different au dernier caractere */
	test_strcmp_one(long1, long2, "longues chaines, diff a la fin");
}

/* ------------------------------------------------------------------ */
/*                              ft_write                              */
/* ------------------------------------------------------------------ */
 
/*
** Cas d'ERREUR : on appelle write puis ft_write avec les memes arguments,
** et on compare la valeur de retour ET errno.
** errno est remis a 0 avant chaque appel, pour etre sur que c'est bien
** l'appel qu'on teste qui l'a modifie.
*/
static void test_write_error(int fd, const void *buf, size_t count,
	const char *name)
{
	ssize_t expected_ret;
	int expected_errno;
	ssize_t got_ret;
	int got_errno;
 
	errno = 0;
	expected_ret = write(fd, buf, count);
	expected_errno = errno;
 
	errno = 0;
	got_ret = ft_write(fd, buf, count);
	got_errno = errno;
 
	check(expected_ret == got_ret && expected_errno == got_errno, name);
	if (expected_ret != got_ret || expected_errno != got_errno)
		printf("       attendu ret %zd, errno %d (%s)\n"
			"       obtenu  ret %zd, errno %d (%s)\n",
			expected_ret, expected_errno, strerror(expected_errno),
			got_ret, got_errno, strerror(got_errno));
}
 
/*
** Cas de SUCCES : on ecrit dans un pipe, puis on relit ce qui est sorti
** de l'autre cote. Ca permet de verifier que ft_write a ecrit EXACTEMENT
** les bons octets, pas seulement qu'elle renvoie le bon nombre.
**
** Un pipe = un "tuyau" a deux bouts : fds[1] pour ecrire, fds[0] pour lire.
*/
static void test_write_pipe(const char *s, size_t count, const char *name)
{
	int fds[2];
	char buf[100];
	ssize_t ret;
	ssize_t n;
 
	if (pipe(fds) == -1)
	{
		perror("pipe");
		return ;
	}
	ret = ft_write(fds[1], s, count);
	close(fds[1]);   /* on ferme le bout ecriture : read verra la fin */
	n = read(fds[0], buf, sizeof(buf));
	close(fds[0]);
 
	check(ret == (ssize_t)count && n == (ssize_t)count
		&& memcmp(buf, s, count) == 0, name);
	if (ret != (ssize_t)count)
		printf("       retour attendu %zu, obtenu %zd\n", count, ret);
	else if (n != (ssize_t)count || memcmp(buf, s, count) != 0)
		printf("       le contenu ecrit n'est pas le bon\n");
}
 
static void test_write(void)
{
	int fds[2];
	int devnull;
	ssize_t ret;
 
	printf("\n=== ft_write ===\n");
 
	/* --- Succes : le contenu et le retour --- */
	test_write_pipe("hello", 5, "ecrit \"hello\" (5 octets)");
	test_write_pipe("", 0, "count = 0 -> renvoie 0");
	test_write_pipe("hello world", 5, "count < longueur -> n'ecrit que 5 octets");
	test_write_pipe("ab\0cd", 5, "ecrit aussi les \\0 (write ignore les chaines)");
 
	/* --- Succes : errno ne doit PAS etre modifie --- */
	devnull = open("/dev/null", O_WRONLY);
	errno = 4242;
	ret = ft_write(devnull, "abc", 3);
	check(ret == 3 && errno == 4242, "succes -> errno inchange");
	close(devnull);
 
	/* --- Erreurs : retour -1 et le bon errno --- */
	test_write_error(-1, "abc", 3, "fd = -1 -> EBADF");
	test_write_error(42, "abc", 3, "fd 42 non ouvert -> EBADF");
	test_write_error(1, NULL, 5, "buf = NULL -> EFAULT (pas EBADF !)");
 
	/* Ecrire dans le bout LECTURE d'un pipe est interdit -> EBADF */
	if (pipe(fds) == 0)
	{
		test_write_error(fds[0], "abc", 3, "bout lecture d'un pipe -> EBADF");
		close(fds[0]);
		close(fds[1]);
	}
 
	/*
	** Affichage visible sur le terminal.
	** fflush : printf garde son texte dans un buffer, write ecrit tout de
	** suite. Sans fflush, l'ordre d'affichage peut etre melange.
	*/
	fflush(stdout);
	ret = ft_write(1, "  [visible] hello depuis ft_write\n", 34);
	check(ret == 34, "ecrit sur la sortie standard (fd 1)");
}

/* ------------------------------------------------------------------ */
/*                              ft_read                               */
/* ------------------------------------------------------------------ */

/*
** Cas d'ERREUR : on compare read et ft_read (retour ET errno).
** Meme principe que test_write_error.
*/
static void test_read_error(int fd, void *buf, size_t count, const char *name)
{
	ssize_t expected_ret;
	int expected_errno;
	ssize_t got_ret;
	int got_errno;

	errno = 0;
	expected_ret = read(fd, buf, count);
	expected_errno = errno;

	errno = 0;
	got_ret = ft_read(fd, buf, count);
	got_errno = errno;

	check(expected_ret == got_ret && expected_errno == got_errno, name);
	if (expected_ret != got_ret || expected_errno != got_errno)
		printf("       attendu ret %zd, errno %d (%s)\n"
			"       obtenu  ret %zd, errno %d (%s)\n",
			expected_ret, expected_errno, strerror(expected_errno),
			got_ret, got_errno, strerror(got_errno));
}

/*
** Cas de SUCCES : on met "data" dans un pipe avec le vrai write,
** puis ft_read lit au plus "count" octets depuis l'autre bout.
** On verifie le retour ET les octets lus.
*/
static void test_read_pipe(const char *data, size_t data_len, size_t count,
	size_t expected, const char *name)
{
	int fds[2];
	char buf[100];
	ssize_t ret;

	if (pipe(fds) == -1)
	{
		perror("pipe");
		return ;
	}
	if (write(fds[1], data, data_len) != (ssize_t)data_len)
		perror("write");
	close(fds[1]);   /* plus rien a ecrire : apres les donnees, ce sera EOF */

	memset(buf, 'X', sizeof(buf));
	ret = ft_read(fds[0], buf, count);
	close(fds[0]);

	check(ret == (ssize_t)expected && memcmp(buf, data, expected) == 0, name);
	if (ret != (ssize_t)expected)
		printf("       retour attendu %zu, obtenu %zd\n", expected, ret);
	else if (memcmp(buf, data, expected) != 0)
		printf("       les octets lus ne sont pas les bons\n");
}

static void test_read(void)
{
	int fds[2];
	int fd;
	char buf[100];
	char zeros[10];
	ssize_t ret;

	printf("\n=== ft_read ===\n");

	/* --- Succes --- */
	test_read_pipe("hello", 5, 100, 5, "lit \"hello\" (5 octets dispo)");
	test_read_pipe("hello world", 11, 5, 5, "count < dispo -> lit 5 octets");
	test_read_pipe("ab\0cd", 5, 100, 5, "lit aussi les \\0");
	test_read_pipe("", 0, 100, 0, "fin de fichier (EOF) -> renvoie 0");
	test_read_pipe("hello", 5, 0, 0, "count = 0 -> renvoie 0");

	/* /dev/zero : un "fichier" infini rempli de 0 */
	fd = open("/dev/zero", O_RDONLY);
	memset(buf, 'X', sizeof(buf));
	memset(zeros, 0, sizeof(zeros));
	errno = 4242;
	ret = ft_read(fd, buf, 10);
	check(ret == 10 && memcmp(buf, zeros, 10) == 0, "lit 10 octets de /dev/zero");
	check(errno == 4242, "succes -> errno inchange");
	check(buf[10] == 'X', "n'ecrit pas plus loin que count");

	/* --- Erreurs : retour -1 et le bon errno --- */
	test_read_error(-1, buf, 10, "fd = -1 -> EBADF");
	test_read_error(42, buf, 10, "fd 42 non ouvert -> EBADF");
	test_read_error(fd, NULL, 10, "buf = NULL -> EFAULT (pas EBADF !)");
	close(fd);

	/* Lire depuis le bout ECRITURE d'un pipe est interdit -> EBADF */
	if (pipe(fds) == 0)
	{
		test_read_error(fds[1], buf, 10, "bout ecriture d'un pipe -> EBADF");
		close(fds[0]);
		close(fds[1]);
	}

	/* Lire un dossier comme un fichier -> EISDIR (encore un autre errno) */
	fd = open(".", O_RDONLY);
	test_read_error(fd, buf, 10, "lire un dossier -> EISDIR");
	close(fd);
}

/* ------------------------------------------------------------------ */
/*                              ft_strdup                             */
/* ------------------------------------------------------------------ */

/*
** Teste ft_strdup sur une chaine s :
**  - le resultat n'est pas NULL ;
**  - c'est une NOUVELLE zone memoire (adresse differente de s) ;
**  - le contenu est le meme, '\0' final compris (strcmp le verifie) ;
**  - on peut le liberer avec free (sinon : crash ou erreur valgrind).
*/
static void test_strdup_one(const char *s, const char *name)
{
	char *dup;

	dup = ft_strdup(s);
	check(dup != NULL, name);
	if (dup == NULL)
		return ;
	check(dup != s, "  -> nouvelle adresse (pas s lui-meme)");
	check(strcmp(dup, s) == 0, "  -> meme contenu");
	if (strcmp(dup, s) != 0)
		printf("       attendu \"%s\", obtenu \"%s\"\n", s, dup);
	free(dup);
}

static void test_strdup(void)
{
	static char long_str[10000];
	char original[] = "hello";
	char *dup;

	memset(long_str, 'd', sizeof(long_str) - 1);
	long_str[sizeof(long_str) - 1] = '\0';

	printf("\n=== ft_strdup ===\n");
	test_strdup_one("hello", "chaine simple");
	test_strdup_one("", "chaine vide");
	test_strdup_one("a", "un seul caractere");
	test_strdup_one("\xff\x80\xc3\xa9", "caracteres >= 128 (non-ASCII)");
	test_strdup_one(long_str, "chaine de 9999 caracteres");

	/* strdup copie jusqu'au PREMIER '\0', comme strlen */
	dup = ft_strdup("hello\0world");
	check(dup != NULL && strcmp(dup, "hello") == 0, "s'arrete au premier \\0");
	free(dup);

	/*
	** La copie doit etre INDEPENDANTE de l'original :
	** modifier l'un ne doit pas changer l'autre.
	*/
	dup = ft_strdup(original);
	if (dup != NULL)
	{
		dup[0] = 'J';
		check(original[0] == 'h', "modifier la copie ne change pas l'original");
		original[1] = 'a';
		check(dup[1] == 'e', "modifier l'original ne change pas la copie");
		free(dup);
	}
}

/* ------------------------------------------------------------------ */
/*                              ft_calloc                             */
/* ------------------------------------------------------------------ */

/* Renvoie 1 si les "n" octets de p valent tous 0 */
static int is_zeroed(const unsigned char *p, size_t n)
{
	size_t i;

	i = 0;
	while (i < n)
	{
		if (p[i] != 0)
			return (0);
		i++;
	}
	return (1);
}

/*
** Cas normal : ft_calloc(nmemb, size) doit renvoyer nmemb * size octets
** tous a 0.
**
** Piege : de la memoire toute neuve vaut souvent deja 0, meme sans memset.
** Pour que le test serve a quelque chose, on salit d'abord un bloc de la
** meme taille avec 'X', puis on le libere : malloc va tres probablement
** redonner ce meme bloc a ft_calloc. S'il n'est pas remis a 0, on le voit.
*/
static void test_calloc_one(size_t nmemb, size_t size, const char *name)
{
	size_t total = nmemb * size;
	unsigned char *dirty;
	unsigned char *p;

	dirty = malloc(total);
	if (dirty != NULL)
	{
		memset(dirty, 'X', total);
		free(dirty);
	}

	p = ft_calloc(nmemb, size);
	check(p != NULL, name);
	if (p == NULL)
		return ;
	check(is_zeroed(p, total), "  -> tous les octets valent 0");
	/* On ecrit sur TOUT le bloc : si ft_calloc a alloue trop peu,
	   valgrind le signale ("Invalid write") */
	memset(p, 'a', total);
	free(p);
}

/*
** Debordement : nmemb * size ne tient pas sur 64 bits.
** ft_calloc doit renvoyer NULL et mettre errno a ENOMEM (12).
*/
static void test_calloc_overflow(size_t nmemb, size_t size, const char *name)
{
	void *p;

	errno = 0;
	p = ft_calloc(nmemb, size);
	check(p == NULL && errno == ENOMEM, name);
	if (p != NULL)
	{
		printf("       attendu NULL, obtenu %p (debordement non detecte)\n", p);
		free(p);
	}
	else if (errno != ENOMEM)
		printf("       errno attendu %d (ENOMEM), obtenu %d (%s)\n",
			ENOMEM, errno, strerror(errno));
}

static void test_calloc(void)
{
	void *expected;
	void *got;

	printf("\n=== ft_calloc ===\n");

	/* --- Cas normaux --- */
	test_calloc_one(10, 4, "10 x 4 octets");
	test_calloc_one(1, 1, "1 x 1 octet");
	test_calloc_one(4, 1, "4 x 1 octet");
	test_calloc_one(1000, 1000, "1000 x 1000 octets (1 Mo)");
	test_calloc_one(3, sizeof(long), "3 x sizeof(long)");

	/*
	** Taille 0 : on compare avec le vrai calloc.
	** Avec glibc, calloc(0, 5) renvoie un pointeur valide (pas NULL),
	** qu'on doit pouvoir passer a free.
	*/
	expected = calloc(0, 5);
	got = ft_calloc(0, 5);
	check((expected == NULL) == (got == NULL), "nmemb = 0 (comme calloc)");
	free(expected);
	free(got);

	expected = calloc(5, 0);
	got = ft_calloc(5, 0);
	check((expected == NULL) == (got == NULL), "size = 0 (comme calloc)");
	free(expected);
	free(got);

	/* --- Debordement de nmemb * size --- */
	test_calloc_overflow(SIZE_MAX, 2, "SIZE_MAX x 2 -> NULL + ENOMEM");
	test_calloc_overflow(2, SIZE_MAX, "2 x SIZE_MAX -> NULL + ENOMEM");
	/*
	** Le plus piegeux : 2^32 x 2^32 = 2^64, qui vaut EXACTEMENT 0
	** une fois tronque a 64 bits. Sans detection, on ferait malloc(0),
	** qui REUSSIT : le bug ne se voit pas sans ce test.
	*/
	test_calloc_overflow((size_t)1 << 32, (size_t)1 << 32,
		"2^32 x 2^32 (tronque = 0 !) -> NULL + ENOMEM");
	test_calloc_overflow(SIZE_MAX / 2 + 1, 2,
		"(SIZE_MAX/2 + 1) x 2 (tronque = 0 !) -> NULL + ENOMEM");

	/*
	** Pas de debordement, mais beaucoup trop gros pour la machine :
	** c'est malloc qui echoue, et c'est lui qui met errno = ENOMEM.
	*/
	test_calloc_overflow(SIZE_MAX, 1, "SIZE_MAX x 1 (trop gros) -> NULL + ENOMEM");
}

/* ------------------------------------------------------------------ */

int main(void)
{
	test_strlen();
	test_strcpy();
	test_strcmp();
	test_write();
	test_read();
	test_strdup();
	test_calloc();

	printf("\n%d OK, %d KO\n", g_ok, g_ko);
	/* Code de sortie 1 si au moins un test a rate (pratique en script) */
	return (g_ko != 0);
}
