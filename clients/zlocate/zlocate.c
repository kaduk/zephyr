/* This file is part of the Project Athena Zephyr Notification System.
 * It contains code for the "zlocate" command.
 *
 *	Created by:	Robert French
 *
 *	$Source$
 *	$Author$
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#include <zephyr/mit-copyright.h>

#include <zephyr/zephyr.h>
#include <string.h>

#ifndef lint
static char rcsid_zlocate_c[] = "$Header$";
#endif lint

main(argc,argv)
	int argc;
	char *argv[];
{
	int retval,numlocs,i,one,ourargc;
	char *whoami,bfr[BUFSIZ],user[BUFSIZ];
	ZLocations_t locations;
	
	whoami = argv[0];

	if (argc < 2) {
		printf("Usage: %s user ... \n",whoami);
		exit(1);
	}
	
	if ((retval = ZInitialize()) != ZERR_NONE) {
		com_err(whoami,retval,"while initializing");
		exit(1);
	} 

	argv++;
	argc--;

	one = 1;
	
	ourargc = argc;
	
	for (;argc--;argv++) {
		(void) strcpy(user,*argv);
		if (!index(user,'@')) {
			(void) strcat(user,"@");
			(void) strcat(user,ZGetRealm());
		} 
		if ((retval = ZLocateUser(user,&numlocs)) != ZERR_NONE) {
			(void) sprintf(bfr,"while locating user %s",user);
			com_err(whoami,retval,bfr);
			continue;
		}
		if (ourargc > 1)
			printf("\t%s:\n",user);
		if (!numlocs) {
			printf("Hidden or not logged-in\n");
			if (argc)
				printf("\n");
			continue;
		}
		for (i=0;i<numlocs;i++) {
			if ((retval = ZGetLocations(&locations,&one))
			    != ZERR_NONE) {
				com_err(whoami,retval,
					"while getting location");
				continue;
			}
			if (one != 1) {
				printf("%s: internal failure while getting location\n",whoami);
				exit(1);
			}
			print_pad(locations.host,43);
			print_pad(locations.tty,8);
			printf("%s\n",locations.time);
		}
		if (argc)
			printf("\n");
		(void) ZFlushLocations();
	}
}

print_pad(s,n)
	char *s;
	int n;
{
	char bfr[81];
	int num;

	(void) strcpy(bfr,"                                                                               ");
	
	if (strlen(s) < n)
		num = n-strlen(s);
	else
		num = 0;

	bfr[num] = '\0';
	printf("%s%s",s,bfr);
}
