/**********************************************************************\
 *                         Oracion  assembler                         *
 *                                                                    *
 *             Copyright (C) 2023-2024 Alexander Nicholi.             *
 *            Copyright (C) 2024-2026 Xion Megatrends LLC.            *
 *            Released under Artisan Software Licence v1.1            *
\**********************************************************************/

#include "common.h"

struct bn3f_lexeme _bn3f_lex_whitespace( FILE * f )
{
	struct bn3f_lexeme r;
	int n;

	r.len   = 0;
	r.type  = BN3F_LEXEME_WHITESPACE;
	r.abort = 0;

	n = fgetc( f );

	if(n != '\t' && n != '\n' && n != '\v'
	&& n != '\f' && n != '\r' && n != ' ')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	/* count the previous character since it succeeded */
	r.len += 1;

	for(;;)
	{
		n = fgetc( f );

		if(n != '\t' && n != '\n' && n != '\v'
		&& n != '\f' && n != '\r' && n != ' ')
		{
			fseek( f, -1, SEEK_CUR );

			break;
		}

		r.len++;
	}

	return r;
}
