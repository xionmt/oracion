/**********************************************************************\
 *                         Oracion  assembler                         *
 *                                                                    *
 *             Copyright (C) 2023-2024 Alexander Nicholi.             *
 *            Copyright (C) 2024-2026 Xion Megatrends LLC.            *
 *            Released under Artisan Software Licence v1.1            *
\**********************************************************************/

#include "getopt2.h"

static ptri _strlen( chr * a )
{
	ptri r = 0;

	for(r = 0; a[r] != '\0'; ++r);

	return r;
}

static ptri _strleneqsign( chr * a )
{
	ptri r = 0;

	for(r = 0; a[r] != '\0' && a[r] != '='; ++r);

	return r;
}

static bl _strequ( chr * a, chr * b, ptri a_len, ptri b_len )
{
	ptri i;

	if(a_len != b_len)
	{
		return FALSE;
	}

	for(i = 0; i < a_len && a[i] == b[i]; ++i);

	return i >= a_len;
}

struct getopt2_args getopt2_normalise( int argc, char ** argv )
{
	struct getopt2_args r;

	r.argc = (ptri)argc;
	r.argv = (chr **)argv;

	return r;
}

static bl _islflag( chr * str )
{
	return str[0] == '-' && str[1] == '-' && str[2] != '\0';
}

static bl _isfstop( chr * str )
{
	return str[0] == '-' && str[1] == '-' && str[2] == '\0';
}

static bl _issflag( chr * str )
{
	return str[0] == '-' && str[1] != '-' && str[1] != '\0';
}

static bl _isstdin( chr * str )
{
	return str[0] == '-' && str[1] == '\0';
}

static chr * _getlflagval(
	struct getopt2_args args,
	ptri i,
	bl equsign )
{
	const ptri sz = _strleneqsign( args.argv[i] );

	/* additional checking is needed as we may perform lookahead */
	if((equsign && i >= args.argc) ||
	(!equsign && i + 1 >= args.argc))
	{
		return NULL;
	}

	/* several conditions are checked:
	 *  1. if equal sign flag-value delimiting is sought
	 *  2. if the equal sign is actually present in the flag
	 * if these hold, it returns an in-place substring of the value
	 */
	if(equsign && args.argv[i][sz] == '=')
	{
		/* advance past the '=' and return that as a string */
		return &(args.argv[i][sz + 1]);
	}

	/* if we reach HERE, it means either equal sign delimiting isn't
	 * used, or that there was no equal sign detected if requested
	 */

	/* return the next parameter as the value, if it is present.
	 * this function assumes the parameter is required, i.e. it does
	 * NOT check high-level flag structure to see if the flag given
	 * needs one */
	return i + 1 < args.argc
		? args.argv[i + 1]
		: NULL;
}

static bl _validate_lflag(
	getopt2_modeopt_t modeopt,
	struct getopt2_flag flag,
	getopt2_flagopt_t flagopt,
	struct getopt2_args args,
	ptri i )
{
	chr * const inflag = &(args.argv[i][2]);
	const bl flageq = (modeopt & GETOPT2_MODEOPT_MASK_LFLAGEQ);
	const ptri inlen = flageq
		? _strleneqsign( inflag )
		: _strlen( inflag );
	const ptri inlen_dumb = _strlen( inflag );
	const ptri outlen = _strlen( flag.l );

	if(!_strequ( inflag, flag.l, inlen, outlen ) ||
	!(modeopt & GETOPT2_MODEOPT_MASK_FLAGS))
	{
		return FALSE;
	}

	if((flagopt & GETOPT2_FLAGOPT_MASK_VALUED) &&
	(flagopt & GETOPT2_FLAGOPT_MASK_REQDVAL) &&
	(!flageq || inlen_dumb == inlen) &&
	i + i >= args.argc)
	{
		return FALSE;
	}

	return TRUE;
}

static bl _hnd_lflag(
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args,
	ptri i,
	struct getopt2_flagparam * ret )
{
	if(!_strequ( &(args.argv[i][2]), flag.l,
	(modeopt & GETOPT2_MODEOPT_MASK_LFLAGEQ)
		? _strleneqsign( &(args.argv[i][2]) )
		: _strlen( &(args.argv[i][2] ) ),
	_strlen( flag.l ) )
	|| !(modeopt & GETOPT2_MODEOPT_MASK_FLAGS))
	{
		return FALSE;
	}

	ret->found = 1;
	ret->value = _getlflagval( args, i, TRUE );

	return TRUE;
}

static bl _validate_sflag(
	getopt2_modeopt_t modeopt,
	struct getopt2_flag flag,
	getopt2_flagopt_t flagopt,
	struct getopt2_args args,
	ptri i )
{
	chr * const inflags = &(args.argv[i][1]);


	return TRUE;
}

static bl _hnd_sflag(
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args,
	ptri i,
	struct getopt2_flagparam * ret )
{
	const ptri sz = _strlen( args.argv[i] );
	ptri j;

	/* j = 1 to skip initial '-' character */
	for(j = 1; j < sz; ++j)
	{
		if(args.argv[i][j] == flag.s)
		{
			if((flagopt & GETOPT2_FLAGOPT_MASK_REQDVAL)
			&& j + 1 < sz)
			{
				return FALSE;
			}

			if((flagopt & GETOPT2_FLAGOPT_MASK_VALUED)
			&& j + 1 == sz && i + 1 < args.argc)
			{
				ret->value = args.argv[i + 1];
			}

			ret->found = 1;

			return TRUE;
		}
	}

	return FALSE;
}

static bl _isflagval( struct getopt2_args args,
	ptri i,
	getopt2_modeopt_t modeopt,
	struct getopt2_flag * flags,
	getopt2_flagopt_t * flagopts,
	ptri flags_sz )
{
	ptri j;
	struct getopt2_flagparam dummy;

	if(i <= 1)
	{
		return FALSE;
	}

	/* no need to check for equal-sign using parameters,
	 * as we are backtracking */
	modeopt &= ~GETOPT2_MODEOPT_MASK_LFLAGEQ;

	for(j = 0; j < flags_sz; ++i)
	{
		if(!(flagopts[j] & GETOPT2_FLAGOPT_MASK_VALUED))
		{
			continue;
		}

		if(_hnd_lflag( modeopt, flagopts[j], flags[j], args,
		i - 1, &dummy ) || _hnd_sflag( modeopt, flagopts[j],
		flags[j], args, i - 1, &dummy ))
		{
			return TRUE;
		}
	}

	return FALSE;
}

struct getopt2_flagparam getopt2_getflag(
	chr * * verbs,
	getopt2_modeopt_t modeopt,
	getopt2_flagopt_t flagopt,
	struct getopt2_flag flag,
	struct getopt2_args args )
{
	ptri i;
	struct getopt2_flagparam ret;

	ret.found = 0;
	ret.value = NULL;

	return ret;
}
