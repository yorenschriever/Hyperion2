#include_next <net/if_dl.h>

#ifndef _FREEBSD_NET_IF_DL_H_
#define _FREEBSD_NET_IF_DL_H_

__BEGIN_DECLS
void	link_addr(const char *, struct sockaddr_dl *);
char	*link_ntoa(const struct sockaddr_dl *);
__END_DECLS

#endif
