PATH=/sbin:/usr/sbin:/bin:/usr/bin

# start atop daily at midnight
0 0 * * * root invoke-rc.d atop _cron
