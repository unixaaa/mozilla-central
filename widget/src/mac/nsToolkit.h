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

#ifndef TOOLKIT_H      
#define TOOLKIT_H

#include "nsIToolkit.h"
#include "nsRepeater.h"

#include "nsCOMPtr.h"

class nsIEventQueue;
class nsIEventQueueService;

/**
 * The toolkit abstraction is necessary because the message pump must
 * execute within the same thread that created the widget under Win32.
 * We don't care about that on Mac: we have only one thread for the UI
 * and maybe even for the whole application.
 * 
 * So on the Mac, the nsToolkit used to be a unique object, created once
 * at startup along with nsAppShell and passed to all the top-level
 * windows and it became a convenient place to throw in everything we
 * didn't know where else to put, like the NSPR event queue and
 * the handling of global pointers on some special widgets (focused
 * widget, widget hit, widget pointed).
 *
 * All this has changed: the application now usually creates one copy of
 * the nsToolkit per window and the special widgets had to be moved
 * to the nsMacEventHandler. Also, to avoid creating several repeaters,
 * the NSPR event queue has been moved to a global object of its own:
 * the nsMacNSPREventQueueHandler declared below.
 *
 * If by any chance we support one day several threads for the UI
 * on the Mac, will have to create one instance of the NSPR
 * event queue per nsToolkit.
 */

#include <MacTypes.h>

class nsToolkit : public nsIToolkit
{

public:
  nsToolkit();
  virtual				~nsToolkit();
  
  NS_DECL_ISUPPORTS
    
  NS_IMETHOD  	Init(PRThread *aThread);
  
  // are there pending events on the toolkit's event queue?
  PRBool        ToolkitBusy();
  
public:

  // Appearance Mgr
  static bool 	HasAppearanceManager();
  
  // helpers to determine if the app is in the fg or bg
  static void AppInForeground ( ) ;
  static void AppInBackground ( ) ;
  static bool IsAppInForeground ( ) ;

protected:
  bool          mInited;
  static bool   sInForeground;
};


class nsMacNSPREventQueueHandler : public Repeater
{
public:
									nsMacNSPREventQueueHandler();
	virtual					~nsMacNSPREventQueueHandler();

	virtual	void		StartPumping();
	virtual	PRBool	StopPumping();

	// Repeater interface
	virtual	void		RepeatAction(const EventRecord& inMacEvent);

  PRBool          EventsArePending();
  
protected:

	void						ProcessPLEventQueue();
	
protected:

	nsrefcnt								mRefCnt;

	nsCOMPtr<nsIEventQueueService>			mEventQueueService;
};


// class for low memory detection and handling
class nsMacMemoryCushion : public Repeater
{
public:

  enum {
    kMemoryBufferSize       = 64 * 1024,  // 64k reserve, purgeable handle purged first
    kMemoryReserveSize      = 32 * 1024   // 32k memory reserve, freed by the GrowZoneProc
  };
  
                nsMacMemoryCushion();
                ~nsMacMemoryCushion();
                
  OSErr         Init(Size bufferSize, Size reserveSize);


  void          RepeatAction(const EventRecord &aMacEvent);

protected:

                // reallocate the memory buffer. Returns true on success
  Boolean       RecoverMemoryBuffer(Size bufferSize);

                // allocate or recover the memory reserve. Returns true on success
  Boolean       RecoverMemoryReserve(Size reserveSize);


public:

  static pascal long  GrowZoneProc(Size amountNeeded);
  
  
protected:

  static Handle  sMemoryReserve;
  
  Handle         mBufferHandle;     // this is first to go

};




#endif  // TOOLKIT_H
