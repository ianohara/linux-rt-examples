#!/bin/bash
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
# This script uses rsync to copy all of the test code
# over to the rt-gumstix-tests/ directory in the
# specified user@gumstix home directory.  If no
# user@gumstix is specified, the default (hardcoded below
# if you want to change it) is used.
#
# Ian O'Hara, 2012
# ian.ohara@gmail.com
#
###########################

DEF_USERHOST="captain@realtime"
DEF_SRCDIR="src/"

usage(){
    echo -e "Usage:\n\t$0 [user@host] [source-directory]\n\t  user@host default: $DEF_USERHOST\n\t  source-directory default: $DEF_SRCDIR"
}

if [ -z "$1" ]; then
    USERHOST=$DEF_USERHOST
    SRCDIR=$DEF_SRCDIR
elif [ -z "$2" ] && [ -n "$1" ]; then
    USERHOST=$1
    SRCDIR=$DEF_SRCDIR
elif [ -n "$1" ] && [ -n "$1" ]; then
    USERHOST=$1
    SRCDIR=$2
fi

if [ -z "$(echo $USERHOST | grep \@)" ]; then
    usage
    exit 1
fi

if [ ! -d "$SRCDIR" ]; then
    usage
    exit 1 
fi

echo "Copying contents of '$SRCDIR' to '$USERHOST:rt-gumstix-tests'..."
rsync --exclude=".svn" -ra $SRCDIR $USERHOST:rt-gumstix-tests

