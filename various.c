/*
** ATOP - System & Process Monitor
**
** The program 'atop' offers the possibility to view the activity of
** the system on system-level as well as process-level.
**
** This source-file contains various functions to a.o. format the
** time-of-day, the cpu-time consumption and the memory-occupation. 
** ==========================================================================
** Author:      Gerlof Langeveld - AT Computing, Nijmegen, Holland
** E-mail:      gerlof@ATComputing.nl
** Date:        November 1996
** LINUX-port:  June 2000
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
** $Log: various.c,v $
** Revision 1.14  2007/02/13 10:32:47  gerlof
** Removal of external declarations.
** Removal of function getpagesz().
**
** Revision 1.13  2006/02/07 08:27:21  gerlof
** Add possibility to show counters per second.
** Modify presentation of CPU-values.
**
** Revision 1.12  2005/10/31 12:26:09  gerlof
** Modified date-format to yyyy/mm/dd.
**
** Revision 1.11  2005/10/21 09:51:29  gerlof
** Per-user accumulation of resource consumption.
**
** Revision 1.10  2004/05/06 09:46:24  gerlof
** Ported to kernel-version 2.6.
**
** Revision 1.9  2003/07/07 09:27:46  gerlof
** Cleanup code (-Wall proof).
**
** Revision 1.8  2003/07/03 11:16:59  gerlof
** Minor bug solutions.
**
** Revision 1.7  2003/06/30 11:31:17  gerlof
** Enlarge counters to 'long long'.
**
** Revision 1.6  2003/06/24 06:22:24  gerlof
** Limit number of system resource lines.
**
** Revision 1.5  2002/08/30 07:49:09  gerlof
** Convert a hh:mm string into a number of seconds since 00:00.
**
** Revision 1.4  2002/08/27 12:08:37  gerlof
** Modified date format (from yyyy/mm/dd to mm/dd/yyyy).
**
** Revision 1.3  2002/07/24 11:14:05  gerlof
** Changed to ease porting to other UNIX-platforms.
**
** Revision 1.2  2002/07/11 09:43:36  root
** Modified HZ into sysconf(_SC_CLK_TCK).
**
** Revision 1.1  2001/10/02 10:43:36  gerlof
** Initial revision
**
*/

static const char rcsid[] = "$Id: various.c,v 1.14 2007/02/13 10:32:47 gerlof Exp $";

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

#include "atop.h"
#include "acctproc.h"

/*
** Function convtime() converts a value (number of seconds since
** 1-1-1970) to an ascii-string in the format hh:mm:ss, stored in
** chartim (9 bytes long).
*/
char *
convtime(time_t utime, char *chartim)
{
	struct tm 	*tt;

	tt = localtime(&utime);

	sprintf(chartim, "%02d:%02d:%02d", tt->tm_hour, tt->tm_min, tt->tm_sec);

	return chartim;
}

/*
** Function convdate() converts a value (number of seconds since
** 1-1-1970) to an ascii-string in the format yyyy/mm/dd, stored in
** chardat (11 bytes long).
*/
char *
convdate(time_t utime, char *chardat)
{
	struct tm 	*tt;

	tt = localtime(&utime);

	sprintf(chardat, "%04d/%02d/%02d",
		tt->tm_year+1900, tt->tm_mon+1, tt->tm_mday);

	return chardat;
}


/*
** Convert a hh:mm string into a number of seconds since 00:00
**
** Return-value:	0 - Wrong input-format
**			1 - Success
*/
int
hhmm2secs(char *itim, unsigned int *otim)
{
	register int	i;
	int		hours, minutes;

	/*
	** check string syntax
	*/
	for (i=0; *(itim+i); i++)
		if ( !isdigit(*(itim+i)) && *(itim+i) != ':' )
			return(0);

	sscanf(itim, "%d:%d", &hours, &minutes);

	if ( hours < 0 || hours > 23 || minutes < 0 || minutes > 59 )
		return(0);

	*otim = (hours * 3600) + (minutes * 60);

	if (*otim >= SECSDAY)
		*otim = SECSDAY-1;

	return(1);
}


/*
** Return number of seconds since midnight according local clock time
**
** Return-value:        Number of seconds
*/
int
daysecs(time_t itime)
{
	struct tm *tt;

	tt = localtime(&itime);

	return( (tt->tm_hour*3600) + (tt->tm_min*60) );
}


/*
** Function val2valstr() converts a value to an ascii-string of a fixed
** number of positions; if the value does not fit, it will be formatted to
** exponent-notation (as short as possible, so not via the standard printf-
** formatters %f or %e). The offered string should have a length of width+1.
** The value might even be printed as an average for the interval-time.
*/
char *
val2valstr(count_t value, char *strvalue, int width, int avg, int nsecs)
{
	count_t	maxval, remain = 0;
	int	exp     = 0;
	char	*suffix = "";

	if (avg)
	{
		value  = (value + (nsecs/2)) / nsecs;     /* rounded value */
		width  = width - 2;	/* subtract two positions for '/s' */
		suffix = "/s";
	}

	maxval = pow(10.0, width) - 1;

	if (value < maxval)
	{
		sprintf(strvalue, "%*lld%s", width, value, suffix);
	}
	else
	{
		if (width < 3)
		{
			/*
			** cannot avoid ignoring width
			*/
			sprintf(strvalue, "%lld%s", value, suffix);
		}
		else
		{
			/*
			** calculate new maximum value for the string,
			** calculating space for 'e' (exponent) + one digit
			*/
			width -= 2;
			maxval = pow(10.0, width) - 1;

			while (value > maxval)
			{
				exp++;
				remain = value % 10;
				value /= 10;
			}

			if (remain >= 5)
				value++;

			sprintf(strvalue, "%*llde%d%s",
					width, value, exp, suffix);
		}
	}

	return strvalue;
}

/*
** Function val2cpustr() converts a value (number of milliseconds)
** to an ascii-string of 7 positions in milliseconds or minute-seconds or
** hours-minutes, stored in strvalue (at least 8 positions).
*/
#define	MAXMSEC		(count_t)100000
#define	MAXSEC		(count_t)60000

char *
val2cpustr(count_t value, char *strvalue)
{
	if (value < MAXMSEC)
	{
		sprintf(strvalue, "%3lld.%02llds", value/1000, value%1000/10);
	}
	else
	{
		/*
		** millisecs irrelevant; round to seconds
		*/
		value = (value + 500) / 1000;

		if (value < MAXSEC) 
		{
			sprintf(strvalue, "%3lldm%02llds", value/60, value%60);
		}
		else
		{
			/*
			** seconds irrelevant; round to minutes
			*/
			value = (value + 30) / 60;

			sprintf(strvalue, "%3lldh%02lldm", value/60, value%60);
		}
	}

	return strvalue;
}


/*
** Function val2memstr() converts a value (number of bytes)
** to an ascii-string in a specific format (indicated by pformat).
** The result-string is placed in the area pointed to strvalue,
** which should be able to contain at least 7 positions.
*/
#define	ONEKBYTE	1024
#define	ONEMBYTE	1048576
#define	ONEGBYTE	1073741824L

#define	MAXBYTE		1024
#define	MAXKBYTE	ONEKBYTE*99999L
#define	MAXMBYTE	ONEMBYTE*999L

char *
val2memstr(count_t value, char *strvalue, int pformat, int avgval, int nsecs)
{
	char 	aformat;	/* advised format		*/
	count_t	verifyval;
	char	*suffix = "";
	int	basewidth = 6;


	/*
	** notice that the value can be negative, in which case the
	** modulo-value should be evaluated and an extra position should
	** be reserved for the sign
	*/
	if (value < 0)
		verifyval = -value * 10;
	else
		verifyval =  value;

	/*
	** verify if printed value is required per second (average) or total
	*/
	if (avgval)
	{
		value     /= nsecs;
		verifyval *= 100;
		basewidth -= 2;
		suffix     = "/s";
	}
	
	/*
	** determine which format will be used on bases of the value itself
	*/
	if (verifyval <= MAXBYTE)	/* bytes ? */
		aformat = ANYFORMAT;
	else
		if (verifyval <= MAXKBYTE)	/* kbytes ? */
			aformat = KBFORMAT;
		else
			if (verifyval <= MAXMBYTE)	/* mbytes ? */
				aformat = MBFORMAT;
			else
				aformat = GBFORMAT;

	/*
	** check if this is also the preferred format
	*/
	if (aformat <= pformat)
		aformat = pformat;

	switch (aformat)
	{
	   case	ANYFORMAT:
		sprintf(strvalue, "%*lld%s",
				basewidth, value, suffix);
		break;

	   case	KBFORMAT:
		sprintf(strvalue, "%*lldK%s",
				basewidth-1, value/ONEKBYTE, suffix);
		break;

	   case	MBFORMAT:
		sprintf(strvalue, "%*.1lfM%s",
			basewidth-1, (double)((double)value/ONEMBYTE), suffix); 
		break;

	   case	GBFORMAT:
		sprintf(strvalue, "%*.1lfG%s",
			basewidth-1, (double)((double)value/ONEGBYTE), suffix);
		break;

	   default:
		sprintf(strvalue, "!TILT!");
	}

	return strvalue;
}


/*
** Function numeric() checks if the ascii-string contains 
** a numeric (positive) value.
** Returns 1 (true) if so, or 0 (false).
*/
int
numeric(char *ns)
{
	register char *s = ns;

	while (*s)
		if (*s < '0' || *s > '9')
			return(0);		/* false */
		else
			s++;
	return(1);				/* true  */
}


/*
** Function getboot() returns the boot-time of this system 
** (as number of seconds since 1-1-1970).
*/
time_t
getboot(void)
{
	static time_t	boottime;

	if (!boottime)		/* do this only once */
	{
#ifdef	linux
		time_t		getbootlinux(long);

		boottime = getbootlinux(hertz);
#else
		struct tms	tms;
		time_t 		boottime_again;

		/*
		** beware that between time() and times() a one-second
		** upgrade could have taken place in the kernel, so an
		** extra check is issued; if we were around a
		** second-upgrade, just do it again (we just passed
		** the danger-zone)
		*/
		boottime 	= time(0) - (times(&tms) / hertz);
		boottime_again	= time(0) - (times(&tms) / hertz);

		if (boottime != boottime_again)
			boottime = time(0) - (times(&tms) / hertz);
#endif
	}

	return boottime;
}

/*
** signal catcher for cleanup before exit
*/
void
cleanstop(exitcode)
{
	acctswoff();
	(vis.show_end)();
	exit(exitcode);
}
