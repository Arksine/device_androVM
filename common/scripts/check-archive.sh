#!/system/bin/sh

PROGNAME=`basename $0`

#############
# FUNCTIONS #
#############

_log_message() {
    echo "$1"
    log -t "$PROGNAME" "$1"
}

# check root access
id | grep root
if [ $? -ne 0 ]
then
    _log_message "`basename $0` must be run as root"
    exit 1
fi

########
# main #
########
{
    # Check if args are ok
    if [ $# -ne 1 ]
    then
        _log_message "Usage: $PROGNAME <archive-to-check.zip>"
        exit 1
    fi

    ZIPFILE=$1

    # Check if argument is an .zip archive
    case "$ZIPFILE" in
        *.zip)
            _log_message "$ZIPFILE seems to be a zip archive";
            ;;
        *)
            _log_message "Sorry "$ZIPFILE" doesn't seem to be a zip archive"
            exit 1
    esac

    # Checking zip content
    RESULT=`unzip -l "$ZIPFILE" 2> /dev/null | grep " system/" | wc -l`
    if [ "$RESULT" -gt 0 ]
    then
    # We found a system/ directory
        _log_message "[$PROGNAME] system/ found in $ZIPFILE; return 0"
        exit 0 # OK code
    else
    # No system/ directory found, should not flash it
        _log_message "[$PROGNAME] system/ NOT found in $ZIPFILE; return 1"
        exit 1 # error code
    fi
}