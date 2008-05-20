#include <stdio.h>
#include <string.h>

#include "lib.h"
#include "str.h"
#include "mempool.h"
#include "ostream.h"

#include "sieve-extensions.h"
#include "sieve-commands-private.h"
#include "sieve-code.h"
#include "sieve-actions.h"
#include "sieve-generator.h"
#include "sieve-binary.h"
#include "sieve-result.h"
#include "sieve-comparators.h"

#include "sieve-dump.h"

struct sieve_code_dumper {
	pool_t pool;
					
	/* Dump status */
	sieve_size_t pc;          /* Program counter */
	
	const struct sieve_operation *operation;
	sieve_size_t mark_address;
	unsigned int indent;
	
	/* Dump environment */
	struct sieve_dumptime_env *dumpenv; 
};

struct sieve_code_dumper *sieve_code_dumper_create
	(struct sieve_dumptime_env *denv) 
{
	pool_t pool;
	struct sieve_code_dumper *dumper;
	
	pool = pool_alloconly_create("sieve_code_dumper", 4096);	
	dumper = p_new(pool, struct sieve_code_dumper, 1);
	dumper->pool = pool;
	dumper->dumpenv = denv;
	dumper->pc = 0;
	
	return dumper;
}

void sieve_code_dumper_free(struct sieve_code_dumper **dumper) 
{
	pool_unref(&((*dumper)->pool));
	
	*dumper = NULL;
}

pool_t sieve_code_dumper_pool(struct sieve_code_dumper *dumper)
{
	return dumper->pool;
}

/* Dump functions */

void sieve_code_dumpf
(const struct sieve_dumptime_env *denv, const char *fmt, ...)
{
	struct sieve_code_dumper *cdumper = denv->cdumper;	
	unsigned tab = cdumper->indent;
	 
	string_t *outbuf = t_str_new(128);
	va_list args;
	
	va_start(args, fmt);	
	str_printfa(outbuf, "%08x: ", cdumper->mark_address);
	
	while ( tab > 0 )	{
		str_append(outbuf, "  ");
		tab--;
	}
	
	str_vprintfa(outbuf, fmt, args);
	str_append_c(outbuf, '\n');
	va_end(args);
	
	o_stream_send(denv->stream, str_data(outbuf), str_len(outbuf));
}

void sieve_code_mark(const struct sieve_dumptime_env *denv)
{
	denv->cdumper->mark_address = denv->cdumper->pc;
}

void sieve_code_mark_specific
(const struct sieve_dumptime_env *denv, sieve_size_t location)
{
	denv->cdumper->mark_address = location;
}

void sieve_code_descend(const struct sieve_dumptime_env *denv)
{
	denv->cdumper->indent++;
}

void sieve_code_ascend(const struct sieve_dumptime_env *denv)
{
	if ( denv->cdumper->indent > 0 )
		denv->cdumper->indent--;
}

/* Operations and operands */

bool sieve_code_dumper_print_optional_operands
	(const struct sieve_dumptime_env *denv, sieve_size_t *address)
{
	int opt_code;
	
	if ( sieve_operand_optional_present(denv->sbin, address) ) {
		while ( opt_code != 0 ) {
			if ( !sieve_operand_optional_read(denv->sbin, address, &opt_code) )
				return FALSE;

			if ( opt_code == SIEVE_OPT_SIDE_EFFECT ) {
				if ( !sieve_opr_side_effect_dump(denv, address) )
					return FALSE;
			}
		}
	}
	return TRUE;
}
 
/* Code Dump */

static bool sieve_code_dumper_print_operation
	(struct sieve_code_dumper *dumper) 
{	
	const struct sieve_operation *op;
	struct sieve_dumptime_env *denv = dumper->dumpenv;
	sieve_size_t address;
	const char *opcode_string;
	
	/* Mark start address of operation */
	dumper->indent = 0;
	address = dumper->mark_address = dumper->pc;

	/* Read operation */
	dumper->operation = op = 
		sieve_operation_read(denv->sbin, &(dumper->pc));

	/* Try to dump it */
	if ( op != NULL ) {
		if ( op->dump != NULL )
			return op->dump(op, denv, &(dumper->pc));
		else if ( op->mnemonic != NULL )
			sieve_code_dumpf(denv, "%s", op->mnemonic);
		else
			return FALSE;
			
		return TRUE;
	}		
	
	opcode_string = sieve_operation_read_string(denv->sbin, &address);

	if ( opcode_string != NULL )
		sieve_code_dumpf(denv, "Unknown opcode: %s", opcode_string);
	else
		sieve_code_dumpf(denv, "Failed to read opcode.");
	return FALSE;
}

void sieve_code_dumper_run(struct sieve_code_dumper *dumper) 
{
	struct sieve_binary *sbin = dumper->dumpenv->sbin;
	
	dumper->pc = 0;
	
	while ( dumper->pc < 
		sieve_binary_get_code_size(sbin) ) {
		if ( !sieve_code_dumper_print_operation(dumper) ) {
			sieve_code_dumpf(dumper->dumpenv, "Binary is corrupt.");
			return;
		}
	}
	
	/* Mark end of the binary */
	dumper->indent = 0;
	dumper->mark_address = sieve_binary_get_code_size(sbin);
	sieve_code_dumpf(dumper->dumpenv, "[End of code]");	
}
