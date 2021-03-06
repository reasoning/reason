
/**
 * Reason
 * Copyright (C) 2009  Emerson Clarke
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PROTOCOL_SMTP_H
#define PROTOCOL_SMTP_H

#include "reason/system/string.h"
#include "reason/network/socket.h"

using namespace Reason::Network;
using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Network { namespace Smtp {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SmtpClient
{
public:

	Socket SmtpSocket;

	void Connect(Address &address);
	void Send(String &from,String &to,String &message);
	void Disconnect();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SmtpServer
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SmtpTransaction
{

};

class SmtpCommand
{
public:

	void Issue(Sequence &command);
	void Accept(Sequence &reply);

	void Mail(Sequence &from)
	{
		String command;
		command << "MAIL FROM:<" << from << ">\r\n";

		Issue(command);

		String reply;
		Accept(reply);
	}

	void Recipient(Sequence &recipient)
	{
		String command;
		command << "RCPT TO:<" << recipient << ">\r\n";
		Issue(command);
	}

};

class SmtpReply
{
public:

	enum SmtpReplyType
	{
		SMTP_REPLY_STATUS=0,
		SMTP_REPLY_HELP,
		SMTP_REPLY_SERVICE_READY,
		SMTP_REPLY_SERVICE_CLOSING,
		SMTP_REPLY_OK,
		SMTP_REPLY_USER_NOT_LOCAL_WILL_FORWARD,
		SMTP_REPLY_READY,
		SMTP_REPLY_SERVICE_UNAVAILABLE,
		SMTP_REPLY_ABORTED_MAILBOX_UNAVAILABLE,
		SMTP_REPLY_ABORTED_PROCESSING_ERROR,
		SMTP_REPLY_ABORTED_INSUFICIENT_STORAGE,
		SMTP_REPLY_COMMAND_UNRECOGNISED,
		SMTP_REPLY_COMANND_INVALID,
		SMTP_REPLY_COMMAND_NOT_IMPLEMENTED,
		SMTP_REPLY_COMMAND_OUT_OF_SEQUENCE,
		SMTP_REPLY_COMMAND_PARAMETER_NOT_IMPLEMENTED,
		SMTP_REPLY_ABORTED_MAILBOX_INACCESABLE,
		SMTP_REPLY_USER_NOT_LOCAL_CANT_FORWARD,
		SMTP_REPLY_ABORTED_STORAGE_EXCEEDED,
		SMTP_REPLY_ABORTED_MAILBOX_NOT_ALLOWED,
		SMTP_REPLY_FAILED,
	};

	static const struct SmtpReplyStruct
	{
		char *Number;
		char *Phrase;
	} ReplyCode[];
	static const int ReplyCodeCount;

	bool Success();
	bool Failure();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SmtpTransport
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

