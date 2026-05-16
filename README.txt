
build should be straight forward and even with c89

cc -std=c89 sread.c -o sread -s -Os -Wall

copy the binary into your $PATH
copy sread.1 into your manpath. 
I encourage $HOME/man/man1  

How to get this done depends on your Distro..


sread(1)               Terminal sleep read               sread(1)

NAME
       sread - terminal sleep read

SYNOPSIS
       sread -t <time-in-seconds> [-p text]

DESCRIPTION
       sread (sleep read) is a small C program that waits for a single
       keypress without requiring the Enter key. When a key is pressed,
       sread writes that key to stdout and ends the sleep early. If no
       key is pressed within the specified time, sread exits after the
       timeout.

       sread behaves like the standard sleep command, but it allows the
       sleep to be interrupted by a single keypress and returns that key
       on stdout. Fractional seconds are supported, for example:
           sread -t 1.22

OPTIONS
       -h
           Show a short usage message.

       -t time
           Time to sleep in seconds; fractional values are allowed.

       -p text
           Display text as statusbar.

EXAMPLES
       while true
       do
         url=$(selectmusicstream)
         duration=$(send_to_upnp_speaker "${url}")
         key=$(sread -t "${duration}" -p "+/Vol up -/Vol down")

         if [ "${key}" = "q" ]; then break; fi
         if [ "${key}" = "+" ]; then incvolume; fi
         if [ "${key}" = "-" ]; then decvolume; fi
       done

BUGS
       Unknown; probably.

AUTHOR
       Written by Manfred Güntner.

REPORTING BUGS
       https://github.com/manfredguentner/sread/issues

COPYRIGHT
       Copyright © 2026 Manfred Güntner
       This software is distributed under the BSD 2-Clause License; see
       the LICENSE file.

sread 1.0               2026-05-13               sread 1.0
