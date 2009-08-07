/* Copyright (c) 2002-2009 Dovecot Sieve authors, see the included COPYING file
 */

#ifndef __SIEVE_MESSAGE_H
#define __SIEVE_MESSAGE_H

/* 
 * Message transmission
 */

const char *sieve_message_get_new_id
	(const struct sieve_script_env *senv);

/* 
 * Message context 
 */

struct sieve_message_context;

struct sieve_message_context *sieve_message_context_create
	(const struct sieve_message_data *msgdata);
void sieve_message_context_ref(struct sieve_message_context *msgctx);
void sieve_message_context_unref(struct sieve_message_context **msgctx);

void sieve_message_context_flush(struct sieve_message_context *msgctx);

pool_t sieve_message_context_pool
	(struct sieve_message_context *msgctx);

/* Extension support */

void sieve_message_context_extension_set
	(struct sieve_message_context *msgctx, const struct sieve_extension *ext, 
		void *context);
const void *sieve_message_context_extension_get
	(struct sieve_message_context *msgctx, const struct sieve_extension *ext);

/* Envelope */

const struct sieve_address *sieve_message_get_recipient_address
	(struct sieve_message_context *msgctx);

const struct sieve_address *sieve_message_get_sender_address
	(struct sieve_message_context *msgctx);

const char *sieve_message_get_recipient
	(struct sieve_message_context *msgctx);

const char *sieve_message_get_sender
	(struct sieve_message_context *msgctx);
	
#endif /* __SIEVE_MESSAGE_H */
