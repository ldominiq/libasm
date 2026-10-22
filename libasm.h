#ifndef LIBASM_H
#define LIBASM_H

#include <stddef.h>
#include <sys/types.h>

size_t	ft_strlen(const char *s);
char	*ft_strcpy(char *dst, const char *src);
int		ft_strcmp(const char *s1, const char *s2);
ssize_t	ft_write(int fd, const void *buf, size_t count);
ssize_t	ft_read(int fd, void *buf, size_t count);
char	*ft_strdup(const char *s);
void	*ft_calloc(size_t nmenb, size_t size);

#endif
