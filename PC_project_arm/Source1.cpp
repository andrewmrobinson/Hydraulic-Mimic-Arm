hCom = ::CreateFile(_T("\\\\.\\COM6"),             // filename
	GENERIC_READ | GENERIC_WRITE,  // desired access
	0,                             // exclusive
	NULL,                          // security irrelevant
	OPEN_EXISTING,                 // it has to be there
	FILE_FLAG_OVERLAPPED,          // open asynchronous
	NULL);                         // template file

/*******************************************************************************
*                                UWM_SEND_DATA
* Inputs:
*       WPARAM: (WPARAM)(CString *) string to send
*       LPARAM: unused
* Result: void
*       This is only called for PostThreadMessage calls and returns no value
* Effect:
*       Queues up a message for transmission
*******************************************************************************/

static UINT UWM_SEND_DATA = ::RegisterWindowMessage(_T("UWM_SEND_DATA-{CC871902-DB13-4330-9E43-C03C6B8CBE45}");
/*******************************************************************************
*                           UWM_WRITER_SHUTTING_DOWN
* Inputs:
*       WPARAM: Error code (ERROR_SUCCESS if not an error)
*       LPARAM: unused
* Result: LRESULT
*       Logically void, 0, always
* Effect:
*       Notifies the main thread that the writer is quitting
*******************************************************************************/
static UINT UWM_WRITER_SHUTTING_DOWN = ::RegisterWindowMessage(_T("UWM_WRITER_SHUTTING_DOWN-{CC871902-DB13-4330-9E43-C03C6B8CBE45}");

/*******************************************************************************
*                           UWM_READER_SHUTTING_DOWN
* Inputs:
*       WPARAM: Error code (ERROR_SUCCESS if not an error)
*       LPARAM: unused
* Result: LRESULT
*       Logically void, 0, always
* Effect:
*       Notifies the main thread that the writer is quitting
*******************************************************************************/
static UINT UWM_READER_SHUTTING_DOWN = ::RegisterWindowMessage(_T("UWM_READER_SHUTTING_DOWN-{CC871902-DB13-4330-9E43-C03C6B8CBE45}");

/*******************************************************************************
*                           UWM_DATA_READ
* Inputs:
*       WPARAM: (WPARAM)(CStringA *) The data that was read
*       LPARAM: unused
* Result: LRESULT
*       Logically void, 0, always
* Effect:
*       Notifies the main thread that the writer is quitting
*******************************************************************************/
static UINT UWM_DATA_READ = ::RegisterWindowMessage(_T("UWM_DATA_READ-{CC871902-DB13-4330-9E43-C03C6B8CBE45}");

/*******************************************************************************
*                           UWM_DATA_WRITTEN
* Inputs:
*       WPARAM: unused
*       LPARAM: unused
* Result: LRESULT
*       Logically void, 0, always
* Effect:
*       Notifies the main thread that the writer is quitting
*******************************************************************************/
static UINT UWM_DATA_WRITTEN = ::RegisterWindowMessage(_T("UWM_DATA_WRITTEN-{CC871902-DB13-4330-9E43-C03C6B8CBE45}");

class SerialParameters {
public:
	SerialParameters() { hCom = NULL; notifyee = NULL; shutdown = NULL; }
	SerialParameters(HANDLE h, CWnd * w, HANDLE sd) {
		hCom = h; notifyee = w; shutdown = sd;
	}
	HANDLE hCom;
	CWnd * notifyee;
	HANDLE shutdown;
};

class SerialWriter : public CWinThread {
	... usual ClassWizard stuff...
public:
	SerialParameters * parms;
protected:
	void OnSendData(WPARAM, LPARAM); // see below
	HANDLE WriteEvent;               // see below
};

//To create the thread, do

/* SerialWriter * */ writer = (SerialWriter *)AfxBeginThread(RUNTIME_CLASS(SerialWriter),
	THREAD_PRIORITY_NORMAL, // priority 
	0,                      // default stack size
	CREATE_SUSPENDED);      // don't run right away

if (writer == NULL)
{ /* deal with error */
	
} /* deal with error */

static UINT ReaderThread(LPVOID p);

writer->parms = new SerialParameters(...);
writer->ResumeThread();   // now let it run  

StringA * msg = new CStringA(whatever_I_want_to_send);
writer->PostThreadMessage(UWM_SEND_DATA, (WPARAM)msg);
AfxBeginThread(ReaderThread, writer->parms);

void SerialWriter::OnSendData(WPARAM wParam, LPARAM)
{
	CStringA * s = (CStringA *)wParam;

	OVERLAPPED ovl = { 0 };
	ovl.hEvent = WriteEvent;

	DWORD bytesWritten;
	BOOL ok = ::WriteFile(parms->hCom,         // handle
		(LPCSTR)*s,          // 8-bit data
		s->GetLength(),      // length
		&bytesWritten,       // amount written
		&ovl);               // overlapped structure
	if (!ok)
	{ /* failed */
		DWORD err = ::GetLastError();
		if (err != ERROR_IO_PENDING)
		{ /* serious error */
			parms->notifyee->PostMessage(UWM_WRITER_SHUTTING_DOWN, (WPARAM)::GetLastError());
			... handle failure notification here
				PostQuitMessage(0);  // this responds by shutting down the thread
									 // Your Mileage May Vary
			delete s;
			return;
		} /* serious error */

		  // If we get here, the reason is ERROR_IO_PENDING
		  // so wait for the I/O operation to complete

		  // By using WFMO and having the shutdown event be first, this allows us
		  // to break out of the wait to shut the thread down cleanly
		HANDLE waiters[2];
		waiters[0] = parms->shutdown;
		waiters[1] = WriteEvent;
		DWORD reason = ::WaitForMultipleObjects(2, waiters, FALSE, INFINITE);
		switch (reason)
		{ /* waitfor */
		case WAIT_OBJECT_0:  // it was the shutdown event
							 // shutting down
			::CancelIo(parms->hCom);
			parms->notifyee->PostMessage(UWM_WRITER_SHUTTING_DOWN, ERROR_SUCCESS);
			PostQuitMessage(0);
			delete s;
			return;
		case WAIT_OBJECT_0 + 1: // data complete
		{ /* write complete */
			BOOL ok = ::GetOverlappedResult(parms->hCom, &ovl, &bytesWritten, TRUE);
			if (!ok)
			{ /* failed */
				parms->notifyee->PostMessage(UWM_WRITER_SHUTTING_DOWN, (WPARAM)::GetLastError());
				PostQuitMessage(0);
				delete s;
				return;
			} /* failed */
			delete s;
		} /* write complete */
		break;
		default:
		{ /* trouble */
			DWORD err = ::GetLastError();
			ASSERT(FALSE);
			parms->notifyee->PostMessage(UWM_WRITER_SHUTTING_DOWN, (WPARAM)err);
			PostQuitMessage(0);
			delete s;
			return;
		} /* trouble */
		} /* waitfor */
	} /* failed */
	else
	{ /* successful write */
		delete s;
	} /* successful write */
	  // if we get here, either the WriteFile succeeded or we waited for it to complete
	parms->notifyee->PostMessage(UWM_DATA_WRITTEN);
	delete s;
}

BOOL SerialWriter::InitInstance()
{
	WriteEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (WriteEvent == NULL)
		return FALSE;
	... rest of initialization, if any
		return TRUE;
}

int SerialWriter::ExitInstance()
{
	::CloseHandle(WriteEvent);
	return CWinThread::ExitInstance();
}

/* static */ LRESULT CMyClass::ReaderThread(LPVOID p)
{
	SerialParameters * parms = (SerialParameters *)p;
	OVERLAPPED ovl = { 0 };
	ovl.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ovl.hEvent == NULL)
	{ /* failed */
		DWORD err = ::GetLastError();
		parms->notifyee->PostMessage(UWM_READER_SHUTTING_DOWN, (WPARAM)err);
		return 0;
	} /* failed */

	DWORD shutdown;  // reason for shutdown
	HANDLE waiters[2];
	waiters[0] = parms->shutdown;
	waiters[1] = ovl.hEvent;

#define MAX_BUFFER_SIZE 100
	BOOL running = TRUE;
	DWORD bytesRead;
	while (running)
	{ /* read loop */
		BYTE buffer[MAX_BUFFER_SIZE];
		BOOL ok = ::ReadFile(parms->hCom, buffer, MAX_BUFFER_SIZE - 1, &bytesRead, &ovl);
		if (!ok)
		{ /* error */
			DWORD err = ::GetLastError();
			if (err != ERROR_IO_PENDING)
			{ /* read error */
				shutdown = err;
				running = FALSE;
				continue;
			} /* read error */
			  // otherwise, it is ERROR_IO_PENDING
			DWORD result = ::WaitForMultipleObjects(2, waiters, FALSE, INFINITE);
			switch (result)
			{ /* wait */
			case WAIT_OBJECT_0:  // shutdown
				::CancelIo(parms->hCom);
				shutdown = ERROR_SUCCESS;  // clean shutdown
				running = FALSE;
				continue;
			case WAIT_OBJECT_0 + 1: // I/O complete
				ok = ::GetOverlappedResult(parms->hCom, &ovl, &bytesRead, TRUE);
				if (!ok)
				{ /* GetOverlappedResult failed */
					DWORD err = ::GetLastError();
					running = FALSE;
					continue;
				} /* GetOverlappedResult failed */
				break;
			default:
			{ /* trouble */
				shutdown = ::GetLastError();
				ASSERT(FALSE); // failure
				running = FALSE;
				continue;
			} /* trouble */
			} /* wait */
		} /* error */

		  // if we get here, either the ReadFile worked immediately, or we waited for completion
		if (bytesRead == 0)
			continue; // nothing was read
		buffer[bytesRead] = '\0'; // assumes 8-bit characters without embedded NUL
		CStringA * s = new CStringA((LPCSTR)buffer);
		parms->notifyee->PostMessage(UWM_DATA_READ, (WPARAM)s);
	} /* read loop */

	parms->notifyee->PostMessage(UWM_READER_SHUTTING_DOWN, (WPARAM)shutdown);
	::CloseHandle(ovl.hEvent);
	return 0;  // we're gone. You may choose to do something different
} // CMyClass::ReaderThread