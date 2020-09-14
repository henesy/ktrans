</$objtype/mkfile

TARG=ktrans

OFILES=main.$O jisho.$O

HFILES=jisho.h ktrans.h

BIN=/$objtype/bin

</sys/src/cmd/mkone

installfull: install
	mkdir /sys/src/cmd/ktrans
	dircp ./ /sys/src/cmd/ktrans
	cp ktrans.man /sys/man/1/ktrans
	echo 'Notice: For a user to use ktrans they must have $home/lib/ktrans-jisho copied from /sys/src/cmd/ktrans!'

uninstall:
	rm /sys/man/1/ktrans
	rm -r /sys/src/cmd/ktrans
	rm /$objtype/bin/ktrans
