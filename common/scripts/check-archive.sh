#!/system/bin/sh

PROGNAME=`basename $0`

#############
# FUNCTIONS #
#############

_log_message() {
    echo "$1"
    log -t "$PROGNAME" "$1"
}

_exit_success() {
    echo "{Result:OK};"
    exit 0
}

_exit_failure() {
    echo "{Result:KO};"
    exit 1
}

# check root access
id | grep root
if [ $? -ne 0 ]
then
    _log_message "`basename $0` must be run as root"
    _exit_failure
fi

########
# main #
########
{
    # Check if args are ok
    if [ $# -ne 1 ]
    then
        _log_message "Usage: $PROGNAME <archive-to-check.zip>"
        _exit_failure
    fi

    ZIPFILE=$1

    # Check if argument is an .zip archive
    case "$ZIPFILE" in
        *.zip)
            _log_message "$ZIPFILE seems to be a zip archive";
            ;;
        *)
            _log_message "Sorry "$ZIPFILE" doesn't seem to be a zip archive"
            _exit_failure
    esac

    # Checking zip content
    RESULT=`unzip -l "$ZIPFILE" 2> /dev/null | grep " system/" | wc -l`
    if [ "$RESULT" -gt 0 ]
    then
    # We found a system/ directory
        _log_message "[$PROGNAME] system/ found in $ZIPFILE; return 0"
        _exit_success # OK code
    else
    # No system/ directory found, should not flash it
        _log_message "[$PROGNAME] system/ NOT found in $ZIPFILE; return 1"
        _exit_failure # error code
    fi
}