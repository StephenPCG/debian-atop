/* No manual changes in this file */
#include <stdio.h>
#include <string.h>

static const char rcsid[] = "$Id: version.c,v 1.23 2008/03/06 09:11:09 root Exp root $";

static char atoprevision[] = "$Revision: 1.23 $";
static char atopdate[]     = "$Date: 2008/03/06 09:11:09 $";

char *
getstrvers(void)
{
	static char vers[256];

	atoprevision[sizeof atoprevision - 3] = '\0';
	atopdate    [sizeof atopdate     - 3] = '\0';

	snprintf(vers, sizeof vers,
		"Version:%s -%s           < gerlof@ATComputing.nl >",
			strchr(atoprevision, ' '),
			strchr(atopdate    , ' '));

	return vers;
}

unsigned short
getnumvers(void)
{
	int	vers1, vers2;

	sscanf(atoprevision, "%*s %u.%u", &vers1, &vers2);

	return (unsigned short) ((vers1 << 8) + vers2);
}
