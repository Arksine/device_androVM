#!/system/bin/sh

PROGNAME=`basename $0`

########
# main #
########

# Check if args are ok
if [ $# -ne 1 ]
then
  echo "Usage: $PROGNAME <archive-to-check.zip>"
  exit 1
fi

ZIPFILE=$1

# Check if argument is an .zip archive
case "$ZIPFILE" in
  *.zip)
    echo "$ZIPFILE seems to be a zip archive";
    ;;
  *)
    echo "Sorry "$ZIPFILE" doesn't seem to be a zip archive"
    exit 1
esac

# Checking zip content
RESULT=`unzip -l "$ZIPFILE" 2> /dev/null | grep " system/" | wc -l`
if [ "$RESULT" -gt 0 ]
then
  # We found a system/ directory
  echo "[$PROGNAME] system/ found in $ZIPFILE; return 0"
  exit 0 # OK code
else
  # No system/ directory found, should not flash it
  echo "[$PROGNAME] system/ NOT found in $ZIPFILE; return 1"
  exit 1 # error code
fi

