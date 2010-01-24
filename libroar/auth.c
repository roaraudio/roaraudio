//auth.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

/* How auth works:
 * 0) set stage to zero
 * 1) get server address and local node name (from uname())
 * 2) look up authfile/authdb/authservice for the server+local address + stage.
 *    if no data was found send NONE-Auth.
 * 3) send data to server
 * 4) read answer from server
 * 5) if stage of server response is non-zero increment stage to server stage+1
 *    and repeat from step 2)
 * 6) check if we got an OK or an ERROR, return currect value
 */

/* The protocol:
 * Auth request:
 * Byte 0: auth type
 * Byte 1: stage
 * Byte 2: reserved (must be zero)
 * Byte 3: reserved (must be zero)
 * Byte 4-end: auth type depending data.
 *
 * If no data is to be send bytes 2 and 3 can be omitted.
 * If no data is to be send and stage is zero bytes 1, 2 and 3 can be omitted.
 *
 * Auth response:
 * The same as the auth request.
 * if the message type is OK the server accepted our auth.
 * if the message type is ERROR the server recjected us. we may try other auth methodes.
 * if the server accepted our data and the stage is non-zero we need to continue with the next
 * stage of the auth.
 * if the server rejected us the auth type value of the response is a suggested next auth type
 * we should try if possible. This may help the client to find a working auth type.
 */

/* The protocol by auth type:
 *
 * --- NONE:
 * No data is send, the server accepts the connect or rejects it depending on some
 * magic within the server. we do not care about this.
 * The data block is not used.
 *
 * --- COOKIE:
 * We send cookies for all stages the server ask us to provide a cookie.
 * if a cookie is wrong the server rejects us or aks us for another.
 * The cookie is send as binary data in the data block.
 *
 * --- TRUST:
 * We ask the server to auth us based on ower UID/GID/PID.
 * The server may reject this becasue we are not allowed or because it is not
 * supported by the transport.
 * If we get rejected we may try to continue with IDENT then RHOST before we use NONE.
 * The data block is not used.
 *
 * --- PASSWORD:
 * This is technikly the same as COOKIE just that the cookie is limited to
 * printable ASCII chars and that the user should be asked to provide the password.
 * This may be done via a GUI popup window.
 *
 * --- SYSUSER:
 * We provide a Username + Password for a system user.
 * The data block contains of two main parts:
 * The first part is a one byte long subtype.
 * The value must be 0x01 for username+password.
 * futur versions may define other types.
 * the secund part is the actual data block.
 * for username+password it is splited into two fields, both terminated with \0.
 * the first is the username the last one the password as clear text.
 * Example: char data[] = "\001MyUser\0MyPassword\0";
 * 
 * --- OPENPGP_SIGN:
 *
 * --- OPENPGP_ENCRYPT:
 *
 * --- OPENPGP_AUTH:
 *
 * --- KERBEROS:
 * We use Kerberos to auth.
 *
 * --- RHOST:
 * The server is asked to auth us based on our source address.
 * The data block is not used.
 *
 * --- XAUTH:
 * We send an X11 Cookie.
 *
 * --- IDENT:
 * The server is asked to auth us based on our source address using the IDENT protocol.
 * The data block is not used.
 *
 */

int roar_auth   (struct roar_connection * con) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd     = ROAR_CMD_AUTH;
 mes.datalen = 0;

 return roar_req(con, &mes, NULL);
}

//ll
