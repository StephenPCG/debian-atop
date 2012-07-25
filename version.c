/* No manual changes in this file */
#include <stdio.h>
#include <string.h>

static char atoprevision[] = "1.27-3";
static char atopdate[]     = "2012/07/22 11:38:52";

char *
getstrvers(void)
{
	static char vers[256];

	snprintf(vers, sizeof vers,
		"Version: %s - %s     < gerlof.langeveld@atoptool.nl >",
		atoprevision, atopdate);

	return vers;
}

unsigned short
getnumvers(void)
{
	int	vers1, vers2;

	sscanf(atoprevision, "%u.%u", &vers1, &vers2);

	return (unsigned short) ((vers1 << 8) + vers2);
}
