#!/usr/bin/env python3
#

HELP_TEXT = '''
Oracion assembler
Copyright (C) 2023-2024 Alexander Nicholi.
Copyright (C) 2024-2026 Xion Megatrends LLC.
All rights reserved.

Released under Artisan Software Licence v1.1.

Usage:-
oracion (-h|--help)
\tDisplay help message and exit.

oracion abs [(-m|--map) <mapfile.ini>] (<input.os>|-) [<output.osa>]
\tTranslate high-level Oracion assembly (with labels, offsets and
\texternal symbols) to low-level "absolute assembly" (with only
\tinstructions, and relative or absolute numerics). If output.osa
\tis not provided, stdout is used.

oracion bin (<input.osa>|-) [<output.bin>]
\tTranslate low-level "absolute assembly" into machine code. If
\toutput.bin is not provided, stdout is used.
'''

INVALID_VERB = 'Invalid verb %s. Run oracion --help to see valid usage.'

def convert(t: str):
	ret = bytearray()
	return bytes(ret)

def main(args):
	argc = len(args)
	if '-h' in args or '--help' in args or argc < 2:
		from sys import stderr
		print(HELP_TEXT, file=stderr)
		return 0
	if args[1] != 'abs' and args[1] != 'bin':
		from sys import stderr
		print(INVALID_VERB % args[1], file=stderr)
		return 1
	from sys import stdin, stdout
	intext = stdin.read()
	outbin = convert(intext)
	stdout.buffer.write(outbin)
	return 0

if __name__ == '__main__':
	from sys import argv, exit
	exit(main(argv))
