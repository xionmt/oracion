/**********************************************************************\
 *                         Oracion  assembler                         *
 *                                                                    *
 *             Copyright (C) 2023-2024 Alexander Nicholi.             *
 *            Copyright (C) 2024-2026 Xion Megatrends LLC.            *
 *            Released under Artisan Software Licence v1.1            *
\**********************************************************************/

#include "common.h"

struct bn3f_lexeme ** bn3f_lex( FILE * f )
{
	int r;
	size_t streamoffs = 0;
	struct bn3f_lexeme ** lexemes;
	size_t lexemes_sz = 16;

	r = fseek( f, 0, SEEK_SET );

	if(r)
	{
		fprintf( stderr,
		"Error: file stream given is not seekable (are you using stdin?)\n" );
	}

	lexemes = calloc( lexemes_sz, sizeof(struct bn3f_lexeme *) );

	for(;;)
	{
		r = _bn3f_lex_loopiter( f, &streamoffs, &lexemes, &lexemes_sz );

		if(r)
		{
			break;
		}
	}

	return lexemes;
}
