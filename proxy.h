/*
 * Management of parent proxies
 *
 * CNTLM is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * CNTLM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright (c) 2022 Francesco MDE aka fralken, David Kubicek
 *
 */

#ifndef PROXY_H
#define PROXY_H

#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>

#include "utils.h"
#include "auth.h"

/* Forward declarations */
typedef struct proxylist_s *proxylist_t;
typedef const struct proxylist_s *proxylist_const_t;

/* Proxy types defined by PAC specification */
enum proxy_type_t { DIRECT, PROXY };

/* Proxy statistics structure */
typedef struct {
	int current_connections;	/* Active connections to this proxy */
	int total_connections;		/* Total connections served */
	int failed_connections;		/* Failed connection attempts */
	time_t last_failure;		/* Timestamp of last failure */
} proxy_stats_t;

/* Proxy structure */
typedef struct {
	enum proxy_type_t type;
	char hostname[64];
	int port;
	struct auth_s creds;
	struct addrinfo *addresses;
	int resolved;
	proxy_stats_t stats;		/* Statistics for load balancing */
	pthread_mutex_t stats_mtx;	/* Per-proxy mutex for stats */
} proxy_t;

extern int proxy_connect(struct auth_s *credentials, const char* url, const char* hostname);
extern int proxy_authenticate(int *sd, rr_data_t request, rr_data_t response, struct auth_s *creds);

extern int parent_add(const char *parent, int port);
extern int parent_available(void);
extern void parent_free(void);

/* Round-robin load balancing functions */
extern void proxy_connection_start(proxy_t *proxy);
extern void proxy_connection_end(proxy_t *proxy);
extern int proxy_is_available(proxy_t *proxy);
extern proxy_t *select_proxy_roundrobin(proxylist_const_t proxylist, int proxycount);
extern unsigned long proxylist_get_key_by_proxy(proxylist_const_t list, proxy_t *proxy);

#endif /* PROXY_H */
