/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */

#include "msgCore.h"

#include "nsISupports.h"
#include "nsCOMPtr.h"

#include "nsIFactory.h"
#include "nsIGenericFactory.h"
#include "nsIServiceManager.h"
#include "nsIModule.h"

#include "pratom.h"
#include "nsMsgCompCID.h"



/* Include all of the interfaces our factory can generate components for */
#include "nsMsgSendFact.h"
#include "nsMsgCompFieldsFact.h"
#include "nsMsgSendLaterFact.h"

#include "nsMsgComposeFact.h"
#include "nsMsgSendLater.h"
#include "nsSmtpUrl.h"
#include "nsISmtpService.h"
#include "nsSmtpService.h"
#include "nsMsgComposeService.h"
#include "nsMsgCompose.h"
#include "nsMsgSend.h"
#include "nsMsgQuote.h"
#include "nsIMsgDraft.h"
#include "nsMsgCreate.h"    // For drafts...I know, awful file name...
#include "nsSmtpServer.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsSmtpService);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSmtpServer);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgCompose);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgCompFields);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgComposeAndSend);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgSendLater);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgDraft)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgComposeService);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgQuote);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsMsgQuoteListener);
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSmtpUrl);

////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////

static nsModuleComponentInfo components[] =
{
  { "Msg Compose",
    NS_MSGCOMPOSE_CID,
    NS_MSGCOMPOSE_PROGID,
    nsMsgComposeConstructor },
  { "Msg Compose Service",
    NS_MSGCOMPOSESERVICE_CID,
    NS_MSGCOMPOSESERVICE_PROGID,
    nsMsgComposeServiceConstructor },
  { "Msg Compose Fields",
    NS_MSGCOMPFIELDS_CID,
    NS_MSGCOMPFIELDS_PROGID,
    nsMsgCompFieldsConstructor },
  { "Msg Draft",
    NS_MSGDRAFT_CID,
    NS_MSGDRAFT_PROGID,
    nsMsgDraftConstructor },
  { "Msg Send",
    NS_MSGSEND_CID,
    NS_MSGSEND_PROGID,
    nsMsgComposeAndSendConstructor },
  { "Msg Send Later",
    NS_MSGSENDLATER_CID,
    NS_MSGSENDLATER_PROGID,
    nsMsgSendLaterConstructor },
  { "SMTP Service",
    NS_SMTPSERVICE_CID,
    NS_SMTPSERVICE_PROGID,
    nsSmtpServiceConstructor },
  { "SMTP Service",
    NS_SMTPSERVICE_CID,
    NS_MAILTOHANDLER_PROGID,
    nsSmtpServiceConstructor },
  { "SMTP Server",
    NS_SMTPSERVER_CID,
    NS_SMTPSERVER_PROGID,
    nsSmtpServerConstructor },
  { "SMTP URL",
    NS_SMTPURL_CID,
    NS_SMTPURL_PROGID,
    nsSmtpUrlConstructor },
  { "Msg Quote",
    NS_MSGQUOTE_CID,
    NS_MSGQUOTE_PROGID,
    nsMsgQuoteConstructor },
  { "Msg Quote Listener",
    NS_MSGQUOTELISTENER_CID,
    NS_MSGQUOTELISTENER_PROGID,
    nsMsgQuoteListenerConstructor }
};

  
NS_IMPL_NSGETMODULE("nsMsgComposeModule", components)
