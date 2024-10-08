# TODO: this shit is hacky AF replace it later
ALL: COMPILE RUN
COMPILE: src/*.c
	gcc src/*.c src/*/*.c -std=c89 -o build/psmplc
RUN:
	build/psmplc build/test.psmpl -d -o build/test.bin
