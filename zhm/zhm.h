#ifndef __HM_H__
#define __HM_H__
/* This file is part of the Project Athena Zephyr Notification System.
 * It contains the hostmanager header file.
 *
 *      Created by:     David C. Jedlinsky
 *
 *      $Source$
 *      $Author$
 *      $Zephyr: /mit/zephyr/src.rw/zhm/RCS/zhm.h,v 1.13 90/10/19 07:11:48 raeburn Exp $
 *
 *      Copyright (c) 1987, 1991 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h". 
 */

#include <zephyr/mit-copyright.h>
#include <internal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "timer.h"

/* These macros are for insertion into and deletion from a singly-linked list
 * with back pointers to the previous element's next pointer.  In order to
 * make these macros act like expressions, they use the comma operator for
 * sequenced evaluations of assignment, and "a && b" for "evaluate assignment
 * b if expression a is true". */
#define LIST_INSERT(head, elem) \
	((elem)->next = *(head), \
	 (*head) && ((*(head))->prev_p = &(elem)->next), \
	 (*head) = (elem), (elem)->prev_p = (head))
#define LIST_DELETE(elem) \
	(*(elem)->prev_p = (elem)->next, \
	 (elem)->next && ((elem)->next->prev_p = (elem)->prev_p))

#ifdef DEBUG
#define DPR(a) fprintf(stderr, a); fflush(stderr)
#define DPR2(a,b) fprintf(stderr, a, b); fflush(stderr)
#define Zperr(e) fprintf(stderr, "Error = %d\n", e)
#else
#define DPR(a)
#define DPR2(a,b)
#define Zperr(e)
#endif

#define BOOT_TIMEOUT 10
#define DEAD_TIMEOUT 5*60

typedef struct _Queue {
    struct _realm_info *ri;
    Timer *timer;
    int retries;
    ZNotice_t notice;
    caddr_t packet;
    struct sockaddr_in reply;
    struct _Queue *next, **prev_p;
} Queue;

typedef enum _realm_state {
   NEED_SERVER, /* never had a server, HM_BOOT when we find one.  This can
		   also be set if a flush was requested when the state
		   was !=  ATTACHED. */
   DEAD_SERVER, /* server timed out, no others around.  This is
		   actually handled in the same way as BOOTING
		   (although some of the timeouts are different), but
		   it's handy to know which of the two states the zhm
		   is in */
   BOOTING, /* waiting for HM_BOOT/HM_ATTACH SERVACK */
   ATTACHED /* active and connected */
} realm_state;

typedef struct _realm_info {
   Z_RealmConfig realm_config;

#define NO_SERVER -1
#define EXCEPTION_SERVER -2
   int current_server;
   struct sockaddr_in sin;
   realm_state state;

   int nchange;
   int nsrvpkts;
   int ncltpkts;

   Queue *queue;
   Timer *boot_timer;
} realm_info;

/* queue.c */
void init_realm_queue __P((realm_info *));
Code_t add_notice_to_realm __P((realm_info *, ZNotice_t *, char *,
				struct sockaddr_in *, int));
Code_t remove_notice_from_realm __P((realm_info *, ZNotice_t *,
				     ZNotice_Kind_t *, struct sockaddr_in *));
void retransmit_realm __P((realm_info *));
void disable_realm_retransmits __P((realm_info *));
int realm_queue_len __P((realm_info *));

/* zhm.c */
extern realm_info *realm_list;
extern int nrealms;

/* zhm_client.c */
void transmission_tower __P((ZNotice_t *, struct sockaddr_in *, char *, int));
Code_t send_outgoing __P((struct sockaddr_in *, ZNotice_t *));

/* zhm_server.c */
void server_manager __P((ZNotice_t *, struct sockaddr_in *));
void hm_control __P((realm_info *, ZNotice_t *));
void realm_new_server __P((realm_info *, struct in_addr *addr));
void realm_flush __P((realm_info *));
void realm_reset __P((realm_info *));


#ifdef vax
#define use_etext
#endif /* vax */

#ifdef ibm032
#define adjust_size(size)	size -= 0x10000000
#endif /* ibm032 */

#if defined(sun) && (defined (SUN4_ARCH) || defined (sparc))
#define use_etext
#endif

#ifdef _AIX
#ifdef i386
#define adjust_size(size)	size -= 0x400000
#endif
#ifdef _IBMR2
#define	adjust_size(size)	size -= 0x20000000
#endif
#endif

#if (defined(ultrix) || defined(sgi)) && defined(mips)
#define adjust_size(size)	size -= 0x10000000
#endif /* (ultrix || sgi) && mips */

#if defined(__alpha)
#define adjust_size(size)	size -= 0x140000000
#endif /* alpha */

#ifdef use_etext
extern int etext;
#define adjust_size(size)	size -= (unsigned int) &etext;
#undef use_etext
#endif

#endif
