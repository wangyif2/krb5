/*
 * lib/krb5/os/read_msg.c
 *
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 * 
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 *
 * Write a message to the network
 */

#define NEED_SOCKETS
#include "k5-int.h"
#include <errno.h>

#ifndef ECONNABORTED
#define ECONNABORTED WSAECONNABORTED
#endif

krb5_error_code
krb5_read_message(context, fdp, inbuf)
    krb5_context context;
	krb5_pointer fdp;
	krb5_data	*inbuf;
{
	krb5_int32	len;
	int		len2, ilen;
	char		*buf = NULL;
	int		fd = *( (int *) fdp);
	
	if ((len2 = krb5_net_read(context, fd, (char *)&len, 4)) != 4)
		return((len2 < 0) ? errno : ECONNABORTED);
	len = ntohl(len);

	if ((len & VALID_UINT_BITS) != len)  /* Overflow size_t??? */
		return ENOMEM;

	inbuf->length = ilen = (int) len;
	if (ilen) {
		/*
		 * We may want to include a sanity check here someday....
		 */
		if (!(buf = malloc(ilen))) {
			return(ENOMEM);
		}
		if ((len2 = krb5_net_read(context, fd, buf, ilen)) != ilen) {
			krb5_xfree(buf);
			return((len2 < 0) ? errno : ECONNABORTED);
		}
	}
	inbuf->data = buf;
	return(0);
}
