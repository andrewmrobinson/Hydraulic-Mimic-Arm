
#include "stdafx.h"
#include <iostream>
#include <cstring>
#include "Leap.h"
#include "windows.h"
#include <conio.h>
#include <string.h>
#include <atlstr.h>
#include <process.h>
#include <stdio.h>
#include <ctime>
#include <thread>

#define MAX_SEM_COUNT 1
HANDLE strSemaphore;
HANDLE vcomSemaphore;
HANDLE getAnglesSemaphore;


inline std::tm localtime_xp(std::time_t timer) {
	std::tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *std::localtime(&timer);
#endif
	return bt;
}

// default = "YYYY-MM-DD HH:MM:SS"
inline std::string time_stamp(const std::string& fmt = "%F %T")
{
	auto bt = localtime_xp(std::time(0));
	char buf[64];
	return { buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &bt) };
}


using namespace Leap;
volatile int recievedData;
volatile int new_distance = 0;

class SampleListener : public Listener {
public:
	virtual void onInit(const Controller&);
	virtual void onConnect(const Controller&);
	virtual void onDisconnect(const Controller&);
	virtual void onExit(const Controller&);
	virtual void onFrame(const Controller&);
	virtual void onFocusGained(const Controller&);
	virtual void onFocusLost(const Controller&);
	virtual void onDeviceChange(const Controller&);
	virtual void onServiceConnect(const Controller&);
	virtual void onServiceDisconnect(const Controller&);
	virtual void onServiceChange(const Controller&);
	virtual void onDeviceFailure(const Controller&);
	virtual void onLogMessage(const Controller&, MessageSeverity severity, int64_t timestamp, const char* msg);
};

const std::string fingerNames[] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const std::string boneNames[] = { "Metacarpal", "Proximal", "Middle", "Distal" };
Vector prox, mid, met;

struct fingerAngles {
	// first value is palm to proximal bone angle, second value is prox to mid angle
	float index[2], middle[2],ring[2], pinky[2], thumb[2];
};
fingerAngles anglesGlobal;

void SampleListener::onInit(const Controller& controller) {
	//std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
	//std::cout << "Connected" << std::endl;
}

void SampleListener::onDisconnect(const Controller& controller) {
	// Note: not dispatched when running in a debugger.
	//std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
	//std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
	DWORD dwWaitResultAngles;
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
	/*
	std::cout << "Frame id: " << frame.id()
	<< ", timestamp: " << frame.timestamp()
	<< ", hands: " << frame.hands().count()
	<< ", extended fingers: " << frame.fingers().extended().count() << std::endl;
	*/
	HandList hands = frame.hands();
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Hand hand = *hl;
		const FingerList fingers = hand.fingers();
		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Finger finger = *fl;

			dwWaitResultAngles = WaitForSingleObject(getAnglesSemaphore, INFINITE);
			switch (finger.type()) {
				case 0:
					anglesGlobal.thumb[0] = finger.bone(static_cast<Bone::Type>(1)).direction().angleTo(finger.bone(static_cast<Bone::Type>(0)).direction());
					anglesGlobal.thumb[1] = finger.bone(static_cast<Bone::Type>(2)).direction().angleTo(finger.bone(static_cast<Bone::Type>(1)).direction());
					break;
				case 1:
					anglesGlobal.index[0] = finger.bone(static_cast<Bone::Type>(1)).direction().angleTo(finger.bone(static_cast<Bone::Type>(0)).direction());
					anglesGlobal.index[1] = finger.bone(static_cast<Bone::Type>(2)).direction().angleTo(finger.bone(static_cast<Bone::Type>(1)).direction());
					break;
				case 2:
					anglesGlobal.middle[0] = finger.bone(static_cast<Bone::Type>(1)).direction().angleTo(finger.bone(static_cast<Bone::Type>(0)).direction());
					anglesGlobal.middle[1] = finger.bone(static_cast<Bone::Type>(2)).direction().angleTo(finger.bone(static_cast<Bone::Type>(1)).direction());
					break;
				case 3:
					anglesGlobal.ring[0] = finger.bone(static_cast<Bone::Type>(1)).direction().angleTo(finger.bone(static_cast<Bone::Type>(0)).direction());
					anglesGlobal.ring[1] = finger.bone(static_cast<Bone::Type>(2)).direction().angleTo(finger.bone(static_cast<Bone::Type>(1)).direction());
					break;
				case 4:
					anglesGlobal.pinky[0] = finger.bone(static_cast<Bone::Type>(1)).direction().angleTo(finger.bone(static_cast<Bone::Type>(0)).direction());
					anglesGlobal.pinky[1] = finger.bone(static_cast<Bone::Type>(2)).direction().angleTo(finger.bone(static_cast<Bone::Type>(1)).direction());
					break;
			}
			if (!ReleaseSemaphore(getAnglesSemaphore, 1, NULL)) { printf("Release Semaphore error: %d\n", GetLastError()); }
		}
	}

	if (!frame.hands().isEmpty()) {
		//std::cout << std::endl;
	}

}

void SampleListener::onFocusGained(const Controller& controller) {
	//std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
	//std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
	//std::cout << "Device Changed" << std::endl;
	const DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		/*std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
		std::cout << "  isSmudged:" << (devices[i].isSmudged() ? "true" : "false") << std::endl;
		std::cout << "  isLightingBad:" << (devices[i].isLightingBad() ? "true" : "false") << std::endl;*/
	}
}

void SampleListener::onServiceConnect(const Controller& controller) {
	//std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
	//std::cout << "Service Disconnected" << std::endl;
}

void SampleListener::onServiceChange(const Controller& controller) {
	//std::cout << "Service Changed" << std::endl;
}

void SampleListener::onDeviceFailure(const Controller& controller) {
	//std::cout << "Device Error" << std::endl;
	const Leap::FailedDeviceList devices = controller.failedDevices();

	for (FailedDeviceList::const_iterator dl = devices.begin(); dl != devices.end(); ++dl) {
		const FailedDevice device = *dl;
		//std::cout << "  PNP ID:" << device.pnpId();
		//std::cout << "    Failure type:" << device.failure();
	}
}

void SampleListener::onLogMessage(const Controller&, MessageSeverity s, int64_t t, const char* msg) {
	switch (s) {
	case Leap::MESSAGE_CRITICAL:
		std::cout << "[Critical]";
		break;
	case Leap::MESSAGE_WARNING:
		std::cout << "[Warning]";
		break;
	case Leap::MESSAGE_INFORMATION:
		std::cout << "[Info]";
		break;
	case Leap::MESSAGE_UNKNOWN:
		std::cout << "[Unknown]";
	}
	std::cout << "[" << t << "] ";
	std::cout << msg << std::endl;
}


HANDLE openComPort(CString PortSpecifier){
	DCB dcb;
	COMMTIMEOUTS com_timeouts;
	HANDLE hPort = CreateFile(PortSpecifier,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	GetCommState(hPort, &dcb);
	dcb.BaudRate = CBR_115200; //9600 Baud
	dcb.ByteSize = 8; //8 data bits
	dcb.Parity = NOPARITY; //no parity
	dcb.StopBits = ONESTOPBIT; //1 stop
	SetCommState(hPort, &dcb);

	GetCommTimeouts(hPort, &com_timeouts);
	com_timeouts.ReadIntervalTimeout = 1;
	com_timeouts.ReadTotalTimeoutMultiplier = 1;
	com_timeouts.ReadTotalTimeoutConstant = 1000;
	com_timeouts.WriteTotalTimeoutConstant = 0;
	com_timeouts.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hPort, &com_timeouts);

	return hPort;
}

double pid[3] = { 19,0.001,0.04 };
bool updatePID[3] = {false, false, false};

DWORD NoBytesRead2;
char TempChar2[26]; //Temporary character used for reading
int positions_0[4];
int positions_1[4];

/* DECLARING COM PORTS */
CString psoc16COM = "COM5";
CString psoc17COM = "COM3";
CString vComPort = "COM9";
/* END DECLARING COM PORTS */

int psoc_thread(CString ComPort, int Pos[],int no_Pos,double PID[3], bool *updatePID_thread, bool forwardvcom) {
	/* COMMS START */
	char data[20];
	int data_size = 20;
	int char_no;
	int k;
	bool retVal;

	HANDLE psoc = openComPort(ComPort);
	DWORD noBytesRead_thread;
	DWORD bytesWritten_thread;
	HANDLE vcom;
	DWORD noBytesRead_vcom;
	DWORD bytesWritten_vcom;
	if (forwardvcom) {
		vcom = openComPort(vComPort);
	}
	/* COMMS END */

	/* KEVO'S SEMAPHORE MAGIC */
	DWORD dwWaitResult;
	DWORD dwWaitResultvcom;
	
	/* KEVO'S SEMAPHORE MAGIC */
	char tempChar_thread[48];

	int switchdirection = 0;

	//bool updatePID_thread = true;
	char sequence_char = 't';
	int alternate = 0;

	FILE *stream;

	errno_t err;
	std::string comport_string(CW2A(ComPort.GetString()));
	std::string filename = time_stamp("%F %H%M%S");
	filename = "responseData/" + comport_string + "_" + filename + ".txt";
	// Open for write
	err = fopen_s(&stream, filename.c_str(), "w+");

	while (true) {
		/* KEVO'S SEMAPHORE MAGIC */
		dwWaitResult = WaitForSingleObject(
			strSemaphore,	//handle to semaphore
			0L);			//zero-second time-out interval

		if (*updatePID_thread) {
			char_no = snprintf(data, data_size, "{p%.4f}", PID[0]);
			for (int i = 0; i < char_no; i++) {
				retVal = WriteFile(psoc, (CString)data[i], 1, &bytesWritten_thread, NULL);
			}
			char_no = snprintf(data, data_size, "{i%.4f}", PID[1]);
			for (int i = 0; i < char_no; i++) {
				retVal = WriteFile(psoc, (CString)data[i], 1, &bytesWritten_thread, NULL);
			}
			char_no = snprintf(data, data_size, "{d%.4f}", PID[2]);
			for (int i = 0; i < char_no; i++) {
				retVal = WriteFile(psoc, (CString)data[i], 1, &bytesWritten_thread, NULL);
			}
			*updatePID_thread = false;
		}
		for (int tt = 0; tt < no_Pos; tt++) {
			char_no = snprintf(data, data_size, "{c%d}{x%4d}", tt,Pos[tt]);
			for (int i = 0; i < char_no; i++) {
				retVal = WriteFile(psoc, (CString)data[i], 1, &bytesWritten_thread, NULL);
			}
			ReadFile(psoc, &tempChar_thread, sizeof(tempChar_thread), &noBytesRead_thread, NULL);
		}
		//ReadFile(psoc, &tempChar_thread, sizeof(tempChar_thread), &noBytesRead_thread, NULL);
		
		/* KEVO'S SEMAPHORE MAGIC */
		switch (dwWaitResult)
		{
			case WAIT_OBJECT_0:
				for (k = 0; k < sizeof(tempChar_thread); k++) {		// j < i-1 to remove the dupliated last character
					fprintf(stream, "%c", tempChar_thread[k]);
					printf("%c", tempChar_thread[k]);
				}
				if (forwardvcom) {
					retVal = PurgeComm(vcom,  PURGE_TXABORT || PURGE_TXCLEAR);

					retVal = WriteFile(vcom, tempChar_thread, sizeof(tempChar_thread), &bytesWritten_thread, NULL);
					retVal = WriteFile(vcom, "\n", 1, &bytesWritten_thread, NULL);
				}
				fprintf(stream, "\tP:%f I:%f D:%f", PID[0], PID[1], PID[2]);
				for (int tt = 0; tt < no_Pos; tt++) {
					fprintf(stream, "\t %d", Pos[tt]);
				}

				fprintf(stream, "\n");
				printf("\n");

				if (!ReleaseSemaphore(
					strSemaphore,
					1,
					NULL))
				{
					printf("Release Semaphore error: %d\n", GetLastError());
				}
				break;
			case WAIT_TIMEOUT:
				//printf("Thread %d: wait timed out\n", 1);
				break;
		}
	}

	// Close stream if it is not NULL
	if (stream)
	{
		err = fclose(stream);
		if (err == 0)
		{
			//printf("The file 'crt_fopen_s.c' was closed\n");
		}
		else
		{
			//printf("The file 'crt_fopen_s.c' was not closed\n");
		}
	}
	
	CloseHandle(psoc); //close the COM port handle 
	CloseHandle(vcom);
	return 0;
}

int main(int argc, char** argv) {

	/* KEVO'S SEMAPHORE MAGIC */
	strSemaphore = CreateSemaphore(
		NULL,			//default security attributes
		MAX_SEM_COUNT,	//initial count
		MAX_SEM_COUNT,	//maximum count
		NULL);			//unnamed semaphore

	if (strSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		return 1;
	}

	DWORD dwWaitResult;

	vcomSemaphore = CreateSemaphore(
		NULL,
		MAX_SEM_COUNT,
		MAX_SEM_COUNT,
		NULL);

	if (vcomSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d \n", GetLastError());
		return 1;
	}

	DWORD dwWaitResultvcom;

	getAnglesSemaphore = CreateSemaphore(
		NULL,			//default security attributes
		MAX_SEM_COUNT,	//initial count
		MAX_SEM_COUNT,	//maximum count
		NULL);			//unnamed semaphore

	if (getAnglesSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		return 1;
	}
	DWORD dwWaitResultAngles;

	/* END KEVO'S SEMAPHORE MAGIC */

	positions_0[0] = 2200;
	positions_0[1] = 2200;
	positions_0[2] = 2200;
	positions_0[3] = 2200;

	pid[0] = 0.1;		//P
	pid[1] = 0.01;	//I
	pid[2] = 0.0;	//D
	updatePID[0] = true;


	bool psoc16Status = true; //true if uart connection possible
	bool psoc17Status = false; //true if uart connection possible
	/* TESTS BOTH COMPORTS AND REQUESTS ID FROM THE PSOCS TO DETERMINE WHICH IS THE FORWARD OR BACKWARD CONTROLLING PSOC */
	/* PSOC16 should be controlling the forward cylinders and PSOC17 backward */
	/*
	HANDLE psocTest;
	DWORD noBytesReadTest;
	DWORD bytesWrittenTest;
	char tempChar[2]; //Temporary character used for reading
	bool retValTest;
	
	CString temp;
	bool invertComs = false;

	psocTest = openComPort(psoc16COM);
	if (psocTest != INVALID_HANDLE_VALUE) {
		retValTest = PurgeComm(psocTest, PURGE_RXABORT || PURGE_RXCLEAR|| PURGE_TXABORT || PURGE_TXCLEAR);
		retValTest = WriteFile(psocTest, "{w}", 4, &bytesWrittenTest, NULL);
		Sleep(100);
		retValTest = PurgeComm(psocTest, PURGE_RXABORT || PURGE_RXCLEAR || PURGE_TXABORT || PURGE_TXCLEAR);
		ReadFile(psocTest, &tempChar, sizeof(tempChar), &noBytesReadTest, NULL);
		if (tempChar[0] == '1' && tempChar[1] == '6') {
			psoc16Status = true;
		}
		else if (tempChar[0] == '1' && tempChar[1] == '7') {
			psoc17Status = true;
			invertComs = true;
		}
		CloseHandle(psocTest);
	}
	psocTest = openComPort(psoc17COM);
	if (psocTest != INVALID_HANDLE_VALUE) {
		retValTest = PurgeComm(psocTest, PURGE_RXABORT || PURGE_RXCLEAR || PURGE_TXABORT || PURGE_TXCLEAR);
		Sleep(5);
		retValTest = WriteFile(psocTest, "{w}", 4, &bytesWrittenTest, NULL);
		ReadFile(psocTest, &tempChar, sizeof(tempChar), &noBytesReadTest, NULL);
		if (tempChar[0] == '1' && tempChar[1] == '7') {
			psoc17Status = true;
		}
		else if (tempChar[0] == '1' && tempChar[1] == '6') {
			psoc16Status = true;
			invertComs = true;
		}
	}
	CloseHandle(psocTest);

	if (invertComs) {
		temp = psoc16COM;
		psoc16COM = psoc17COM;
		psoc17COM = temp;
	}
	*/
	
	if (psoc16Status) {
		std::thread t1(psoc_thread, psoc16COM, positions_0, 4, pid, &updatePID[0], 1);
	}
	if (psoc17Status) {
		std::thread t1(psoc_thread, psoc17COM, positions_1, 4, pid, &updatePID[1], 0);
	}


	/*
	// Create a sample listener and controller
	SampleListener listener;
	Controller controller;

	// Have the sample listener receive events from the controller
	controller.addListener(listener);

	if (argc > 1 && strcmp(argv[1], "--bg") == 0)
		controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

	controller.setPolicy(Leap::Controller::POLICY_ALLOW_PAUSE_RESUME);
	*/
	// Keep this process running until Enter is pressed
	
	int switchdirection = 0;
	int alternate = 0;
	char sequence_char = 'n';
	int cyl_no = 0;
	int psoc_no = 0;
	fingerAngles angles;
	char followHand = 0;

	while (true) {
		if (GetAsyncKeyState(0x50)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 'p');
			std::cout << "Please enter a P value: ";
			std::cin >> pid[0];
			std::cout << "New PID values are P: " << pid[0] << " I: " << pid[1] << " D: " << pid[2] << "\n";
			updatePID[psoc_no] = true;
			system("PAUSE");

			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //P
		if (GetAsyncKeyState(0x49)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 'i');
			std::cout << "Please enter a I value: ";
			std::cin >> pid[1];
			std::cout << "New PID values are P: " << pid[0] << " I: " << pid[1] << " D: " << pid[2] << "\n";
			updatePID[psoc_no] = true;
			system("PAUSE");
			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //I
		if (GetAsyncKeyState(0x44)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 'd');
			std::cout << "Please enter a D value: ";
			std::cin >> pid[2];
			std::cout << "New PID values are P: " << pid[0] << " I: " << pid[1] << " D: " << pid[2] << "\n";
			updatePID[psoc_no] = true;
			system("PAUSE");
			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //D
		if (GetAsyncKeyState(0x58)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 'x');
			std::cout << "Please enter a position value (0-4095): ";
			if (psoc_no == 0) { 
				std::cin >> positions_0[cyl_no];
				std::cout << "New position value of cylinder " << cyl_no << "is X: " << positions_0[cyl_no] << "\n";
			}
			else if (psoc_no == 1){ 
				std::cin >> positions_1[cyl_no];
				std::cout << "New position value of cylinder " << cyl_no << "is X: " << positions_1[cyl_no] << "\n";
			}
			system("PAUSE");
			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //X
		if (GetAsyncKeyState(0x53)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 's');
			std::cout << "Sequence on? (r - ramp /t - step): ";
			std::cin >> sequence_char;
			std::cout << "Sequence is: " << sequence_char << "\n";
			system("PAUSE");
			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //S
		if (GetAsyncKeyState(0x43)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 'c');
			std::cout << "Please select a cylinder number (0-3): ";
			std::cin >> cyl_no;
			std::cout << "Selected cylinder is : " << cyl_no << "\n";
			system("PAUSE");
			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //C
		if (GetAsyncKeyState(0x51)) {
			/* KEVO'S SEMAPHORE MAGIC */
			dwWaitResult = WaitForSingleObject(
				strSemaphore,	//handle to semaphore
				INFINITE);		//infinite time-out interval
			std::cin.ignore(INT_MAX, 'q');
			std::cout << "Please select a thread number (1-3): ";
			std::cin >> psoc_no;
			std::cout << "Selected psoc is : " << psoc_no << "\n";
			system("PAUSE");
			/* KEVO'S SEMAPHORE MAGIC */
			if (!ReleaseSemaphore(
				strSemaphore,
				1,
				NULL))
			{
				printf("Release Semaphore error: %d\n", GetLastError());
			}
		} //Q
		if (GetAsyncKeyState(0x46)) {
			dwWaitResult = WaitForSingleObject(strSemaphore,INFINITE);		
			std::cin.ignore(INT_MAX, 'f');
			std::cout << "Follow hand (y/n): ";
			std::cin >> followHand;
			std::cout << "Follow hand is set to: " << followHand << "\n";
			system("PAUSE");
			if (!ReleaseSemaphore(strSemaphore,1,NULL)){printf("Release Semaphore error: %d\n", GetLastError());}
		} //F

		if (followHand == 'y') {
			dwWaitResultAngles = WaitForSingleObject(getAnglesSemaphore, INFINITE);
			angles = anglesGlobal;
			if (!ReleaseSemaphore(getAnglesSemaphore, 1, NULL)) { printf("Release Semaphore error: %d\n", GetLastError()); }

			//Forward cylinders - PSOC_ID = 16 - controls met-prox angles
			//positions_0[0] = angles.index[0];//index
			//positions_0[1] = angles.middle[0]; //middle
			//positions_0[2] = angles.ring[0]; //ring
			positions_0[3] = -843.35 * angles.pinky[0] + 2220.7;	//pinky

			//Back cylinders - PSOC_ID = 17 - controls prox-mid angles
			//positions_1[0] = angles.index[1]; //index
			//positions_1[1] = angles.middle[1];//middle
			//positions_1[2] = angles.ring[1];//ring
			//positions_1[3] = angles.pinky[1];//pinky

			//std::cout << "Met to Prox :" << index[0] * 180 / PI << " degrees\t" << "Prox to Mid :" << index[1] * 180 / PI << " degrees\n";

		}
		
		if (sequence_char == 't') {
			switchdirection++;
			if (switchdirection > 1000)
			{
				if (positions_0[1] == 1500)
				{
					positions_0[1] = 1550;
				}
				else
				{
					positions_0[1] = 1500;
				}

				switchdirection = 0;
			}
		}
		else if(sequence_char == 'r') {
			
			if (alternate == 0) {
				positions_0[0] += 2;
				positions_0[1] += 2 ;
				positions_0[2] += 2;
				positions_0[3] += 2;
				if (positions_0[1] >= 2000) { alternate = 1; }
			}
			if (alternate == 1) {
				positions_0[0] -= 2;
				positions_0[1] -= 2;
				positions_0[2] -= 2;
				positions_0[3] -= 2;
				if (positions_0[1] <= 1150) { alternate = 0; }
			}
		}
		Sleep(10);
	}
		
	

	// Remove the sample listener when done
//controller.removeListener(listener);

	

	return 0;
}
