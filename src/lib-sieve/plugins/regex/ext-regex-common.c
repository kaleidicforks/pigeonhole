#include "sieve-common.h"
#include "sieve-match-types.h"

#include "ext-regex-common.h"

/* 
 * Regex match type operand
 */

static const struct sieve_extension_obj_registry ext_match_types =
    SIEVE_EXT_DEFINE_MATCH_TYPE(regex_match_type);

const struct sieve_operand regex_match_type_operand = {
    "regex match",
    &regex_extension,
    0,
    &sieve_match_type_operand_class,
    &ext_match_types
};
