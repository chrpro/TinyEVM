/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *    Hauke Mehrtens - memory optimization, ECC integration
 *
 *******************************************************************************/

#include "tinydtls.h"
#include "netq.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#ifndef assert
#warning "assertions are disabled"
#  define assert(x)
#endif
#endif

#include "lib/memb.h"

/* Log configuration */
#define LOG_MODULE "netq"
#define LOG_LEVEL  LOG_LEVEL_DTLS
#include "dtls-log.h"

MEMB(netq_storage, netq_t, NETQ_MAXCNT);

static inline netq_t *
netq_malloc_node(size_t size) {
  return (netq_t *)memb_alloc(&netq_storage);
}

static inline void
netq_free_node(netq_t *node) {
  memb_free(&netq_storage, node);
}

void
netq_init() {
  memb_init(&netq_storage);
}

int
netq_insert_node(netq_t **queue, netq_t *node) {
  netq_t *p, *op;

  assert(queue);
  assert(node);

  p = *queue;
  op = NULL;
  while(p && p->t <= node->t) {
    op = p;
    p = p->next;
  }

  node->next = p;
  if(op) {
    op->next = node;
  } else {
    *queue = node;
  }
  return 1;
}

netq_t *
netq_head(netq_t **queue) {
  return queue ? *queue : NULL;
}

netq_t *
netq_next(netq_t *p) {
  if (!p)
    return NULL;

  return p->next;
}

void
netq_remove(netq_t **queue, netq_t *item) {
{
  struct netq_t *curr, *prev;

  if (!queue || !item) {
    return;
  }

  if(*queue == NULL) {
    return;
  }

  prev = NULL;
  for(curr = *queue; curr != NULL; curr = curr->next) {
    if(curr == item) {
      if(prev == NULL) {
	/* First on list */
	*queue = curr->next;
      } else {
	/* Not first on list */
	prev->next = curr->next;
      }
      curr->next = NULL;
      return;
    }
    prev = curr;
  }
 }
}

netq_t
*netq_pop_first(netq_t **queue) {
  netq_t *p = netq_head(queue);

  if(p) {
    /* remove the element from the list and update head */
    *queue = p->next;
    p->next = NULL;
  }

  return p;
}

netq_t *
netq_node_new(size_t size) {
  netq_t *node;
  node = netq_malloc_node(size);

  if (!node) {
    LOG_WARN("netq_node_new: malloc\n");
  }

  if (node) {
    memset(node, 0, sizeof(netq_t));
  }

  return node;
}

void
netq_node_free(netq_t *node)
{
  if (node) {
    netq_free_node(node);
  }
}

void
netq_delete_all(netq_t **queue)
{
  netq_t *p, *tmp;
  if (queue) {
    p = *queue;
    while(p) {
      tmp = p->next;
      p->next = NULL;
      netq_free_node(p);
      p = tmp;
    }
    *queue = NULL;
  }
}
