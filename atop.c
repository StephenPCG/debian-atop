/*
** ATOP - System & Process Monitor
**
** The program 'atop' offers the possibility to view the activity of 
** the system on system-level as well as process-level.
**
** This source-file contains the main-function, which verifies the
** calling-parameters and takes care of initialization. 
** The engine-function drives the main sample-loop in which after the
** indicated interval-time a snapshot is taken of the system-level and
** process-level counters and the deviations are calculated and
** visualized for the user.
** ==========================================================================
** Author:      Gerlof Langeveld - AT Computing, Nijmegen, Holland
** E-mail:      gerlof@ATComputing.nl
** Date:        November 1996
** Linux-port:  June 2000
** Modified: 	May 2001 - Ported to kernel 2.4
** --------------------------------------------------------------------------
** Copyright (C) 2000-2005 Gerlof Langeveld
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
** --------------------------------------------------------------------------
**
** After initialization, the main-function calls the ENGINE.
** For every cycle (so after another interval) the ENGINE calls various 
** functions as shown below:
**
** +---------------------------------------------------------------------+
** |                           E  N  G  I  N  E                          |
** |                                                                     |
** |                                                                     |
** |    _____________________await interval-timer_____________________   |
** |   |                                                              ^  |
** |   |      ________       ________      ________      ________     |  |
** |   |     ^        |     ^        |    ^        |    ^        |    |  |
** +---|-----|--------|-----|--------|----|--------|----|--------|----|--+
**     |     |        |     |        |    |        |    |        |    |
**  +--V-----|--+  +--V-----|--+  +--V----|--+  +--V----|--+  +--V----|-+  
**  |           |  |           |  |          |  |          |  |         |
**  | photosyst |  | photoproc |  |   acct   |  | deviate  |  |  print  |
**  |           |  |           |  |photoproc |  |  ...syst |  |         |
**  |           |  |           |  |          |  |  ...proc |  |         |
**  +-----------+  +-----------+  +----------+  +----------+  +---------+  
**        ^              ^             ^              ^            |
**        |              |             |              |            |
**        |              |             |              V            V 
**      ______       _________     __________     ________     _________
**     /      \     /         \   /          \   /        \   /         \
**      /proc          /proc       accounting     process-     screen or
**                                    file        database        file
**     \______/     \_________/   \__________/   \________/   \_________/
**
**    -	photosyst()
**	Takes a snapshot of the counters related to resource-usage on
** 	system-level (cpu, disk, memory, network).
**	This code is UNIX-flavor dependent; in case of Linux the counters
**	are retrieved from /proc.
**
**    -	photoproc()
**	Takes a snapshot of the counters related to resource-usage of
**	processes which are currently active. For this purpose the whole
**	task-list is read.
**	This code is UNIX-flavor dependent; in case of Linux the counters
**	are retrieved from /proc.
**
**    -	acctphotoproc()
**	Takes a snapshot of the counters related to resource-usage of
**	processes which have been finished during the last interval.
**	For this purpose all new records in the accounting-file are read.
**
** When all counters have been gathered, functions are called to calculate
** the difference between the current counter-values and the counter-values
** of the previous cycle. These functions operate on the system-level
** as well as on the process-level counters. 
** These differences are stored in a new structure(-table). 
**
**    -	deviatsyst()
**	Calculates the differences between the current system-level
** 	counters and the corresponding counters of the previous cycle.
**
**    -	deviatproc()
**	Calculates the differences between the current process-level
** 	counters and the corresponding counters of the previous cycle.
**	The per-process counters of the previous cycle are stored in the
**	process-database; this "database" is implemented as a linked list
**	of process-info structures in memory (so no disk-accesses needed).
**	Within this linked list hash-buckets are maintained for fast searches.
**	The entire process-database is handled via a set of well-defined 
** 	functions from which the name starts with "pdb_..." (see the
**	source-file procdbase.c).
**	The processes which have been finished during the last cycle
** 	are also treated by deviatproc() in order to calculate what their
**	resource-usage was before they finished.
**
** All information is ready to be visualized now.
** There is a structure which holds the start-address of the
** visualization-function to be called. Initially this structure contains
** the address of the generic visualization-function ("generic_samp"), but
** these addresses can be modified in the main-function depending on particular
** flags. In this way various representation-layers (ASCII, graphical, ...)
** can be linked with 'atop'; the one to use can eventually be chosen
** at runtime. 
**
** $Log: atop.c,v $
** Revision 1.32  2008/01/07 10:16:13  gerlof
** Implement summaries for atopsar.
**
** Revision 1.31  2007/11/06 09:16:05  gerlof
** Add keyword atopsarflags to configuration-file ~/.atoprc
**
** Revision 1.30  2007/08/16 11:58:35  gerlof
** Add support for atopsar reporting.
**
** Revision 1.29  2007/03/20 13:01:36  gerlof
** Introduction of variable supportflags.
**
** Revision 1.28  2007/03/20 12:13:00  gerlof
** Be sure that all pstat struct's are initialized with binary zeroes.
**
** Revision 1.27  2007/02/19 11:55:04  gerlof
** Bug-fix: flag -S was not recognized any more.
**
** Revision 1.26  2007/02/13 10:34:20  gerlof
** Support parseable output with flag -P
**
** Revision 1.25  2007/01/26 12:10:40  gerlof
** Add configuration-value 'swoutcritsec'.
**
** Revision 1.24  2007/01/18 10:29:22  gerlof
** Improved syntax-checking for ~/.atoprc file.
** Support for network-interface busy-percentage.
**
** Revision 1.23  2006/02/07 08:27:04  gerlof
** Cosmetic changes.
**
** Revision 1.22  2005/10/28 09:50:29  gerlof
** All flags/subcommands are defined as macro's.
**
** Revision 1.21  2005/10/21 09:48:48  gerlof
** Per-user accumulation of resource consumption.
**
** Revision 1.20  2004/12/14 15:05:38  gerlof
** Implementation of patch-recognition for disk and network-statistics.
**
** Revision 1.19  2004/10/26 13:42:49  gerlof
** Also lock current physical pages in memory.
**
** Revision 1.18  2004/09/15 08:23:42  gerlof
** Set resource limit for locked memory to infinite, because
** in certain environments it is set to 32K (causes atop-malloc's
** to fail).
**
** Revision 1.17  2004/05/06 09:45:44  gerlof
** Ported to kernel-version 2.6.
**
** Revision 1.16  2003/07/07 09:18:22  gerlof
** Cleanup code (-Wall proof).
**
** Revision 1.15  2003/07/03 11:16:14  gerlof
** Implemented subcommand `r' (reset).
**
** Revision 1.14  2003/06/30 11:29:12  gerlof
** Handle configuration file ~/.atoprc
**
** Revision 1.13  2003/01/14 09:01:10  gerlof
** Explicit clearing of malloced space for exited processes.
**
** Revision 1.12  2002/10/30 13:44:51  gerlof
** Generate notification for statistics since boot.
**
** Revision 1.11  2002/10/08 11:34:52  gerlof
** Modified storage of raw filename.
**
** Revision 1.10  2002/09/26 13:51:47  gerlof
** Limit header lines by not showing disks.
**
** Revision 1.9  2002/09/17 10:42:00  gerlof
** Copy functions rawread() and rawwrite() to separate source-file rawlog.c
**
** Revision 1.8  2002/08/30 07:49:35  gerlof
** Implement possibility to store and retrieve atop-data in raw format.
**
** Revision 1.7  2002/08/27 12:09:16  gerlof
** Allow raw data file to be written and to be read (with compression).
**
** Revision 1.6  2002/07/24 11:12:07  gerlof
** Redesigned to ease porting to other UNIX-platforms.
**
** Revision 1.5  2002/07/11 09:15:53  root
** *** empty log message ***
**
** Revision 1.4  2002/07/08 09:20:45  root
** Bug solution: flag list overflow.
**
** Revision 1.3  2001/11/07 09:17:41  gerlof
** Use /proc instead of /dev/kmem for process-level statistics.
**
** Revision 1.2  2001/10/04 13:03:15  gerlof
** Separate kopen() function called i.s.o. implicit with first kmem-read
**
** Revision 1.1  2001/10/02 10:43:19  gerlof
** Initial revision
**
*/

static const char rcsid[] = "$Id: atop.c,v 1.32 2008/01/07 10:16:13 gerlof Exp $";

#include <sys/types.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/utsname.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <regex.h>

#include "atop.h"
#include "acctproc.h"
#include "ifprop.h"
#include "photoproc.h"
#include "photosyst.h"
#include "showgeneric.h"
#include "parseable.h"

#define	allflags  "ab:cde:fghijklmnopqrstuvwxyz1ABCDEFGHIJKLMNOP:QRSTUVWXYZ"
#define	PROCCHUNK	50	/* process-entries for future expansion  */
#define	MAXFL		64      /* maximum number of command-line flags  */

/*
** declaration of global variables
*/
struct utsname	utsname;
time_t 		pretime;	/* timing info				*/
time_t 		curtime;	/* timing info				*/
unsigned long	interval = 10;
unsigned long 	sampcnt;
char		screen;
char		acctactive;	/* accounting active (boolean)		*/
char		rawname[RAWNAMESZ];
char		rawreadflag;
char		flaglist[MAXFL];
char		deviatonly  = 1;
unsigned short	hertz;
unsigned int	pagesize;
int 		osrel;
int 		osvers;
int 		ossub;

int		supportflags;	/* supported features             	*/

struct visualize vis = {generic_samp, generic_error,
			generic_end,  generic_usage};

/*
** argument values
*/
static char		awaittrigger;	/* boolean: awaiting trigger */
static unsigned int 	nsamples = 0xffffffff;
static char		midnightflag;
static unsigned int	begintime, endtime;

/*
** interpretation of defaults-file $HOME/.atop
*/
void do_flags(char *);
void do_interval(char *);
void do_username(char *);
void do_procname(char *);
void do_maxcpu(char *);
void do_maxdisk(char *);
void do_maxintf(char *);
void do_cpucritperc(char *);
void do_memcritperc(char *);
void do_swpcritperc(char *);
void do_dskcritperc(char *);
void do_netcritperc(char *);
void do_swoutcritsec(char *);
void do_almostcrit(char *);
void do_atopsarflags(char *);

static struct {
	char	*tag;
	void	(*func)(char *);
} manrc[] = {
	{	"flags",		do_flags		},
	{	"interval",		do_interval		},
	{	"username",		do_username		},
	{	"procname",		do_procname		},
	{	"maxlinecpu",		do_maxcpu		},
	{	"maxlinedisk",		do_maxdisk		},
	{	"maxlineintf",		do_maxintf		},
	{	"cpucritperc",		do_cpucritperc		},
	{	"memcritperc",		do_memcritperc		},
	{	"swpcritperc",		do_swpcritperc		},
	{	"dskcritperc",		do_dskcritperc		},
	{	"netcritperc",		do_netcritperc		},
	{	"swoutcritsec",		do_swoutcritsec		},
	{	"almostcrit",		do_almostcrit		},
	{	"atopsarflags",		do_atopsarflags		},
};

/*
** internal prototypes
*/
static void	engine(void);

int
main(int argc, char *argv[])
{
	register int	i;
	int		c, nr, line=0;
	char		*p;
	struct rlimit	rlim;

	/*
	** check if defaults-file $HOME/.atoprc present;
	** if so, set defaults specified in this file
	*/
	if ( (p = getenv("HOME")) )
	{
		char path[1024];

		snprintf(path, sizeof path, "%s/.atoprc", p);

		/*
		** check if this file is readable with the user's
		** *real uid/gid* with syscall access()
		*/
		if ( access(path, R_OK) == 0)
		{
			FILE	*fp;
			char	linebuf[256], tagname[16], tagvalue[16];

			fp = fopen(path, "r");

			while ( fgets(linebuf, sizeof linebuf, fp) )
			{
				line++;

				nr = sscanf(linebuf, "%15s %15s",
							tagname, tagvalue);

				switch (nr)
				{
				   case 0:
					continue;

				   case 1:
					if (tagname[0] == '#')
						continue;

					fprintf(stderr,
						"~/.atoprc: syntax error line "
						"%d (no value specified)\n",
						line);

					cleanstop(1);
					break;		/* not reached */

				   default:
					if (tagname[0] == '#')
						continue;
					
					if (tagvalue[0] != '#')
						break;

					fprintf(stderr,
						"~/.atoprc: syntax error line "
						"%d (no value specified)\n",
						line);

					cleanstop(1);
				}

				/*
				** tag name and tag value found
				** try to recognize tag name
				*/
				for (i=0; i < sizeof manrc/sizeof manrc[0]; i++)
				{
					if ( strcmp(tagname, manrc[i].tag) ==0)
					{
						manrc[i].func(tagvalue);
						break;
					}
				}

				/*
				** tag name not recognized
				*/
				if (i == sizeof manrc/sizeof manrc[0])
				{
					fprintf(stderr,
						"~/.atoprc: syntax error line "
						"%d (tag name %s not valid)\n",
						line, tagname);

					cleanstop(1);
				}
			}

			fclose(fp);
		}
	}

	/*
	** check if we are supposed to behave as 'atopsar'
	** i.e. system statistics only
	*/
	if ( (p = strrchr(argv[0], '/')))
		p++;
	else
		p = argv[0];

	if ( strcmp(p, "atopsar") == 0)
		return atopsar(argc, argv);


	/* 
	** interpret command-line arguments & flags 
	*/
	if (argc > 1)
	{
		/* 
		** gather all flags for visualization-functions
		**
		** generic flags will be handled here;
		** unrecognized flags are passed to the print-routines
		*/
		i = 0;

		while (i < MAXFL-1 && (c=getopt(argc, argv, allflags)) != EOF)
		{
			switch (c)
			{
			   case '?':		/* usage wanted ?             */
				prusage(argv[0]);
				break;

			   case 'w':		/* writing of raw data ?      */
				if (optind >= argc)
					prusage(argv[0]);

				strncpy(rawname, argv[optind++], RAWNAMESZ-1);
				vis.show_samp = rawwrite;
				break;

			   case 'r':		/* reading of raw data ?      */
				if (optind < argc && *(argv[optind]) != '-')
					strncpy(rawname, argv[optind++],
							RAWNAMESZ-1);

				rawreadflag++;
				break;

			   case 'S':		/* midnight limit ?           */
				midnightflag++;
				break;

                           case 'a':		/* all processes per sample ? */
				deviatonly=0;
				break;

                           case 'b':		/* begin time ?               */
				if ( !hhmm2secs(optarg, &begintime) )
					prusage(argv[0]);
				break;

                           case 'e':		/* end   time ?               */
				if ( !hhmm2secs(optarg, &endtime) )
					prusage(argv[0]);
				break;

                           case 'P':		/* parseable output?          */
				if ( !parsedef(optarg) )
					prusage(argv[0]);

				vis.show_samp = parseout;
				break;

			   default:		/* gather other flags */
				flaglist[i++] = c;
			}
		}

		/*
		** get optional interval-value and optional number of samples	
		*/
		if (optind < argc && optind < MAXFL)
		{
			if (!numeric(argv[optind]))
				prusage(argv[0]);
	
			interval = atoi(argv[optind]);
	
			optind++;
	
			if (optind < argc)
			{
				if (!numeric(argv[optind]) )
					prusage(argv[0]);

				if ( (nsamples = atoi(argv[optind])) < 1)
					prusage(argv[0]);
			}
		}
	}

	/*
	** determine the name of this node (without domain-name)
	** and the kernel-version
	*/
	(void) uname(&utsname);

	if ( (p = strchr(utsname.nodename, '.')) )
		*p = '\0';

	sscanf(utsname.release, "%d.%d.%d", &osrel, &osvers, &ossub);

	/*
	** determine the clock rate and memory page size for this machine
	*/
	hertz		= sysconf(_SC_CLK_TCK);
	pagesize	= sysconf(_SC_PAGESIZE);

	/*
	** check if raw data from a file must be viewed
	*/
	if (rawreadflag)
	{
		seteuid( getuid() );
		rawread(begintime, endtime);
		cleanstop(0);
	}

	/*
	** determine start-time for gathering current statistics
	*/
	curtime = getboot();

	/*
	** lock ATOP in memory to get reliable samples (also when
	** memory is low);
	** ignored if not running under superuser priviliges!
	*/
	rlim.rlim_cur	= RLIM_INFINITY;
	rlim.rlim_max	= RLIM_INFINITY;
	(void) setrlimit(RLIMIT_MEMLOCK, &rlim);

	(void) mlockall(MCL_CURRENT|MCL_FUTURE);

	/*
	** increment CPU scheduling-priority to get reliable samples (also
	** during heavy CPU load);
	** ignored if not running under superuser priviliges!
	*/
	(void) nice(-20);

	/*
	** catch signals for proper close-down
	*/
	signal(SIGHUP,  cleanstop);
	signal(SIGTERM, cleanstop);

	/*
	** switch-on the process-accounting mechanism to register the
	** (remaining) resource-usage by processes which have finished
	*/
	acctswon();

	/*
	** determine properties (like speed) of all interfaces
	*/
	initifprop();

	/*
	** since priviliged activities are finished now, there is no
	** need to keep running under root-priviliges, so switch
	** effective user-id to real user-id
	*/
	seteuid ( getuid() );

	/*
	** start the engine now .....
	*/
	engine();

	cleanstop(0);

	return 0;	/* never reached */
}

/*
** The engine() drives the main-loop of the program
*/
static void
engine(void)
{
	struct sigaction 	sigact;
	static time_t		timelimit;
	void			getusr1(int);

	/*
	** reserve space for system-level statistics
	*/
	static struct sstat	*cursstat; /* current   */
	static struct sstat	*presstat; /* previous  */
	static struct sstat	*devsstat; /* deviation */
	static struct sstat	*hlpsstat;

	/*
	** reserve space for process-level statistics
	*/
	static struct pstat	*curpact;	/* current active list  */
	static int		curplen;	/* current active size  */

	struct pstat		*curpexit;	/* exitted process list	*/
	struct pstat		*devpstat;	/* deviation list	*/

	int			npresent, nexit, n, nzombie;

	/*
	** initialization: allocate required memory dynamically
	*/
	cursstat = calloc(1, sizeof(struct sstat));
	presstat = calloc(1, sizeof(struct sstat));
	devsstat = calloc(1, sizeof(struct sstat));

	curplen = countprocs() + PROCCHUNK;
	curpact = calloc(curplen, sizeof(struct pstat));

	if (!cursstat || !presstat || !devsstat || !curpact)
	{
		fprintf(stderr, "unexpected calloc-failure...\n");
		cleanstop(1);
	}

	/*
	** install the signal-handler for ALARM and SIGUSR1 (both triggers
	* for the next sample)
	*/
	memset(&sigact, 0, sizeof sigact);
	sigact.sa_handler = getusr1;
	sigaction(SIGUSR1, &sigact, (struct sigaction *)0);

	memset(&sigact, 0, sizeof sigact);
	sigact.sa_handler = getalarm;
	sigaction(SIGALRM, &sigact, (struct sigaction *)0);

	if (interval > 0)
		alarm(interval);

	if (midnightflag)
	{
		time_t		timenow = time(0);
		struct tm	*tp = localtime(&timenow);

		tp->tm_hour = 23;
		tp->tm_min  = 59;
		tp->tm_sec  = 59;

		timelimit = mktime(tp);
	}

	/*
	** MAIN-LOOP:
	**    -	Wait for the requested number of seconds or for other trigger
	**
	**    -	System-level counters
	**		get current counters
	**		calculate the differences with the previous sample
	**
	**    -	Process-level counters
	**		get current counters from running & exited processes
	**		calculate the differences with the previous sample
	**
	**    -	Call the print-function to visualize the differences
	*/
	for (sampcnt=0; sampcnt < nsamples; sampcnt++)
	{
		char	lastcmd;

		/*
		** if the limit-flag is specified:
		**  check if the next sample is expected before midnight;
		**  if not, stop atop now 
		*/
		if (midnightflag && (curtime+interval) > timelimit)
			break;

		/*
		** wait for alarm-signal to arrive (except first sample)
		** or wait for SIGUSR1 in case of an interval of 0.
		*/
		if (sampcnt > 0 && awaittrigger)
			pause();

		awaittrigger = 1;

		/*
		** gather time info for this sample
		*/
		pretime  = curtime;
		curtime  = time(0);		/* seconds since 1-1-1970 */

		/*
		** take a snapshot of the current system-level statistics 
		** and calculate the deviations (i.e. calculate the activity
		** during the last sample)
		*/
		hlpsstat = cursstat;	/* swap current/prev. stats */
		cursstat = presstat;
		presstat = hlpsstat;

		photosyst(cursstat);	/* obtain new counters      */

		deviatsyst(cursstat, presstat, devsstat);

		/*
		** take a snapshot of the current process-level statistics 
		** and calculate the deviations (i.e. calculate the activity
		** during the last sample)
		**
		** first register active processes
		**  --> atop malloc's a minimal amount of space which is
		**      only extended when needed
		*/
		memset(curpact, 0, curplen * sizeof(struct pstat));

		while ( (npresent = photoproc(curpact, curplen)) == curplen)
		{
			curplen = countprocs() + PROCCHUNK;

			curpact = realloc(curpact,
			                   curplen * sizeof(struct pstat));

			memset(curpact, 0, curplen * sizeof(struct pstat));
		}

		/*
		** register processes which exited during last sample;
		** first determine how many processes exited and
		** reserve space for them, and secondly obtain the info
		*/
		nexit = acctprocnt();	/* number of exited processes */

		if (nexit > 0)	
		{
			curpexit = malloc(  nexit * sizeof(struct pstat));
			memset(curpexit, 0, nexit * sizeof(struct pstat));

			acctphotoproc(curpexit, nexit);
		}
		else
			curpexit = NULL;

		/*
		** calculate deviations
		*/
		devpstat = malloc((npresent+nexit) * sizeof(struct pstat));

		n = deviatproc(curpact, npresent, curpexit, nexit,
					deviatonly, devpstat, &nzombie);

		/*
		** activate the installed print-function to visualize
		** the deviations
		*/
		lastcmd = (vis.show_samp)( curtime,
				     curtime-pretime > 0 ? curtime-pretime : 1,
		           	     devsstat, devpstat, n, npresent,
		           	     nzombie, nexit, sampcnt==0);

		/*
		** release dynamically allocated memory
		*/
		if (nexit > 0)
			free(curpexit);

		free(devpstat);

		if (lastcmd == 'r')	/* reset requested ? */
		{
			sampcnt = -1;

			curtime = getboot();	/* reset current time */

			/* set current (will be 'previous') counters to 0 */
			memset(cursstat, 0,           sizeof(struct sstat));
			memset(curpact,  0, curplen * sizeof(struct pstat));

			/* remove all processes in process database */
			pdb_makeresidue();
			pdb_cleanresidue();
		}
	} /* end of main-loop */
}

/*
** print usage of this command
*/
void
prusage(char *myname)
{
	printf("Usage: %s [-flags] [interval [samples]]\n",
					myname);
	printf("\t\tor\n");
	printf("Usage: %s -w  file  [-S] [-%c] [interval [samples]]\n",
					myname, MALLPROC);
	printf("       %s -r [file] [-b hh:mm] [-e hh:mm] [-flags]\n",
					myname);
	printf("\n");
	printf("\tgeneric flags:\n");
	printf("\t  -%c  show or log all processes (i.s.o. active processes "
	                "only)\n", MALLPROC);
	printf("\t  -P  generate parseable output for specified label(s)\n");

	(*vis.show_usage)();

	printf("\n");
	printf("\tspecific flags for raw logfiles:\n");
	printf("\t  -w  write raw data to   file (compressed)\n");
	printf("\t  -r  read  raw data from file (compressed)\n");
	printf("\t  -S  finish atop automatically before midnight "
	                "(i.s.o. #samples)\n");
	printf("\t  -b  begin showing data from specified time\n");
	printf("\t  -e  finish showing data after specified time\n");
	printf("\n");
	printf("\tinterval: number of seconds   (minimum 0)\n");
	printf("\tsamples:  number of intervals (minimum 1)\n");
	printf("\n");
	printf("If the interval-value is zero, a new sample can be\n");
	printf("forced manually by sending signal USR1"
			" (kill -USR1 pid_atop)\n");
	printf("or with the keystroke '%c' in interactive mode.\n", MSAMPNEXT);


	cleanstop(1);
}

/*
** handler for ALRM-signal
*/
void
getalarm(int sig)
{
	awaittrigger=0;

	if (interval > 0)
		alarm(interval);	/* restart the timer */
}

/*
** handler for USR1-signal
*/
void
getusr1(int sig)
{
	awaittrigger=0;
}

/*
** functions to handle a particular tag in the .atoprc file
*/
void
do_interval(char *val)
{
	if (numeric(val))
	{
		interval = atoi(val);
	}
	else
	{
		fprintf(stderr, ".atoprc: interval value not numeric\n");
		exit(1);
	}
}
