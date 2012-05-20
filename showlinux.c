/*
** ATOP - System & Process Monitor 
**
** The program 'atop' offers the possibility to view the activity of
** the system on system-level as well as process-level.
**
** This source-file contains the Linux-specific functions to calculate
** figures to be visualized.
** ==========================================================================
** Author:      Gerlof Langeveld - AT Computing, Nijmegen, Holland
** E-mail:      gerlof@ATComputing.nl
** Date:        July 2002
** --------------------------------------------------------------------------
** Copyright (C) 2000-2007 Gerlof Langeveld
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
** $Log: showlinux.c,v $
** Revision 1.49  2008/03/06 08:38:28  gerlof
** Register/show ppid of a process.
**
** Revision 1.48  2008/01/18 07:37:05  gerlof
** Show information about the state of the individual threads
** in the scheduling report shown with keystroke 's'.
**
** Revision 1.47  2008/01/07 11:34:18  gerlof
** Correct the sort-order of network-interfaces (on busy-percentage).
**
** Revision 1.46  2007/11/07 09:23:29  gerlof
** Modified format for avg1, avg5 and avg15 (CPL) when counters too large.
**
** Revision 1.45  2007/11/05 11:43:25  gerlof
** Bug-solution for new-process indicator on 64-bits machines.
**
** Revision 1.44  2007/11/05 10:57:56  gerlof
** Bug-solution for huge exit code on 64-bits machines.
**
** Revision 1.43  2007/08/17 09:45:57  gerlof
** Experimental: gather info about HTTP statistics.
**
** Revision 1.42  2007/08/16 12:02:04  gerlof
** Add support for atopsar reporting.
** Concerns modification of networking-counters.
**
** Revision 1.41  2007/07/04 10:18:16  gerlof
** Bug-solution for division by zero.
**
** Revision 1.40  2007/07/03 09:02:29  gerlof
** Support Apache-statistics.
**
** Revision 1.39  2007/03/22 10:12:54  gerlof
** Support for io counters (>= kernel 2.6.20).
**
** Revision 1.38  2007/03/21 14:22:24  gerlof
** Handle io counters maintained from 2.6.20
**
** Revision 1.37  2007/02/13 10:36:09  gerlof
** Removal of external declarations.
** Use of hertz variable instead of HZ.
**
** Revision 1.36  2007/01/26 12:11:07  gerlof
** Add configuration-value 'swoutcritsec'.
**
** Revision 1.35  2007/01/26 10:25:42  gerlof
** Introduce steal percentage for virtual machines.
** Correct bug: when one interface is colored all subsequent interfaces
**              are colored.
**
** Revision 1.34  2007/01/18 10:58:45  gerlof
** Only check for committed limit if it is not zero.
**
** Revision 1.33  2007/01/18 10:37:09  gerlof
** Add support for colors.
** Add support for automatic determination of most critical resource.
** Add support for parsing of new arguments in ~/.atoprc
**
** Revision 1.32  2006/11/13 13:48:46  gerlof
** Implement load-average counters, context-switches and interrupts.
**
** Revision 1.31  2006/02/07 08:38:49  gerlof
** Swapped the zombie counter and exit counter in the PRC-line.
**
** Revision 1.30  2006/02/07 08:30:07  gerlof
** Add possibility to show counters per second.
** Ease parsing of output-lines by fixed number of columns per line.
**
** Revision 1.29  2006/01/30 09:24:12  gerlof
** PRC-line: 'exits' modified to 'exit' to save space.
**
** Revision 1.28  2006/01/30 09:14:26  gerlof
** Extend memory counters (a.o. page scans).
**
** Revision 1.27  2005/11/04 14:16:45  gerlof
** Minor bug-solutions.
**
** Revision 1.26  2005/10/28 09:51:29  gerlof
** All flags/subcommands are defined as macro's.
** Subcommand 'p' has been changed to 'z' (pause).
**
** Revision 1.25  2005/10/21 09:51:11  gerlof
** Per-user accumulation of resource consumption.
**
** Revision 1.24  2004/12/14 15:06:48  gerlof
** Implementation of patch-recognition for disk and network-statistics.
**
** Revision 1.23  2004/10/28 08:31:41  gerlof
** New counter: vm committed space
**
** Revision 1.22  2004/09/24 10:02:46  gerlof
** Wrong cpu-numbers for system level statistics.
**
** Revision 1.21  2004/09/23 08:21:10  gerlof
** Added wait-percentage per cpu.
**
** Revision 1.20  2004/09/23 07:37:34  gerlof
** Consistent handling of CPU percentages on system-level and process-level.
**
** Revision 1.19  2004/09/13 09:20:21  gerlof
** Modify subcommands (former 's' -> 'v', 'v' -> 'V', new 's').
**
** Revision 1.18  2004/09/02 10:55:21  root
** Added sleep-average to process-info.
**
** Revision 1.17  2004/08/31 09:53:31  gerlof
** Show information about underlying threads.
**
** Revision 1.16  2004/06/01 11:58:34  gerlof
** Regular expressions for selections on process-name and user-name.
**
** Revision 1.15  2004/05/06 09:47:59  gerlof
** Ported to kernel-version 2.6.
**
** Revision 1.14  2003/07/07 09:27:34  gerlof
** Cleanup code (-Wall proof).
**
** Revision 1.13  2003/07/03 12:04:25  gerlof
** Minor bug fixes.
**
** Revision 1.12  2003/06/30 11:29:57  gerlof
** Enlarge counters to 'long long'.
**
** Revision 1.11  2003/06/24 06:22:10  gerlof
** Limit number of system resource lines.
**
** Revision 1.10  2003/02/07 10:43:22  gerlof
** Solved a division-by-zero bug for process-percentage.
**
** Revision 1.9  2003/01/24 14:20:57  gerlof
** If possible, also show commandline when process has exited.
**
** Revision 1.8  2003/01/17 07:32:49  gerlof
** Show the full command-line per process (option 'c').
**
** Revision 1.7  2002/10/04 10:05:54  gerlof
** Bug-solution: New process indicator in static output set when needed.
**
** Revision 1.6  2002/10/03 11:14:42  gerlof
** Modify (effective) uid/gid to real uid/gid.
**
** Revision 1.5  2002/09/26 13:52:51  gerlof
** Limit header lines by not showing disks.
** Limit header lines by not showing disks.
**
** Revision 1.4  2002/09/16 08:59:13  gerlof
** Change field EXCODE to STATUS for support of indicator of newly created
** processes.
**
** Revision 1.3  2002/09/02 08:42:44  gerlof
** Bug-solution: blank line after header when more than 999 screens of
** process-list information.
**
** Revision 1.2  2002/08/30 07:11:20  gerlof
** Minor changes in the header-line of the process list.
**
** Revision 1.1  2002/07/24 11:14:16  gerlof
** Initial revision
**
**
** Initial
**
*/

static const char rcsid[] = "$Id: showlinux.c,v 1.49 2008/03/06 08:38:28 gerlof Exp $";

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>
#include <stdarg.h>
#include <curses.h>
#include <pwd.h>
#include <grp.h>
#include <regex.h>

#include "atop.h"
#include "photoproc.h"
#include "photosyst.h"
#include "showgeneric.h"

/*
** critical percentages for occupation-percentage;
** these defaults can be overruled via the config-file
*/
int	cpubadness = 90;	/* percentage		*/
int	membadness = 90;	/* percentage		*/
int	swpbadness = 80;	/* percentage		*/
int	dskbadness = 70;	/* percentage		*/
int	netbadness = 90;	/* percentage		*/
int	pagbadness = 10;	/* number per second	*/

int	almostcrit = 80;	/* percentage		*/

/*
** table with column headers for sorted process list
*/
static char	*columnhead[] = {
	   		[MSORTCPU]= "CPU", [MSORTMEM]= "MEM",
	   		[MSORTDSK]= "DSK", [MSORTNET]= "NET",
};

/*
** function protoypes
*/
static int	syscolorlabel(char *, int, unsigned int);
static void	syscoloroff  (char);

/*
** format-lines for printing
*/
char 	*prcline   	= "PRC | sys %s | user %s | #proc %s | #zombie %s |"
	       	     	  " #exit %s |\n";
/***************************************************************************/
char	*genprochdr1	= "\n  PID  SYSCPU  USRCPU  VGROW  RGROW RDDSK WRDSK "
		     	  "RNET SNET S %c%s CMD  %4d/%-4d\n";
char 	*genproclina1	= "%5d %s %s %s %s %s %s %s %s %c %3.0lf%% %.14s\n";
char 	*genprocline1	= "%s %s     0K     0K (r&w %s)   -    "
			  "- E %3.0lf%% <%.12s>\n";
char 	*genprocline1p	= "%s %s %s %s %s %s %s %s E %3.0lf%% <%.12s>\n";

char	*genprochdr2	= "\n  PID  SYSCPU  USRCPU  VGROW  RGROW  RDDSK  WRDSK "
		     	  " ST EXC S %c%s CMD  %4d/%-4d\n";
char 	*genproclina2	= "%5d %s %s %s %s %s %s  %c%c   - %c %3.0lf%% "
	                  "%.14s\n";
char 	*genprocline2	= "%s %s     0K     0K      -      -  %c%c %3d "
			  "E %3.0lf%% <%.12s>\n";

char	*genprochdr3	= "\n  PID  SYSCPU  USRCPU  VGROW  RGROW USERNAME THR  "
		     	  "ST EXC  S %c%s CMD  %4d/%-4d\n";
char 	*genproclina3	= "%5d %s %s %s %s %-8.8s %3d  %c%c   -  %c %3.0lf%% "
	                  "%.14s\n";
char 	*genprocline3	= "%s %s     0K     0K %-8.8s   0  %c%c %3d  "
			  "E %3.0lf%% <%.12s>\n";
/***************************************************************************/
char	*memprochdr	= "\n  PID MINFLT MAJFLT      VSTEXT "
		     	  " VSIZE  RSIZE  VGROW  RGROW %c%s CMD  %4d/%-4d\n";
char 	*memproclina	= "%5d %s %s      %s %s %s %s %s %3.0lf%% %.14s\n";
char 	*memprocline	= "%s %s          0K     0K     0K     0K     0K"
			  " %3.0lf%% <%.12s>\n";
char 	*memproclinep	= "%5d %s %s          0K %s %s %s %s %3.0lf%% "
                          "<%.12s>\n";
/***************************************************************************/
char	*dskprochdr1	= "\n  PID  READDISK AVGSIZE TOTSIZE   WRITEDISK "
			  "AVGSIZE TOTSIZE %c%s CMD  %4d/%-4d\n";
char 	*dskproclina1	= "%5d %s %s  %s   %s %s  %s %3.0lf%% %.14s\n";
char 	*dskprocline1	= " (reads&writes              %s)   "
			  "             %3.0lf%% <%.12s>\n";
char 	*dskprocline1p	= "%5d %s %s  %s   %s %s  %s %3.0lf%% <%.12s>\n";

char	*dskprochdr2	= "\n  PID     RDDSK    WRDSK  WRDSK_CANCEL    "
			  "                  %c%s CMD  %4d/%-4d\n";
char 	*dskproclina2	= "%5d    %s   %s        %s                      "
			  "%3.0lf%% %.14s\n";
char 	*dskprocline2	= "        -        -             -       "
			  "               %3.0lf%% <%.12s>\n";
/***************************************************************************/
char	*netprochdr	= "\n  PID TCPRCV AVSZ|TCPSND AVSZ|UDPRCV AVSZ"
			  "|UDPSND AVSZ|RAWRS|%c%s CMD  %4d/%-4d\n";
char 	*netproclina	= "%5d %s %s|%s %s|%s %s|%s %s|%2llu %2llu|"
			  "%3.0lf%% %.14s\n";
char 	*netprocline	= "                                        "
			  "              %3.0lf%% <%.12s>\n";
char 	*netproclinep	= "%5d %s %s|%s %s|%s %s|%s %s|%2llu %2llu|"
			  "%3.0lf%% <%.12s>\n";
/***************************************************************************/
char	*varprochdr	= "\n  PID  PPID USERNAME GROUP      "
			  "STDATE    STTIME  ST EXC  S %c%s CMD  %4d/%-4d\n";
char 	*varproclina	= "%5d %5d %-8.8s %-8.8s %s %s %c%c   -  %c %3.0lf%% "
	                  "%.14s\n";
char 	*varprocline	= "%5d %-8.8s %-8.8s %s %s %c%c %3d  E %3.0lf%% "
			  "<%.12s>\n";
/***************************************************************************/
char	*cmdprochdr	= "\n  PID %c%s COMMAND-LINE                           "
			  "                    %4d/%-4d\n";
char 	*cmdproclina	= "%5d %3.0lf%% %s\n";
char 	*cmdprocline	= "%3.0lf%% <%.66s>\n";
/***************************************************************************/
char	*schprochdr	= "\n  PID TRUN TSLPI TSLPU  POLI NICE PRI RTPR "
			  "CURCPU ST EXC  S %c%s CMD  %4d/%-4d\n";
char 	*schproclina	= "%5d %4d %5d %5d  %-4s %4d %3d %4d %6d %-2s   -  "
			  "%c %3.0lf%% %.14s\n";
char 	*schprocline	= "    0     0     0     -    -   -    -      - "
			  "%c%c %3d  E %3.0lf%% <%.12s>\n";
/***************************************************************************/
char	*totuserhdr	= "\nNPROCS  SYSCPU  USRCPU  VSIZE  RSIZE  RDDSK "
			  "WRDSK RNET SNET %c%s USER %4d/%-4d\n";
char 	*totuserlin	= "%s %s %s %s %s  %s %s %s %s "
	                  "%3.0lf%% %.14s\n";
/***************************************************************************/
char	*totprochdr	= "\nNPROCS  SYSCPU  USRCPU  VSIZE  RSIZE  RDDSK "
			  "WRDSK RNET SNET %c%s CMD  %4d/%-4d\n";
char 	*totproclin	= "%s %s %s %s %s  %s %s %s %s "
	                  "%3.0lf%% %.14s\n";
/***************************************************************************/

/*
** calculate the total consumption on system-level for the 
** four main resources
*/
void
totalcap(struct syscap *psc, struct sstat *sstat, struct pstat *pstat, int nact)
{
	register int	i;

	psc->nrcpu      = sstat->cpu.nrcpu;
	psc->availcpu	= sstat->cpu.all.stime +
			  sstat->cpu.all.utime +
			  sstat->cpu.all.ntime +
			  sstat->cpu.all.itime +
			  sstat->cpu.all.wtime +
			  sstat->cpu.all.Itime +
			  sstat->cpu.all.Stime +
			  sstat->cpu.all.steal;

	psc->availmem	= sstat->mem.physmem * pagesize/1024;

	if (supportflags & PATCHSTAT)
	{
		/*
		** calculate total number of accesses which have been
		** issued by the active processes for disk and for network
		*/
		for (psc->availnet=psc->availdsk=0, i=0; i < nact; i++) 
		{
			psc->availnet += (pstat+i)->net.tcpsnd;
			psc->availnet += (pstat+i)->net.tcprcv;
			psc->availnet += (pstat+i)->net.udpsnd;
			psc->availnet += (pstat+i)->net.udprcv;
			psc->availnet += (pstat+i)->net.rawsnd;
			psc->availnet += (pstat+i)->net.rawrcv;

			psc->availdsk += (pstat+i)->dsk.rio;
			psc->availdsk += (pstat+i)->dsk.wio;
		}
	}
	else
	{
		for (psc->availnet=psc->availdsk=0, i=0; i < nact; i++) 
		{
			psc->availdsk += (pstat+i)->dsk.rsz;
			psc->availdsk += (pstat+i)->dsk.wsz;
		}
	}
}

/*
** calculate cumulative system- and user-time for all active processes
*/
void
pricumproc(struct pstat *pstat, int nact, int nproc, int nzomb, int nexit,
                                int avgval, int nsecs)
{
	int 	i;
	count_t totut, totst;
        char	format1[16], format2[16], format3[16];
	char	format4[16], format5[16];

	for (i=totut=totst=0; i < nact; i++)
	{
		totut += (pstat+i)->cpu.utime;
		totst += (pstat+i)->cpu.stime;
	}

	printg(prcline, val2cpustr(totst * 1000/hertz, format1),
			val2cpustr(totut * 1000/hertz, format2),
			val2valstr(nproc,           format3, 6, 0, 0),
			val2valstr(nzomb,           format4, 4, 0, 0),
			supportflags & ACCTACTIVE ?
			       val2valstr(nexit, format5, 6, avgval, nsecs):
			       "     ?");
}

/*
** print a line of general figures about one process
*/
void
showgenproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	struct passwd 	*pwd;
	char		*username, usname[16], exittype;

	char		format1[16], format2[16];
	char		format3[16], format4[16];
	char		format5[16], format6[16];
	char		format7[16], format8[16];

	if (curstat->gen.state != 'E')
	{
		if (supportflags & PATCHSTAT)
		{
			/*
			** show active processes for patched kernel
			*/
			printg(genproclina1,
			   curstat->gen.pid,
			   val2cpustr(curstat->cpu.stime*1000/hertz, format1),
			   val2cpustr(curstat->cpu.utime*1000/hertz, format2),
			   val2memstr(curstat->mem.vgrow*1024,
						format3, KBFORMAT, 0, 0),
			   val2memstr(curstat->mem.rgrow*1024,
						format4, KBFORMAT, 0, 0),
			   val2valstr(curstat->dsk.rio,     format5, 5,
			                                    avgval, nsecs),
			   val2valstr(curstat->dsk.wio,     format6, 5,
			                                    avgval, nsecs),
			   val2valstr(curstat->net.tcprcv +
			              curstat->net.udprcv +
			              curstat->net.rawrcv,  format7, 4,
			                                    avgval, nsecs),
			   val2valstr(curstat->net.tcpsnd +
			              curstat->net.udpsnd +
			              curstat->net.rawsnd,  format8, 4,
			                                    avgval, nsecs),
			   curstat->gen.state,
			   perc, curstat->gen.name);
		}
		else
		{
			if (supportflags & IOSTAT)
			{
				/*
				** show active processes for kernel with iostats
				*/
				printg(genproclina2,
				   curstat->gen.pid,
				   val2cpustr(curstat->cpu.stime*1000/hertz,
								format1),
				   val2cpustr(curstat->cpu.utime*1000/hertz,
								format2),
				   val2memstr(curstat->mem.vgrow*1024,
						format3, KBFORMAT, 0, 0),
				   val2memstr(curstat->mem.rgrow*1024,
						format4, KBFORMAT, 0, 0),
				   val2memstr(curstat->dsk.rsz*512,
						format5, KBFORMAT, 0, 0),
				   val2memstr(curstat->dsk.wsz*512,
						format6, KBFORMAT, 0, 0),
			   	   curstat->gen.excode & ~(INT_MAX) ?
								'N' : '-',
			   	   '-',
			  	   curstat->gen.state,
			   	   perc, curstat->gen.name);
			}
			else
			{
				/*
				** show active processes for conventional kernel
				**
				** determine the user-name of process
				*/
				if ( (pwd = getpwuid(curstat->gen.ruid)) )
				{
					username = pwd->pw_name;
				}
				else
				{
					snprintf(usname, sizeof usname, "%d",
							curstat->gen.ruid);
					username = usname;
				}
	
				printg(genproclina3,
			   	curstat->gen.pid,
			   	val2cpustr(curstat->cpu.stime*1000/hertz,
								format1),
			   	val2cpustr(curstat->cpu.utime*1000/hertz,
								format2),
			   	val2memstr(curstat->mem.vgrow*1024,
						format3, KBFORMAT, 0, 0),
			   	val2memstr(curstat->mem.rgrow*1024,
						format4, KBFORMAT, 0, 0),
			   	username,
			   	curstat->gen.nthr,
			   	curstat->gen.excode & ~(INT_MAX) ? 'N' : '-',
			   	'-',
			   	curstat->gen.state,
			   	perc, curstat->gen.name);
			}
		}
	}
	else
	{
		/*
		** show exited processes
		*/
		if (curstat->gen.pid == 0)
			printg("    ? ");
		else
			printg("%5d ", curstat->gen.pid);

		if (supportflags & PATCHSTAT)
		{
			/*
			** show exited processes for patched kernel
			*/
			if (supportflags & PATCHACCT)
			{
				/*
				** show exited processes for patched kernel
				** including patches for accounting
				*/
				printg(genprocline1p,
				   val2cpustr(curstat->cpu.stime*1000/hertz,
								format1),
				   val2cpustr(curstat->cpu.utime*1000/hertz,
								format2),
				   val2memstr(curstat->mem.vgrow*1024,
						format3, KBFORMAT, 0, 0),
				   val2memstr(curstat->mem.rgrow*1024,
						format4, KBFORMAT, 0, 0),
				   val2valstr(curstat->dsk.rio,
						format5, 5, avgval, nsecs),
				   val2valstr(curstat->dsk.wio,
						    format6, 5, avgval, nsecs),
				   val2valstr(curstat->net.tcprcv +
				              curstat->net.udprcv +
				              curstat->net.rawrcv,
						 format7, 4, avgval, nsecs),
				   val2valstr(curstat->net.tcpsnd +
				              curstat->net.udpsnd +
				              curstat->net.rawsnd,
						 format8, 4, avgval, nsecs),
				   perc, curstat->gen.name);
			}
			else
			{
				/*
				** show exited processes for patched kernel
				** without patches for accounting
				*/
				printg(genprocline1,
				  val2cpustr(curstat->cpu.stime*1000/hertz,
						format1),
				  val2cpustr(curstat->cpu.utime*1000/hertz,
						format2),
				  val2valstr(curstat->dsk.rio,
						format3, 6, avgval, nsecs),
				  perc, curstat->gen.name);
			}
		}
		else
		{
			if (curstat->gen.excode & 0xff)
			{
				if (curstat->gen.excode & 0x80)
					exittype = 'C';
				else
					exittype = 'S';
			}
			else
			{
				exittype = 'E';
			}
	
			if (supportflags & IOSTAT)
			{
				/*
				** show exited processes for kernel with iostats
				*/
				printg(genprocline2,
			  	  val2cpustr(curstat->cpu.stime*1000/hertz,
								format1),
			  	  val2cpustr(curstat->cpu.utime*1000/hertz,
								format2),
			  	  curstat->gen.excode & ~(INT_MAX) ? 'N' : '-',
			  	  exittype,
		  	  	  curstat->gen.excode & 0xff        ? 
		 		 	 curstat->gen.excode     & 0x7f :
			  		(curstat->gen.excode>>8) & 0xff,
			  	  perc, curstat->gen.name);
			}
			else
			{
				/*
				** show active processes for conventional kernel
				**
				** determine the user-name of process
				*/
				if ( (pwd = getpwuid(curstat->gen.ruid)) )
				{
					username = pwd->pw_name;
				}
				else
				{
					snprintf(usname, sizeof usname, "%d",
							curstat->gen.ruid);
					username = usname;
				}

				printg(genprocline3,
			  	  val2cpustr(curstat->cpu.stime*1000/hertz,
								format1),
			  	  val2cpustr(curstat->cpu.utime*1000/hertz,
								format2),
			  	  username,
			  	  curstat->gen.excode & ~(INT_MAX) ? 'N' : '-',
			  	  exittype,
		  	  	  curstat->gen.excode & 0xff        ? 
				 	 curstat->gen.excode     & 0x7f :
			  		(curstat->gen.excode>>8) & 0xff,
			  	  perc, curstat->gen.name);
			}
		}
	}
}

/*
** print a line of memory-figures about one process
*/
void
showmemproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	char	format1[16], format2[16];
	char	format3[16], format4[16];
	char	format5[16], format6[16];
	char	format7[16];

	if (curstat->gen.state != 'E')
	{
		/*
		** show process-info of active process
		*/
		printg(memproclina,
		  curstat->gen.pid,
		  val2valstr(curstat->mem.minflt,      format1, 6,
			                               avgval, nsecs),
		  val2valstr(curstat->mem.majflt,      format2, 6,
			                               avgval, nsecs),
		  val2memstr(curstat->mem.shtext*1024, format3, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.vmem  *1024, format4, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.rmem  *1024, format5, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.vgrow *1024, format6, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.rgrow *1024, format7, KBFORMAT, 0, 0),
		  perc, curstat->gen.name);
	}
	else
	{
		/*
		** show process-info of exited process
		*/
	   if (supportflags & PATCHACCT)
	   {
		printg(memproclinep,
		  curstat->gen.pid,
		  val2valstr(curstat->mem.minflt,      format1, 6,
			                               avgval, nsecs),
		  val2valstr(curstat->mem.majflt,      format2, 6,
			                               avgval, nsecs),
		  val2memstr(curstat->mem.vmem  *1024, format3, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.rmem  *1024, format4, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.vgrow *1024, format5, KBFORMAT, 0, 0),
		  val2memstr(curstat->mem.rgrow *1024, format6, KBFORMAT, 0, 0),
		  perc, curstat->gen.name);
	   }
	   else
	   {
		if (curstat->gen.pid == 0)
			printg("    ? ");
		else
			printg("%5d ", curstat->gen.pid);

		printg(memprocline,
			  val2valstr(curstat->mem.minflt,     format1, 6,
			                                      avgval, nsecs),
			  val2valstr(curstat->mem.majflt,     format2, 6,
			                                      avgval, nsecs),
			  perc, curstat->gen.name);
	   }
	}
}

/*
** print a line of disk-figures about one process
*/
void
showdskproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	char		format1[16], format2[16], format3[16];
	char		format4[16], format5[16], format6[16];
	unsigned int	avgrsz, avgwsz;

	if (supportflags & PATCHSTAT)
	{
		avgrsz = curstat->dsk.rio ?
		         curstat->dsk.rsz * 512LL / curstat->dsk.rio : 0;
		avgwsz = curstat->dsk.wio ?
		         curstat->dsk.wsz * 512LL / curstat->dsk.wio : 0;

		if (curstat->gen.state != 'E')
		{
			/*
			** show process-info of active process
			*/
			printg(dskproclina1,
			  curstat->gen.pid,
			  val2valstr(curstat->dsk.rio,
					format1, 9, avgval, nsecs),
			  val2valstr(avgrsz, format2, 7, 0, 0),
			  val2memstr(curstat->dsk.rsz*512LL,
					format3, KBFORMAT, avgval, nsecs),
			  val2valstr(curstat->dsk.wio, 
					format4, 9, avgval, nsecs),
			  val2valstr(avgwsz, format5, 7, 0, 0),
			  val2memstr(curstat->dsk.wsz*512LL,
					format6, KBFORMAT, avgval, nsecs),
			  perc, curstat->gen.name);
		}
		else
		{
			/*
			** show process-info of exited process
			*/
		   	if (supportflags & PATCHACCT)
		   	{
				printg(dskprocline1p,
		  		  curstat->gen.pid,
			  	  val2valstr(curstat->dsk.rio,
					format1, 9, avgval, nsecs),
			  	  val2valstr(avgrsz, format2, 7, 0, 0),
			  	  val2memstr(curstat->dsk.rsz * 512LL,	
					format3, KBFORMAT, avgval, nsecs),
			  	  val2valstr(curstat->dsk.wio,
					format4, 9, avgval, nsecs),
			  	  val2valstr(avgwsz, format5, 7, 0, 0),
			  	  val2memstr(curstat->dsk.wsz * 512LL,
					format6, KBFORMAT, avgval, nsecs),
			  	  perc, curstat->gen.name);
		   	}
		   	else
		   	{
				if (curstat->gen.pid == 0)
					printg("    ? ");
				else
					printg("%5d ", curstat->gen.pid);
	
				printg(dskprocline1,
				  val2valstr(curstat->dsk.rio,
					format1, 9, avgval, nsecs),
				  perc, curstat->gen.name);
			}
		}
	}
	else
	{
		if (supportflags & IOSTAT)
		{
			if (curstat->gen.state != 'E')
			{
				/*
				** show process-info of active process
				*/
				printg(dskproclina2,
				  curstat->gen.pid,
				  val2memstr(curstat->dsk.rsz*512LL,
					format1, KBFORMAT, avgval, nsecs),
				  val2memstr(curstat->dsk.wsz*512LL,
					format2, KBFORMAT, avgval, nsecs),
				  val2memstr(curstat->dsk.cwsz*512LL,
					format3, KBFORMAT, avgval, nsecs),
				  perc, curstat->gen.name);
			}
			else
			{
				/*
				** show process-info of exited process
				*/
				if (curstat->gen.pid == 0)
					printg("    ? ");
				else
					printg("%5d ", curstat->gen.pid);

				printg(dskprocline2,
				  perc, curstat->gen.name);
			}
		}
	}
}

/*
** print a line of network-figures about one process
*/
void
shownetproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	char	format1[16], format2[16], format3[16], format4[16];
	char	format5[16], format6[16], format7[16], format8[16];
	int	avgtcpr, avgtcps, avgudpr, avgudps;

	avgtcpr = curstat->net.tcprcv ?
		  curstat->net.tcprsz / curstat->net.tcprcv : 0;
	avgtcps = curstat->net.tcpsnd ?
		  curstat->net.tcpssz / curstat->net.tcpsnd : 0;
	avgudpr = curstat->net.udprcv ?
		  curstat->net.udprsz / curstat->net.udprcv : 0;
	avgudps = curstat->net.udpsnd ?
		  curstat->net.udpssz / curstat->net.udpsnd : 0;

	if (curstat->gen.state != 'E')
	{
		/*
		** show process-info of active process
		*/
		printg(netproclina,
		  curstat->gen.pid,
		  val2valstr(curstat->net.tcprcv,      format1,  6,
			                                      avgval, nsecs),
		  val2valstr(avgtcpr,                  format2,  4, 0, 0),
		  val2valstr(curstat->net.tcpsnd,      format3,  6,
			                                      avgval, nsecs),
		  val2valstr(avgtcps,                  format4,  4, 0, 0),
		  val2valstr(curstat->net.udprcv,      format5,  6,
			                                      avgval, nsecs),
		  val2valstr(avgudpr,                  format6,  4, 0, 0),
		  val2valstr(curstat->net.udpsnd,      format7,  6,
			                                      avgval, nsecs),
		  val2valstr(avgudps,                  format8,  4, 0, 0),
		  curstat->net.rawrcv,
		  curstat->net.rawsnd,
		  perc, curstat->gen.name);
	}
	else
	{
		/*
		** show process-info of exited process
		*/
	   	if (supportflags & PATCHACCT)
	   	{
			printg(netproclinep,
		  	curstat->gen.pid,
		  	val2valstr(curstat->net.tcprcv,      format1, 6,
			                                      avgval, nsecs),
		  	val2valstr(avgtcpr,                  format2, 4, 0, 0),
		  	val2valstr(curstat->net.tcpsnd,      format3, 6,
			                                      avgval, nsecs),
		  	val2valstr(avgtcps,                  format4, 4, 0, 0),
		  	val2valstr(curstat->net.udprcv,      format5, 6,
			                                      avgval, nsecs),
		  	val2valstr(avgudpr,                  format6, 4, 0, 0),
		  	val2valstr(curstat->net.udpsnd,      format7, 6,
			                                      avgval, nsecs),
		  	val2valstr(avgudps,                  format8, 4, 0, 0),
		  	curstat->net.rawrcv,
		  	curstat->net.rawsnd,
		  	perc, curstat->gen.name);
	   	}
	   	else
	   	{
			if (curstat->gen.pid == 0)
				printg("    ? ");
			else
				printg("%5d ", curstat->gen.pid);

			printg(netprocline, perc, curstat->gen.name);
	  	}
	}
}

/*
** print a line of total resource consumption per user
*/
void
showtotuser(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	struct passwd 	*pwd;
	char		numuser[16], *username;

	char		format1[16], format2[16];
	char		format3[16], format4[16];
	char		format5[16], format6[16];
	char		format7[16], format8[16];
	char		format9[16];

	/*
	** determine the user-name of process
	*/
	if ( (pwd = getpwuid(curstat->gen.ruid)) == NULL)
	{
		snprintf(numuser, sizeof numuser, "%u", curstat->gen.ruid);
		username = numuser;
	}
	else
	{
		username = pwd->pw_name;
	}

	printg(totuserlin,
		val2valstr(curstat->gen.pid,           format1, 6, 0, 0),
		val2cpustr(curstat->cpu.stime*1000/hertz, format2),
		val2cpustr(curstat->cpu.utime*1000/hertz, format3),
		val2memstr(curstat->mem.vmem *1024,    format4, KBFORMAT, 0, 0),
		val2memstr(curstat->mem.rmem *1024,    format5, KBFORMAT, 0, 0),
		val2valstr(curstat->dsk.rio,           format6, 5,
								avgval, nsecs),
		val2valstr(curstat->dsk.wio,           format7, 5,
								avgval, nsecs),
	   	val2valstr(curstat->net.tcprcv +
		           curstat->net.udprcv +
			   curstat->net.rawrcv,        format8, 4,
								avgval, nsecs),
		val2valstr(curstat->net.tcpsnd +
			   curstat->net.udpsnd +
			   curstat->net.rawsnd,        format9, 4,
								avgval, nsecs),
		perc, username);
}

/*
** print a line of total resource consumption per program (process name)
*/
void
showtotproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	char		format1[16], format2[16];
	char		format3[16], format4[16];
	char		format5[16], format6[16];
	char		format7[16], format8[16];
	char		format9[16];

	printg(totproclin,
		val2valstr(curstat->gen.pid,           format1, 6, 0, 0),
		val2cpustr(curstat->cpu.stime*1000/hertz, format2),
		val2cpustr(curstat->cpu.utime*1000/hertz, format3),
		val2memstr(curstat->mem.vmem *1024,    format4, KBFORMAT, 0, 0),
		val2memstr(curstat->mem.rmem *1024,    format5, KBFORMAT, 0, 0),
		val2valstr(curstat->dsk.rio,           format6, 5,
								avgval, nsecs),
		val2valstr(curstat->dsk.wio,           format7, 5,
								avgval, nsecs),
	   	val2valstr(curstat->net.tcprcv +
		           curstat->net.udprcv +
			   curstat->net.rawrcv,        format8, 4,
								avgval, nsecs),
		val2valstr(curstat->net.tcpsnd +
			   curstat->net.udpsnd +
			   curstat->net.rawsnd,        format9, 4,
								avgval, nsecs),
		perc, curstat->gen.name);
}

/*
** print a line of (more or less) static info about one process
*/
void
showvarproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	char		format1[16], format2[16];
	struct passwd 	*pwd;
	struct group 	*grp;
	char		*username, *groupname;
	char		usname[16], grname[16];
	char		exittype;

	/*
	** determine the user- and group-name of the process
	*/
	if ( (pwd = getpwuid(curstat->gen.ruid)) )
	{
		username = pwd->pw_name;
	}
	else
	{
		snprintf(usname, sizeof usname, "%d", curstat->gen.ruid);
		username = usname;
	}

	if ( (grp = getgrgid(curstat->gen.rgid)) )
	{
		groupname = grp->gr_name;
	}
	else
	{
		snprintf(grname, sizeof grname, "%d", curstat->gen.rgid);
		groupname = grname;
	}

	/*
	** show process-info
	*/
	if (curstat->gen.state != 'E')
	{
		/*
		** show process-info of active process
		*/
		printg(varproclina,
		  	curstat->gen.pid,
		  	curstat->gen.ppid,
		  	username,
		  	groupname,
			convdate(curstat->gen.btime, format1),
			convtime(curstat->gen.btime, format2),
			curstat->gen.excode & ~(INT_MAX) ? 'N' : '-',
			'-',
		  	curstat->gen.state,
		  	perc, curstat->gen.name);
	}
	else
	{
		/*
		** show process-info of exited process
		*/
		if (curstat->gen.pid == 0)
			printg("    ? ");
		else
			printg("%5d ", curstat->gen.pid);

		if (curstat->gen.excode & 0xff)
		{
			if (curstat->gen.excode & 0x80)
				exittype = 'C';
			else
				exittype = 'S';
		}
		else
		{
			exittype = 'E';
		}

		printg(varprocline,
			curstat->gen.ppid,
		  	username,
		  	groupname,
			convdate(curstat->gen.btime, format1),
			convtime(curstat->gen.btime, format2),
			curstat->gen.excode & ~(INT_MAX) ? 'N' : '-',
			exittype,
			curstat->gen.excode & 0xff        ?
				curstat->gen.excode     & 0x7f :
				(curstat->gen.excode>>8) & 0xff,
		  	perc, curstat->gen.name);
	}
}

/*
** print a line containing the (almost) full command line
*/
void
showcmdproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	/*
	** show process-info
	*/
	if (curstat->gen.state != 'E')
	{
		/*
		** show process-info of active process
		*/
		printg(cmdproclina,
		  	curstat->gen.pid,
			perc,
			curstat->gen.cmdline[0] ? 
			         curstat->gen.cmdline : curstat->gen.name);
	}
	else
	{
		/*
		** show process-info of exited process
		*/
		if (curstat->gen.pid == 0)
			printg("    ? ");
		else
			printg("%5d ", curstat->gen.pid);

		printg(cmdprocline,
			perc, 
			curstat->gen.cmdline[0] ? 
			         curstat->gen.cmdline : curstat->gen.name);
	}
}

/*
** print a line of scheduling info about one process
*/
#define SCHED_NORMAL            0
#define SCHED_FIFO              1
#define SCHED_RR                2

void
showschproc(struct pstat *curstat, double perc, int nsecs, int avgval)
{
	char	*pol, exittype;

	switch (curstat->cpu.policy)
	{
	   case SCHED_NORMAL:
		pol = "norm";
		break;
	   case SCHED_RR:
		pol = "rr  ";
		break;
	   case SCHED_FIFO:
		pol = "fifo";
		break;
	   default:
		pol = "?";
	}

	/*
	** show process-info
	*/
	if (curstat->gen.state != 'E')
	{
		/*
		** show process-info of active process
		*/
		printg(schproclina,
		  	curstat->gen.pid,
		  	curstat->gen.nthrrun,
		  	curstat->gen.nthrslpi,
		  	curstat->gen.nthrslpu,
		  	pol,
		  	curstat->cpu.nice,
		  	curstat->cpu.prio,
		  	curstat->cpu.rtprio,
		  	curstat->cpu.curcpu,
			curstat->gen.excode & ~(INT_MAX) ? "N-" : "--",
		  	curstat->gen.state,
		  	perc, curstat->gen.name);
	}
	else
	{
		/*
		** show process-info of exited process
		*/
		if (curstat->gen.pid == 0)
			printg("    ?");
		else
			printg("%5d", curstat->gen.pid);

		if (curstat->gen.excode & 0xff)
		{
			if (curstat->gen.excode & 0x80)
				exittype = 'C';
			else
				exittype = 'S';
		}
		else
		{
			exittype = 'E';
		}

		printg(schprocline,
			curstat->gen.excode & ~(INT_MAX) ? 'N' : '-',
			exittype,
		 	curstat->gen.excode & 0xff        ? 
				 curstat->gen.excode     & 0x7f :
				(curstat->gen.excode>>8) & 0xff,
			perc, curstat->gen.name);
	}
}

/*
** print the header for the process-list
*/
void
priphead(int curlist, int totlist, char showtype, char showorder, char autosort)
{
	char	columnprefix;
	int	order = showorder;

	/*
	** determine the prefix for the sorting-column
	** to indicate automatic mode
	*/
	if (autosort)
		columnprefix = 'A';
	else
		columnprefix = ' ';

	/*
	** print the header line
	*/
	switch (showtype)
	{
	   case MPROCGEN:
		if (supportflags & PATCHSTAT)
		{
			printg(genprochdr1, columnprefix,
				columnhead[order], curlist, totlist);
		}
		else 
		{
			if (supportflags & IOSTAT)
				printg(genprochdr2, columnprefix,
					columnhead[order], curlist, totlist);
			else
				printg(genprochdr3, columnprefix,
					columnhead[order], curlist, totlist);
		}
		break;

	   case MPROCMEM:
		printg(memprochdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;

	   case MPROCDSK:
		if (supportflags & PATCHSTAT)
		{
			printg(dskprochdr1, columnprefix, columnhead[order],
						curlist, totlist);
		}
		else
		{
			if (supportflags & IOSTAT)
				printg(dskprochdr2, columnprefix,
					columnhead[order], curlist, totlist);
		}
		break;

	   case MPROCNET:
		printg(netprochdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;

	   case MPROCVAR:
		printg(varprochdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;

	   case MPROCARG:
		printg(cmdprochdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;

	   case MPROCSCH:
		printg(schprochdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;

	   case MCUMUSER:
		printg(totuserhdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;

	   case MCUMPROC:
		printg(totprochdr, columnprefix, columnhead[order],
						curlist, totlist);
		break;
	}
}


/*
** print the list of processes from the deviation-list
*/
int
priproc(struct pstat *pstat, int firstproc, int lastproc, int curline,
	int curlist, int totlist, char showtype, char showorder,
	struct syscap *sb, struct selection *sel, int nsecs, int avgval)
{
	register int		i;
	register struct pstat	*curstat;
	double			perc;

	/*
	** print info per actual process and maintain totals
	*/
	for (i=firstproc; i < lastproc; i++)
	{
		if (screen && curline >= LINES)	/* screen filled entirely ? */
			break;

		curstat = pstat+i;

		/*
		** calculate occupation-percentage of this process
		** depending on selected resource
		*/
		switch (showorder) 
		{
		   case MSORTCPU:
			perc = 0.0;

			if (sb->availcpu)
			{
				perc = (double)(curstat->cpu.stime +
				                curstat->cpu.utime  ) *
						100.0 /
						(sb->availcpu / sb->nrcpu);

				if (perc > 100.0 * sb->nrcpu)
					perc = 100.0 * sb->nrcpu;

				if (perc > 100.0 * curstat->gen.nthr)
					perc = 100.0 * curstat->gen.nthr;
			}
			break;

		   case MSORTMEM:
			perc = 0.0;

			if (sb->availmem)
			{
				perc = (double)curstat->mem.rmem *
                                               100.0 / sb->availmem;

				if (perc > 100.0)
					perc = 100.0;
			}
			break;

		   case MSORTDSK:
			perc = 0.0;

			if (supportflags & PATCHSTAT)
			{
				if (sb->availdsk)
				{
					perc = (double)(curstat->dsk.rio +
					                curstat->dsk.wio  ) *
				        		100.0 / sb->availdsk;

					if (perc > 100.0)
						perc = 100.0;
				}
			}
			else
			{
				if (sb->availdsk)
				{
					perc = (double)(curstat->dsk.rsz +
					                curstat->dsk.wsz  ) *
				        		100.0 / sb->availdsk;

					if (perc > 100.0)
						perc = 100.0;
				}
			}
			break;

		   case MSORTNET:
			perc = 0.0;

			if (sb->availnet)
			{
				perc = (double)(curstat->net.tcpsnd +
						curstat->net.tcprcv +
						curstat->net.udpsnd +
						curstat->net.udprcv +
						curstat->net.rawsnd +
						curstat->net.rawrcv ) *
						100.0 / sb->availnet;

				if (perc > 100.0)
					perc = 100.0;
			}
			break;

		   default:
			perc = 0.0;
		}

		/*
		** check if only processes of a particular user
		** should be shown
		*/
		if (sel->userid[0] != USERSTUB)
		{
			int	u = 0;

		     	while (sel->userid[u] != USERSTUB)
			{
				if (sel->userid[u] == curstat->gen.ruid)
					break;
				u++;
			}

			if (sel->userid[u] != curstat->gen.ruid)
				continue;
		}

		/*
		** check if only processes with a particular name
		** should be shown
		*/
		if (sel->procnamesz &&
		    regexec(&(sel->procregex), curstat->gen.name, 0, NULL, 0))
			continue;

		/*
		** now do the formatting of output
		*/
		switch (showtype)
		{
		   case MPROCGEN:
			showgenproc(curstat, perc, nsecs, avgval);
			break;

		   case MPROCMEM:
			showmemproc(curstat, perc, nsecs, avgval);
			break;

		   case MPROCDSK:
			showdskproc(curstat, perc, nsecs, avgval);
			break;

		   case MPROCNET:
			shownetproc(curstat, perc, nsecs, avgval);
			break;

		   case MPROCVAR:
			showvarproc(curstat, perc, nsecs, avgval);
			break;

		   case MPROCARG:
			showcmdproc(curstat, perc, nsecs, avgval);
			break;

		   case MPROCSCH:
			showschproc(curstat, perc, nsecs, avgval);
			break;

		   case MCUMUSER:
			showtotuser(curstat, perc, nsecs, avgval);
			break;

		   case MCUMPROC:
			showtotproc(curstat, perc, nsecs, avgval);
			break;
		}

		curline++;
	}

	return curline;
}


/*
** print the system-wide statistics
*/
int
prisyst(struct sstat *sstat, int curline, int nsecs, int avgval,
        int fixedhead, int usecolors, char *highorderp,
        int maxcpulines, int maxdsklines, int maxintlines)
{
	register int	i, lin;
	count_t		cputot, percputot, mstot, busy;
	count_t		udpin, udpout, ipin, ipout, ipfrw, ipindel;
	unsigned int	badness, highbadness=0;
	char		format1[16], format2[16], format3[16];
	char		format4[16], format5[16];
	char		busyval[5], coloron=0;

	/*
	** CPU statistics
	*/
	cputot = sstat->cpu.all.stime + sstat->cpu.all.utime +
	         sstat->cpu.all.ntime + sstat->cpu.all.itime +
	         sstat->cpu.all.wtime + sstat->cpu.all.Itime +
	         sstat->cpu.all.Stime + sstat->cpu.all.steal;

	busy   = (cputot - sstat->cpu.all.itime - sstat->cpu.all.wtime)
				* 100.0 / cputot;

	if (cpubadness)
		badness = busy * 100 / cpubadness;
	else
		badness = 0;

	if (highbadness < badness)
	{
		highbadness = badness;
		*highorderp = MSORTCPU;
	}

	if (cputot == 0)
		cputot = 1;		/* avoid divide-by-zero */

	percputot = cputot / sstat->cpu.nrcpu;

	if (percputot == 0)
		percputot = 1;		/* avoid divide-by-zero */

	coloron   = syscolorlabel("CPU", usecolors, badness);

	printg(" | sys %6.0f%% | user %6.0f%% | "
	       "irq  %6.0f%% | idle %6.0f%% | wait %6.0f%% |\n",
		(double) (sstat->cpu.all.stime * 100.0) / percputot,
		(double)((sstat->cpu.all.utime + sstat->cpu.all.ntime)
		                               * 100.0) / percputot,
		(double)((sstat->cpu.all.Itime + sstat->cpu.all.Stime)
		                               * 100.0) / percputot,
		(double) (sstat->cpu.all.itime * 100.0) / percputot,
		(double) (sstat->cpu.all.wtime * 100.0) / percputot);

	if (coloron)
		syscoloroff(coloron);
	
	curline++;

	if (sstat->cpu.all.steal)	/* only for virtual machines ... */
	{
		coloron   = syscolorlabel("CPU", usecolors, badness);

		printg(" | steal %4.0f%% | stl/cpu %3.0f%% | "
	       		"             |              |              |\n",
			(double) (sstat->cpu.all.steal * 100.0) / percputot,
			(double) (sstat->cpu.all.steal * 100.0) / cputot);

		if (coloron)
			syscoloroff(coloron);

		curline++;
	}

	if (sstat->cpu.nrcpu > 1)
	{
		for (i=lin=0; i < sstat->cpu.nrcpu && lin < maxcpulines; i++)
		{
			percputot =  sstat->cpu.cpu[i].stime +
			             sstat->cpu.cpu[i].utime +
	                             sstat->cpu.cpu[i].ntime +
			             sstat->cpu.cpu[i].itime +
	         	             sstat->cpu.cpu[i].wtime +
			             sstat->cpu.cpu[i].Itime +
	         	             sstat->cpu.cpu[i].Stime +
	         	             sstat->cpu.cpu[i].steal;

			if (percputot == (sstat->cpu.cpu[i].itime +
			                  sstat->cpu.cpu[i].wtime  ) &&
			                  !fixedhead                   )
				continue;	/* inactive cpu */

			busy   = (percputot - sstat->cpu.cpu[i].itime 
			                    - sstat->cpu.cpu[i].wtime)
							* 100.0 / percputot;

			if (cpubadness)
				badness = busy * 100 / cpubadness;
			else
				badness = 0;

			if (highbadness < badness)
			{
				highbadness = badness;
				*highorderp = MSORTCPU;
			}

			if (percputot == 0)
				percputot = 1; /* avoid divide-by-zero */


			coloron = syscolorlabel("cpu", usecolors, badness);

			printg(" | sys %6.0f%% | "
			       "user %6.0f%% | irq  %6.0f%% | "
			       "idle %6.0f%% | cpu%03d w%3.0f%% |\n",
				(double) (sstat->cpu.cpu[i].stime 
						* 100.0) / percputot,
				(double)((sstat->cpu.cpu[i].utime +
				          sstat->cpu.cpu[i].ntime)
			                       	* 100.0) / percputot,
				(double)((sstat->cpu.cpu[i].Itime +
				          sstat->cpu.cpu[i].Stime)
			                       	* 100.0) / percputot,
				(double) (sstat->cpu.cpu[i].itime
						* 100.0) / percputot,
				sstat->cpu.cpu[i].cpunr,
				(double) (sstat->cpu.cpu[i].wtime
						* 100.0) / percputot);

			if (coloron)
				syscoloroff(coloron);

			curline++;
			lin++;
		}
	}

	/*
	** other CPU-related statistics
	*/
	if (sstat->cpu.lavg1  > 999.0  ||
	    sstat->cpu.lavg5  > 9999.0 ||
	    sstat->cpu.lavg15 > 999.0    )
	{
		printg("CPL | avg1 %6.0f | avg5 %7.0f | avg15 %6.0f | csw %s"
 		       " | intr %s |\n",
			sstat->cpu.lavg1,
			sstat->cpu.lavg5,
			sstat->cpu.lavg15,
			val2valstr(sstat->cpu.csw,    format1, 8,avgval,nsecs),
			val2valstr(sstat->cpu.devint, format2, 7,avgval,nsecs));
	}
	else
	{
		printg("CPL | avg1 %6.2f | avg5 %7.2f | avg15 %6.2f | csw %s"
 		       " | intr %s |\n",
			sstat->cpu.lavg1,
			sstat->cpu.lavg5,
			sstat->cpu.lavg15,
			val2valstr(sstat->cpu.csw,    format1, 8,avgval,nsecs),
			val2valstr(sstat->cpu.devint, format2, 7,avgval,nsecs));
	}

	curline++;

	/*
	** MEMORY statistics
	*/
	busy   = (sstat->mem.physmem - sstat->mem.freemem
 	                             - sstat->mem.cachemem
	                             - sstat->mem.buffermem)
						* 100.0 / sstat->mem.physmem;

	if (membadness)
		badness = busy * 100 / membadness;
	else
		badness = 0;

	if (highbadness < badness)
	{
		highbadness = badness;
		*highorderp = MSORTMEM;
	}

	coloron = syscolorlabel("MEM", usecolors, badness);

	printg(" | tot  %s | free  %s | cache %s | buff  %s | slab  %s |\n",
		val2memstr(sstat->mem.physmem   * pagesize, format1,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.freemem   * pagesize, format2,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.cachemem  * pagesize, format3,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.buffermem * pagesize, format4,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.slabmem   * pagesize, format5,
							MBFORMAT, 0, 0));

	if (coloron)
		syscoloroff(coloron);

	curline++;

	/*
	** SWAP statistics
	*/
	busy   = (sstat->mem.totswap - sstat->mem.freeswap)
				* 100.0 / sstat->mem.totswap;

	if (swpbadness)
		badness = busy * 100 / swpbadness;
	else
		badness = 0;

	if (highbadness < badness)
	{
		highbadness = badness;
		*highorderp = MSORTMEM;
	}

	if (sstat->mem.commitlim && sstat->mem.committed > sstat->mem.commitlim)
		 badness = 100;		/* force colored output */

	coloron = syscolorlabel("SWP", usecolors, badness);

	printg(" | tot  %s | free  %s |              "
	       "| vmcom %s | vmlim %s |\n",
		val2memstr(sstat->mem.totswap   * pagesize, format1,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.freeswap  * pagesize, format2,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.committed * pagesize, format3,
							MBFORMAT, 0, 0),
		val2memstr(sstat->mem.commitlim * pagesize, format4,
							MBFORMAT, 0, 0));

	if (coloron)
		syscoloroff(coloron);

	curline++;

	/*
	** PAGING statistics
	*/
	if (fixedhead             ||
	    sstat->mem.pgscans    ||
	    sstat->mem.allocstall ||
	    sstat->mem.swins      ||
	    sstat->mem.swouts       )
	{
		busy = sstat->mem.swouts / nsecs * pagbadness;

		if (busy > 100)
			busy = 100;

		if (membadness)
			badness = busy * 100 / membadness;
		else
			badness = 0;

		if (highbadness < badness)
		{
			highbadness = badness;
			*highorderp = MSORTMEM;
		}

		/*
		** take care that this line is anyhow colored for
		** 'almost critical' in case of swapouts > 1 per second
		*/
		if (sstat->mem.swouts / nsecs > 0  &&
		    pagbadness && almostcrit && badness < almostcrit)
			badness = almostcrit;

		coloron = syscolorlabel("PAG", usecolors, badness);

		printg(" | scan %s | stall %s |             "
		       " | swin %s | swout %s |\n",
			val2valstr(sstat->mem.pgscans,    format1, 6,
								avgval, nsecs),
			val2valstr(sstat->mem.allocstall, format2, 6,
								avgval, nsecs),
			val2valstr(sstat->mem.swins,      format3, 7,
								avgval, nsecs),
			val2valstr(sstat->mem.swouts,     format4, 6,
								avgval, nsecs));

		if (coloron)
			syscoloroff(coloron);

		curline++;
	}

	/*
	** DISK statistics
	**
	** If extended disk-stats's are supported, take them
	*/
	mstot = cputot * 1000 / hertz / sstat->cpu.nrcpu; 

	for (i=0, lin=0; sstat->xdsk.xdsk[i].name[0] && lin < maxdsklines; i++)
	{
		count_t iotot;

		iotot =  sstat->xdsk.xdsk[i].nread + sstat->xdsk.xdsk[i].nwrite;

		busy   = (double)(sstat->xdsk.xdsk[i].io_ms * 100.0 / mstot);

		if (dskbadness)
			badness = busy * 100 / dskbadness;
		else
			badness = 0;

		if (highbadness < badness &&
				(supportflags & (PATCHSTAT|IOSTAT)) )
		{
			highbadness = badness;
			*highorderp = MSORTDSK;
		}

		if (iotot || fixedhead)
		{
			coloron = syscolorlabel("DSK", usecolors, badness);

		    	printg(" | %11.11s | busy %6.0lf%% | read %s |"
		               " write %s | avio %4.0lf ms |\n",
		    	  sstat->xdsk.xdsk[i].name,
			  (double)(sstat->xdsk.xdsk[i].io_ms * 100.0 / mstot),
		    	  val2valstr(sstat->xdsk.xdsk[i].nread,  format1, 7,
								avgval, nsecs),
		    	  val2valstr(sstat->xdsk.xdsk[i].nwrite, format2, 6,
								avgval, nsecs),
		      	  iotot?(double)(sstat->xdsk.xdsk[i].io_ms/iotot):0.0);

			if (coloron)
				syscoloroff(coloron);

		    curline++;
		    lin++;
		}
	}

	/*
	** NET statistics
	*/
        udpin  = sstat->net.udpv4.InDatagrams  +
			sstat->net.udpv6.Udp6InDatagrams;
	udpout = sstat->net.udpv4.OutDatagrams +
			sstat->net.udpv6.Udp6OutDatagrams;

	if (sstat->net.tcp.InSegs || sstat->net.tcp.OutSegs ||
	                   udpin ||                udpout || fixedhead )
	{
		printg("NET | transport   | tcpi %s | tcpo %s |"
		       " udpi %s | udpo %s |\n",
			val2valstr(sstat->net.tcp.InSegs,  format1, 7,
								avgval, nsecs),
			val2valstr(sstat->net.tcp.OutSegs, format2, 7,
								avgval, nsecs),
                        val2valstr(udpin,   format3, 7, avgval, nsecs),
			val2valstr(udpout,  format4, 7, avgval, nsecs));
		curline++;
	}

	ipin    = sstat->net.ipv4.InReceives  +
			sstat->net.ipv6.Ip6InReceives;
	ipout   = sstat->net.ipv4.OutRequests +
			sstat->net.ipv6.Ip6OutRequests;
	ipindel = sstat->net.ipv4.InDelivers +
			sstat->net.ipv6.Ip6InDelivers;
	ipfrw   = sstat->net.ipv4.ForwDatagrams +
			sstat->net.ipv6.Ip6OutForwDatagrams;

	if (ipin || ipout || fixedhead )
	{
		printg("NET | network     | ipi %s | ipo %s | ipfrw %s |"
		       " deliv %s |\n",
			val2valstr(ipin,    format1, 8, avgval, nsecs),
			val2valstr(ipout,   format2, 8, avgval, nsecs),
			val2valstr(ipfrw,   format3, 6, avgval, nsecs),
			val2valstr(ipindel, format4, 6, avgval, nsecs));

		curline++;
	}

	for (i=0, lin=0; sstat->intf.intf[i].name[0] && lin < maxintlines; i++)
	{
		if (sstat->intf.intf[i].rpack ||
		    sstat->intf.intf[i].spack || fixedhead)
		{
			char    *iform = "Kbps";
			char    *oform = "Kbps";
			count_t ival, ipres, oval, opres;

			/*
			** convert byte-transfers to bit-transfers     (*    8)
			** convert bit-transfers  to kilobit-transfers (/ 1000)
			** per second
			*/
			ival	= sstat->intf.intf[i].rbyte/125/nsecs;
			oval	= sstat->intf.intf[i].sbyte/125/nsecs;

			if (ival >= 10000)    /* input value 4 positions */
			{
				if (ival < 10000000)
				{
					ipres = ival / 1000;
					iform = "Mbps";
				}
				else
				{
					ipres = ival / 1000000;
					iform = "Gbps";
				}
			}
			else
			{
				ipres = ival;
			}

			if (oval >= 10000)    /* input value 4 positions */
			{
				if (oval < 10000000)
				{
					opres = oval / 1000;
					oform = "Mbps";
				}
				else
				{
					opres = oval / 1000000;
					oform = "Gbps";
				}
			}
			else
			{
				opres = oval;
			}

			/*
			** calculate busy-percentage for interface
			*/
			if (sstat->intf.intf[i].speed)	/* speed known? */
			{
				if (sstat->intf.intf[i].duplex)
					busy = (ival > oval ? ival : oval) /
					       (sstat->intf.intf[i].speed *10);
				else
					busy = (ival + oval) /
					       (sstat->intf.intf[i].speed *10);

				snprintf(busyval, sizeof busyval,
							"%3lld%%", busy);
			}
			else
			{
				strcpy(busyval, "----"); /* speed unknown */

				busy = 0;
			}

			if (netbadness)
				badness = busy * 100 / netbadness;
			else
				badness = 0;

			if (highbadness < badness &&
					(supportflags & PATCHSTAT) )
			{
				highbadness = badness;
				*highorderp = MSORTNET;
			}

			coloron = syscolorlabel("NET", usecolors, badness);

			printg(" | %-6.6s %4s | pcki %s | pcko %s |"
			       " si %4lld %s | so %4lld %s |\n",
				sstat->intf.intf[i].name,
				busyval,
				val2valstr(sstat->intf.intf[i].rpack,
						format1, 7, avgval, nsecs),
				val2valstr(sstat->intf.intf[i].spack,
						format2, 7, avgval, nsecs),
				ipres, iform,
				opres, oform);

			if (coloron)
				syscoloroff(coloron);

			curline++;
			lin++;
		}
	}

	/*
	** application statistics
	**
	** WWW: notice that we cause one access ourselves by fetching
	**      the statistical counters
	*/
#if	HTTPSTATS
	if (sstat->www.accesses > 1 || fixedhead )
	{
		printg("WWW | reqs %s | totKB %s | byt/rq %s | iwork %s |"
		       " bwork %s |\n",
			val2valstr(sstat->www.accesses,  format1, 6,
							 avgval, nsecs),
			val2valstr(sstat->www.totkbytes, format2, 6,
						         avgval, nsecs),
			val2valstr(sstat->www.accesses ?
			    sstat->www.totkbytes*1024/sstat->www.accesses : 0,
			                                 format3, 5, 0, 0),
			val2valstr(sstat->www.iworkers,  format4, 6, 0, 0),
			val2valstr(sstat->www.bworkers,  format5, 6, 0, 0) );
		curline++;
	}
#endif

	/*
	** if the system is hardly loaded, still CPU-ordering of
	** processes is most interesting (instead of memory)
	*/
	if (highbadness < 70 && *highorderp == MSORTMEM)
		*highorderp = MSORTCPU;

	return curline;
}

/*
** sort-functions
*/
int
compcpu(const void *a, const void *b)
{
	register count_t acpu = ((struct pstat *)a)->cpu.stime +
	                        ((struct pstat *)a)->cpu.utime;
	register count_t bcpu = ((struct pstat *)b)->cpu.stime +
	                        ((struct pstat *)b)->cpu.utime;

	if (acpu < bcpu) return  1;
	if (acpu > bcpu) return -1;
	                 return compmem(a, b);
}

int
compdsk(const void *a, const void *b)
{
	register count_t adsk = ((struct pstat *)a)->dsk.rio +
			        ((struct pstat *)a)->dsk.wio;
	register count_t bdsk = ((struct pstat *)b)->dsk.rio +
			        ((struct pstat *)b)->dsk.wio;

	if (adsk < bdsk) return  1;
	if (adsk > bdsk) return -1;
	                 return compcpu(a, b);
}

int
compmem(const void *a, const void *b)
{
	register count_t amem = ((struct pstat *)a)->mem.rmem;
	register count_t bmem = ((struct pstat *)b)->mem.rmem;

	if (amem < bmem) return  1;
	if (amem > bmem) return -1;
	                 return  0;
}

int
compnet(const void *a, const void *b)
{
	register count_t anet = ((struct pstat *)a)->net.tcpsnd +
			        ((struct pstat *)a)->net.tcprcv +
			        ((struct pstat *)a)->net.udpsnd +
			        ((struct pstat *)a)->net.udprcv +
			        ((struct pstat *)a)->net.rawsnd +
			        ((struct pstat *)a)->net.rawrcv  ;
	register count_t bnet = ((struct pstat *)b)->net.tcpsnd +
			        ((struct pstat *)b)->net.tcprcv +
			        ((struct pstat *)b)->net.udpsnd +
			        ((struct pstat *)b)->net.udprcv +
			        ((struct pstat *)b)->net.rawsnd +
			        ((struct pstat *)b)->net.rawrcv  ;

	if (anet < bnet) return  1;
	if (anet > bnet) return -1;
	                 return compcpu(a, b);
}

int
cpucompar(const void *a, const void *b)
{
	register count_t aidle = ((struct percpu *)a)->itime +
	                         ((struct percpu *)a)->wtime;
	register count_t bidle = ((struct percpu *)b)->itime +
	                         ((struct percpu *)b)->wtime;

	if (aidle < bidle) return -1;
	if (aidle > bidle) return  1;
	                   return  0;
}

int
diskcompar(const void *a, const void *b)
{
	register count_t amsio = ((struct perxdsk *)a)->io_ms;
	register count_t bmsio = ((struct perxdsk *)b)->io_ms;

	if (amsio < bmsio) return  1;
	if (amsio > bmsio) return -1;
	                   return  0;
}

int
intfcompar(const void *a, const void *b)
{
	register count_t afactor=0, bfactor=0;

	count_t aspeed         = ((struct perintf *)a)->speed;
	count_t bspeed         = ((struct perintf *)b)->speed;
	char    aduplex        = ((struct perintf *)a)->duplex;
	char    bduplex        = ((struct perintf *)b)->duplex;
	count_t arbyte         = ((struct perintf *)a)->rbyte;
	count_t brbyte         = ((struct perintf *)b)->rbyte;
	count_t asbyte         = ((struct perintf *)a)->sbyte;
	count_t bsbyte         = ((struct perintf *)b)->sbyte;


	/*
	** if speed of first interface known, calculate busy factor
	*/
	if (aspeed)
	{
		if (aduplex)
			afactor = (arbyte > asbyte ? arbyte : asbyte) /
								(aspeed / 10);
		else
			afactor = (arbyte + asbyte) /		(aspeed / 10);
	}

	/*
	** if speed of second interface known, calculate busy factor
	*/
	if (bspeed)
	{
		if (bduplex)
			bfactor = (brbyte > bsbyte ? brbyte : bsbyte) /
								(bspeed / 10);
		else
			bfactor = (brbyte + bsbyte) /		(bspeed / 10);
	}

	/*
	** compare interfaces
	*/
	if (aspeed && bspeed)
	{
		if (afactor < bfactor)	return  1;
		if (afactor > bfactor)	return -1;
					return  0;
	}

	if (!aspeed && !bspeed)
	{
		if ((arbyte + asbyte) < (brbyte + bsbyte))	return  1;
		if ((arbyte + asbyte) > (brbyte + bsbyte))	return -1;
								return  0;
	}

	if (aspeed)
		return -1;
	else
		return  1;
}

int
compusr(const void *a, const void *b)
{
	register int uida = ((struct pstat *)a)->gen.ruid;
	register int uidb = ((struct pstat *)b)->gen.ruid;

	if (uida > uidb) return  1;
	if (uida < uidb) return -1;
	                 return  0;
}

int
compnam(const void *a, const void *b)
{
	register char *nama = ((struct pstat *)a)->gen.name;
	register char *namb = ((struct pstat *)b)->gen.name;

	return strcmp(nama, namb);
}

/*
** print the label of a system-statistics line and switch on
** colors if needed 
*/
static int
syscolorlabel(char *labeltext, int usecolors, unsigned int badness)
{
	if (usecolors)
	{
		if (badness >= 100)
		{
			attron(A_BOLD);
			attron(COLOR_PAIR(COLORHIGH));
			attron (A_BLINK);
			printg(labeltext);
			attroff(A_BLINK);
			return COLORHIGH;
		}

		if (almostcrit && badness >= almostcrit)
		{
			attron(A_BOLD);
			attron(COLOR_PAIR(COLORMED));
			printg(labeltext);
			return COLORMED;
		}
	}

	/*
	** no colors required or no reason to show colors
	*/
	printg(labeltext);
	return 0;
}

static void
syscoloroff(char curcolor)
{
	attroff(A_BOLD);
	attroff(COLOR_PAIR(curcolor));
}

/*
** handle modifications from the ~/.atoprc file
*/
void
do_cpucritperc(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: cpucritperc value not numeric\n");
		exit(1);
	}

	if (value < 0 || value > 100)
	{
		fprintf(stderr,
			".atoprc: cpucritperc value not in range 0-100\n");
		exit(1);
	}

	cpubadness = value;
}

void
do_memcritperc(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: memcritperc value not numeric\n");
		exit(1);
	}

	if (value < 0 || value > 100)
	{
		fprintf(stderr,
			".atoprc: memcritperc value not in range 0-100\n");
		exit(1);
	}

	membadness = value;
}

void
do_swpcritperc(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: swpcritperc value not numeric\n");
		exit(1);
	}

	if (value < 0 || value > 100)
	{
		fprintf(stderr,
			".atoprc: swpcritperc value not in range 0-100\n");
		exit(1);
	}

	swpbadness = value;
}

void
do_dskcritperc(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: dskcritperc value not numeric\n");
		exit(1);
	}

	if (value < 0 || value > 100)
	{
		fprintf(stderr,
			".atoprc: dskcritperc value not in range 0-100\n");
		exit(1);
	}

	dskbadness = value;
}

void
do_netcritperc(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: netcritperc value not numeric\n");
		exit(1);
	}

	if (value < 0 || value > 100)
	{
		fprintf(stderr,
			".atoprc: netcritperc value not in range 0-100\n");
		exit(1);
	}

	netbadness = value;
}

void
do_swoutcritsec(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: swoutcritsec value not numeric\n");
		exit(1);
	}

	if (value < 0)
	{
		fprintf(stderr,
			".atoprc: swoutcritsec value less then 0\n");
		exit(1);
	}

	pagbadness = value;
}

void
do_almostcrit(char *val)
{
	int	value = atoi(val);

	if ( !numeric(val))
	{
		fprintf(stderr, ".atoprc: almostcrit value not numeric\n");
		exit(1);
	}

	if (value < 0 || value > 99)
	{
		fprintf(stderr,
			".atoprc: almostcrit value not in range 0-99\n");
		exit(1);
	}

	almostcrit = value;
}
