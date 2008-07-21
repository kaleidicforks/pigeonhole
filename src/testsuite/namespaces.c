/* Copyright (c) 2005-2007 Dovecot authors, see the included COPYING file */

#include "lib.h"
#include "istream.h"
#include "istream-seekable.h"
#include "fd-set-nonblock.h"
#include "str.h"
#include "str-sanitize.h"
#include "strescape.h"
#include "message-address.h"
#include "raw-storage.h"
#include "mbox-storage.h"
#include "maildir-storage.h"
#include "mail-namespace.h"

#include "namespaces.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

#define DEFAULT_ENVELOPE_SENDER "MAILER-DAEMON"

/* Hideous .... */

extern struct mail_storage raw_storage;
extern struct mail_storage maildir_storage;
extern struct mail_storage mbox_storage;
void mail_storage_register_all(void) {
	mail_storage_class_register(&raw_storage);
	mail_storage_class_register(&mbox_storage);
	mail_storage_class_register(&maildir_storage);
}

extern struct mailbox_list maildir_mailbox_list;
extern struct mailbox_list fs_mailbox_list;
void index_mailbox_list_init(void);
void mailbox_list_register_all(void) {
mailbox_list_register(&maildir_mailbox_list);
mailbox_list_register(&fs_mailbox_list);
index_mailbox_list_init();
}

static pool_t namespaces_pool;

pool_t namespaces_init(void) 
{
	mail_storage_init();
	mail_storage_register_all();
	mailbox_list_register_all();

	namespaces_pool = pool_alloconly_create("namespaces", 1024);

	return namespaces_pool;
}	
	
void namespaces_deinit(void)
{
	mail_storage_deinit();
	pool_unref(&namespaces_pool);
}
