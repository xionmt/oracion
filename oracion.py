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

oracion abs [(-m|--map) (<mapfile.ini>|-)] (<input.os>|-) [<output.osa>]
\tTranslate high-level Oracion assembly (with labels, offsets and
\texternal symbols) to low-level "absolute assembly" (with only
\tinstructions, and relative or absolute numerics). If output.osa
\tis not provided, stdout is used.

oracion bin (<input.osa>|-) [<output.bin>]
\tTranslate low-level "absolute assembly" into machine code. If
\toutput.bin is not provided, stdout is used.
'''

INVALID_VERB = 'Invalid verb %s. Run oracion --help to see valid usage.'
INSUFFICIENT_ARGS = 'Verb %s needs at least one argument. ' + \
	'Run oracion --help to see valid\nusage.'
NO_MAPFILE_NAME = 'No filename given for map file positional ' + \
	'argument. Run oracion --help\nto see valid usage.'
NO_INPUT_NAME = 'No input filename given after map file argument. ' + \
	'Run oracion --help to\nsee valid usage.'
MAPFILE_INPUT_OVERLAP = 'Either the map file or the input file may ' + \
	'be given via stdin, but not\nboth. Run oracion --help to ' + \
	'see valid usage.'
MALFORMED_SECTION = 'Malformed section on line %i.'
EMPTY_SECTION_NAME = 'Empty section name on line %i.'
DUPLICATE_SECTION = 'Duplicate section name on line %i.'
ERRONEOUS_LINE = 'Line %i not a section heading, key-value pair or ' + \
	'whitespace/comment.'

def Lexeme:
	type: str
	value: str

	def __init__(self, text: str):


def lex(text: str) -> list[]

def parse_ini(text: str) -> dict[str, dict[str, str]]:
	import re
	expr_comment = re.compile(r'^\s*#.*$')
	lines: list[str] = text.replace('\r\n', '\n').replace('\r',
		'\n').split('\n')
	lines_sz: int = len(lines)
	i = 0
	section = ''
	ret: dict[str, dict[str, str]] = {'': {}}
	while i < lines_sz:
		if expr_comment.match(lines[i]):
			i += 1
			continue
		line = lines[i].lstrip().rstrip()
		if len(line) == 0:
			i += 1
			continue
		if line[0] == '[':
			if line[-1] != ']':
				print(MALFORMED_SECTION % (i + 1),
					file=stderr)
				return 5
			name = line[1:-1]
			if name == '':
				print(EMPTY_SECTION_NAME % (i + 1),
					file=stderr)
				return 6
			if name in ret:
				print(DUPLICATE_SECTION % (i + 1),
					file=stderr)
				return 7
			ret[name] = {}
			section = name
			i += 1
			continue
		if '=' not in lines[i]:
			print(ERRONEOUS_LINE % (i + 1), file=stderr)
			return 8
		pair = lines[i].split('=', 1)
		ret[section][pair[0]] = pair[1]
		i += 1
	return ret

def convert_abs(args: list[str], argc: int) -> int:
	resolvmap: dict[str, dict[str, str]]
	if args[0] == '-m' or args[0] == '--map':
		if argc < 2:
			from sys import stderr
			print(NO_MAPFILE_NAME, file=stderr)
			return 2
		if argc < 3:
			from sys import stderr
			print(NO_INPUT_NAME, file=stderr)
			return 3
		if args[1] == '-' and args[2] == '-':
			from sys import stderr
			print(MAPFILE_INPUT_OVERLAP, file=stderr)
			return 4
		if args[1] == '-':
			from sys import stdin
			t = stdin.buffer.read().decode('utf-8')
			resolvmap = parse_ini(t)
		else:
			f = open(args[1], 'rb')
			t = f.read().decode('utf-8')
			f.close()
			resolvmap = parse_ini(t)
	else:
		resolvmap = {'': {}}
	print(resolvmap)
	return 0

def main(args: list[str]) -> int:
	argc = len(args)
	if '-h' in args or '--help' in args or argc < 2:
		from sys import stderr
		print(HELP_TEXT, file=stderr)
		return 0
	if args[1] != 'abs' and args[1] != 'bin':
		from sys import stderr
		print(INVALID_VERB % args[1], file=stderr)
		return 1
	if argc < 3:
		print(INSUFFICIENT_ARGS % args[1], file=stderr)
	if args[1] == 'abs':
		return convert_abs(args[2:], argc - 2)
	# else args[1] == 'bin'
	return convert_bin(args[2:], argc - 2)

if __name__ == '__main__':
	from sys import argv, exit
	exit(main(argv))
