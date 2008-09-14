/* Copyright (c) 2002-2008 Dovecot Sieve authors, see the included COPYING file
 */

#include "sieve-common.h"
#include "sieve-script.h"
#include "sieve-commands.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-interpreter.h"
#include "sieve-code.h"
#include "sieve-binary.h"
#include "sieve-dump.h"
#include "sieve.h"

#include "testsuite-common.h"

/*
 * Test_execute command
 *
 * Syntax:   
 *   test_execute
 */

static bool tst_test_execute_generate
	(const struct sieve_codegen_env *cgenv, struct sieve_command_context *ctx);

const struct sieve_command tst_test_execute = { 
	"test_execute", 
	SCT_TEST, 
	0, 0, FALSE, FALSE,
	NULL, NULL, NULL,
	tst_test_execute_generate, 
	NULL 
};

/* 
 * Operation 
 */

static int tst_test_execute_operation_execute
	(const struct sieve_operation *op, 
		const struct sieve_runtime_env *renv, sieve_size_t *address);

const struct sieve_operation test_execute_operation = { 
	"TEST_EXECUTE",
	&testsuite_extension, 
	TESTSUITE_OPERATION_TEST_EXECUTE,
	NULL, 
	tst_test_execute_operation_execute 
};

/* 
 * Code generation 
 */

static bool tst_test_execute_generate
(const struct sieve_codegen_env *cgenv, 
	struct sieve_command_context *tst ATTR_UNUSED)
{
	sieve_operation_emit_code(cgenv->sbin, &test_execute_operation);

	return TRUE;
}

/*
 * Intepretation
 */

static int tst_test_execute_operation_execute
(const struct sieve_operation *op ATTR_UNUSED,
	const struct sieve_runtime_env *renv, 
	sieve_size_t *address ATTR_UNUSED)
{
	bool result = TRUE;

	/*
	 * Perform operation
	 */

	result = testsuite_script_execute(renv);

	/* Set result */
	sieve_interpreter_set_test_result(renv->interp, result);

	return SIEVE_EXEC_OK;
}



