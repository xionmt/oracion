/**********************************************************************\
 *                         Oracion  assembler                         *
 *                                                                    *
 *             Copyright (C) 2023-2024 Alexander Nicholi.             *
 *            Copyright (C) 2024-2026 Xion Megatrends LLC.            *
 *            Released under Artisan Software Licence v1.1            *
\**********************************************************************/

#include "common.h"

struct bn3f_lexeme _bn3f_lex_comment( FILE * f )
{
	struct bn3f_lexeme r;
	int n[2];

	r.len   = 0;
	r.type  = BN3F_LEXEME_COMMENT;
	r.abort = 0;

	n[0] = fgetc( f );

	if(n[0] != '/')
	{
		fseek( f, -1, SEEK_CUR );

		return r;
	}

	n[1] = fgetc( f );

	if(n[1] != '*')
	{
		fseek( f, -2, SEEK_CUR );

		return r;
	}

	/* default state is meaningless to the logic of the loop that follows */
	n[0] = '\0';
	n[1] = '\0';

	/* count the opening characters */
	r.len += 2;

	for(;;)
	{
		n[r.len & 1] = fgetc( f );

		if(n[r.len & 1] == EOF)
		{
			r.abort = 1;

			return r;
		}

		if(n[r.len & 1] == '/' && n[(r.len - 1) & 1] == '*')
		{
			r.len++;

			break;
		}

		r.len++;
	}

	return r;
}
