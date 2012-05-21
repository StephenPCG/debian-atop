#! /bin/sh
#
# atop init script
#

### BEGIN INIT INFO
# Provides:          atop
# Required-Start:    $syslog
# Required-Stop:     $syslog
# Should-Start:      $local_fs $remote_fs
# Should-Stop:       $local_fs $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Monitor for system resources and process activity
# Description:       Atop is an ASCII full-screen performance monitor,
#                    similar to the top command, but atop only shows
#                    the active system-resources and processes, and
#                    only shows the deviations since the previous
#                    interval.
### END INIT INFO

PATH=/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/bin/atop
DARGS="-a -w /var/log/atop.log 600"
NAME=atop
DESC="atop system monitor"

test -x $DAEMON || exit 0

set -e

. /lib/lsb/init-functions

case "$1" in
  start)
	echo -n "Starting $DESC: "
	start-stop-daemon --start --background --quiet --exec $DAEMON -- $DARGS
	echo "$NAME."
	;;
  stop)
	echo -n "Stopping $DESC: "
	start-stop-daemon --stop --quiet --retry 2 --oknodo --exec $DAEMON -- $DARGS
	echo "$NAME."
	;;
  #reload)
  restart|force-reload)
	#
	#	If the "reload" option is implemented, move the "force-reload"
	#	option to the "reload" entry above. If not, "force-reload" is
	#	just the same as "restart".
	#
	echo -n "Restarting $DESC: "
	start-stop-daemon --stop --quiet --exec $DAEMON -- $DARGS
	sleep 1
	start-stop-daemon --start --background --quiet --exec $DAEMON -- $DARGS
	echo "$NAME."
	;;
  status)
       status_of_proc $DAEMON $NAME
       ;;
  *)
	N=/etc/init.d/$NAME
	echo "Usage: $N {start|stop|restart|force-reload|status}" >&2
	exit 1
	;;
esac

exit 0
