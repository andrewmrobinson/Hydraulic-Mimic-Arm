
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
HANDLE hPort;
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
Vector prox, mid;
volatile float prox_mid_angle = 0;

void SampleListener::onInit(const Controller& controller) {
	std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
}

void SampleListener::onDisconnect(const Controller& controller) {
	// Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
	std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
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
		/*
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
		<< ", palm position: " << hand.palmPosition() << std::endl;
		// Get the hand's normal vector and direction
		const Vector normal = hand.palmNormal();
		const Vector direction = hand.direction();
		*/
		/*
		// Calculate the hand's pitch, roll, and yaw angles
		std::cout << std::string(2, ' ') << "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
		<< "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
		<< "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
		*/
		/*
		// Get the Arm bone
		Arm arm = hand.arm();
		std::cout << std::string(2, ' ') << "Arm direction: " << arm.direction()
		<< " wrist position: " << arm.wristPosition()
		<< " elbow position: " << arm.elbowPosition() << std::endl;
		*/
		// Get fingers
		const FingerList fingers = hand.fingers();
		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Finger finger = *fl;
			if (finger.type() == 1) {
				prox = finger.bone(static_cast<Bone::Type>(1)).direction();
				mid = finger.bone(static_cast<Bone::Type>(2)).direction();
				prox_mid_angle = mid.angleTo(prox);
				//std::cout << "Angle in Radians: " << prox_mid_angle*180/3.14;

				/*std::cout << std::string(4, ' ') << fingerNames[finger.type()]
				<< " finger, id: " << finger.id()
				<< ", length: " << finger.length()
				<< "mm, width: " << finger.width() << std::endl;

				// Get finger bones
				for (int b = 1; b < 3; ++b) {
				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);
				std::cout << std::string(6, ' ') << boneNames[boneType]
				<< " bone, start: " << bone.prevJoint()
				<< ", end: " << bone.nextJoint()
				<< ", direction: " << bone.direction() << std::endl;
				}
				*/
			}
		}
	}

	if (!frame.hands().isEmpty()) {
		//std::cout << std::endl;
	}

}

void SampleListener::onFocusGained(const Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
	std::cout << "Device Changed" << std::endl;
	const DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
		std::cout << "  isSmudged:" << (devices[i].isSmudged() ? "true" : "false") << std::endl;
		std::cout << "  isLightingBad:" << (devices[i].isLightingBad() ? "true" : "false") << std::endl;
	}
}

void SampleListener::onServiceConnect(const Controller& controller) {
	std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
	std::cout << "Service Disconnected" << std::endl;
}

void SampleListener::onServiceChange(const Controller& controller) {
	std::cout << "Service Changed" << std::endl;
}

void SampleListener::onDeviceFailure(const Controller& controller) {
	std::cout << "Device Error" << std::endl;
	const Leap::FailedDeviceList devices = controller.failedDevices();

	for (FailedDeviceList::const_iterator dl = devices.begin(); dl != devices.end(); ++dl) {
		const FailedDevice device = *dl;
		std::cout << "  PNP ID:" << device.pnpId();
		std::cout << "    Failure type:" << device.failure();
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

bool WriteComPort(CString PortSpecifier, CString data)
{
	DCB dcb;
	DWORD byteswritten;
	HANDLE hPort = CreateFile(

		PortSpecifier,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (!GetCommState(hPort, &dcb))
		return false;
	dcb.BaudRate = CBR_115200; //9600 Baud
	dcb.ByteSize = 8; //8 data bits
	dcb.Parity = NOPARITY; //no parity
	dcb.StopBits = ONESTOPBIT; //1 stop
	if (!SetCommState(hPort, &dcb))
		return false;
	bool retVal = WriteFile(hPort, data, 1, &byteswritten, NULL);
	CloseHandle(hPort); //close the handle
	return retVal;
}

char TempChar[26]; //Temporary character used for reading
char SerialBuffer[256];//Buffer for storing Rxed Data
DWORD NoBytesRead;
int k;

int main(int argc, char** argv) {
	
	CString PortSpecifier = "COM5";
	DCB dcb;
	COMMTIMEOUTS com_timeouts;
	
	DWORD byteswritten;
	hPort = CreateFile(PortSpecifier,
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
	/*
	// Create a sample listener and controller
	SampleListener listener;
	Controller controller;

	// Have the sample listener receive events from the controller
	controller.addListener(listener);

	if (argc > 1 && strcmp(argv[1], "--bg") == 0)
		controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

	controller.setPolicy(Leap::Controller::POLICY_ALLOW_PAUSE_RESUME);

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit, or enter 'p' to pause or unpause the service..." << std::endl;
	*/
	bool paused = false;
	int psoc_status;
	int ideal_pos = 1500;
	int switchdirection = 0;
	//OVERLAPPED osWriter = { 0 };
	char data[20];
	int data_size = 20;
	int char_no;
	double pid[3] = { 0.03,0.000,0.00 };
	//double pid[3] = { -10.996,-0.13052,-15.89892 };
	char temp;
	bool updatePID = true;
	char sequence_char = 'r';
	FILE * pFile;
	char name[100];
	int alternative = 0;
	int numclosed = _fcloseall();
	printf("Number of files closed by _fcloseall: %u\n", numclosed);

	FILE *stream;

	errno_t err;
	std::string filename = time_stamp("%F %H%M%S");
	filename = "responseData/" + filename + ".txt";
	// Open for write
	err = fopen_s(&stream, filename.c_str(), "w+");
	if (err == 0)
	{
		printf("The file 'data.txt' was opened\n");
	}
	else
	{
		printf("The file 'data.txt' was not opened\n");
	}

	while (true) {
		if (GetAsyncKeyState(0x50)) {
			std::cin.ignore(INT_MAX, 'p');
			std::cout << "Please enter a P value: ";
			std::cin >> pid[0];
			std::cout << "New PID values are P: " << pid[0] << " I: " << pid[1] << " D: " << pid[2] << "\n";
			updatePID = true;
			system("PAUSE");
		} //P
		if (GetAsyncKeyState(0x49)) {
			std::cin.ignore(INT_MAX, 'i');
			std::cout << "Please enter a I value: ";
			std::cin >> pid[1];
			std::cout << "New PID values are P: " << pid[0] << " I: " << pid[1] << " D: " << pid[2] << "\n";
			updatePID = true;
			system("PAUSE");
		} //I
		if (GetAsyncKeyState(0x44)) {
			std::cin.ignore(INT_MAX, 'd');
			std::cout << "Please enter a D value: ";
			std::cin >> pid[2];
			std::cout << "New PID values are P: " << pid[0] << " I: " << pid[1] << " D: " << pid[2] << "\n";
			updatePID = true;
			system("PAUSE");
		} //D
		if (GetAsyncKeyState(0x58)) {
			std::cin.ignore(INT_MAX, 'x');
			std::cout << "Please enter a position value (0-4095): ";
			std::cin >> ideal_pos;
			std::cout << "New position value is X: " << ideal_pos << "\n";
			system("PAUSE");
		} //X
		if (GetAsyncKeyState(0x53)) {
			std::cin.ignore(INT_MAX, 's');
			std::cout << "Sequence on? (y/n): ";
			std::cin >> sequence_char;
			std::cout << "Sequence is: " << sequence_char << "\n";
			system("PAUSE");
		} //S
		if (GetAsyncKeyState(0x43)) {
			std::cin.ignore(INT_MAX, 'c');
			system("PAUSE");
			system("CLS");
		} //S

		if (updatePID) {
			char_no = snprintf(data, data_size, "{p%.4f}", pid[0]);
			for (int i = 0; i < char_no; i++)
				bool retVal = WriteFile(hPort, (CString)data[i], 1, &byteswritten, NULL);
			char_no = snprintf(data, data_size, "{i%.4f}", pid[1]);
			for (int i = 0; i < char_no; i++)
				bool retVal = WriteFile(hPort, (CString)data[i], 1, &byteswritten, NULL);
			char_no = snprintf(data, data_size, "{d%.4f}", pid[2]);
			for (int i = 0; i < char_no; i++)
				bool retVal = WriteFile(hPort, (CString)data[i], 1, &byteswritten, NULL);
			updatePID = false;
		}

		//std::cout << "Angle in Radians: " << prox_mid_angle * 180 / 3.14;
		//std::cout << std::endl;
		//sprintf_s(data, "{%0.0f}", (prox_mid_angle * 180 / 3.14) - 45);
		sprintf_s(data, "{x%4d}", ideal_pos);
		for (int i = 0; i < 10; i++)
			bool retVal = WriteFile(hPort, (CString)data[i], 1, &byteswritten, NULL);

		ReadFile(hPort,           //Handle of the Serial port
			&TempChar,       //Temporary character
			sizeof(TempChar),//Size of TempChar
			&NoBytesRead,    //Number of bytes read
			NULL);

		for (k = 0; k < sizeof(TempChar); k++) {		// j < i-1 to remove the dupliated last character
			printf("%c", TempChar[k]);
			fprintf(stream, "%c", TempChar[k]);
		}

		printf(" | %d\n", ideal_pos);
		fprintf(stream, " | %d\n", ideal_pos);
		

		

		if (sequence_char == 't') {
			switchdirection++;
			if (switchdirection > 500)
			{
				if (ideal_pos == 1500)
				{
					ideal_pos = 1510;
				}
				else
				{
					ideal_pos = 1500;
				}

				switchdirection = 0;
			}
			
			
		}
		else if(sequence_char == 'r') {
			
			if (alternative == 0) {
				ideal_pos++;
				ideal_pos++;
				if (ideal_pos > 2300) { alternative = 1; }
			}
			if (alternative == 1) {
				ideal_pos--;
				ideal_pos--;
				if (ideal_pos < 1000) { alternative = 0; }
			}
		}
	}
		Sleep(10);
	

	// Remove the sample listener when done
	//controller.removeListener(listener);

	CloseHandle(hPort); //close the handle
						// Close stream if it is not NULL
	if (stream)
	{
		err = fclose(stream);
		if (err == 0)
		{
			printf("The file 'crt_fopen_s.c' was closed\n");
		}
		else
		{
			printf("The file 'crt_fopen_s.c' was not closed\n");
		}
	}

	return 0;
}
