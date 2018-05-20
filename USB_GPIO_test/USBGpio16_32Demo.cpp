/*
License
-------
This code is published and shared by Numato Systems Pvt Ltd under GNU LGPL
license with the hope that it may be useful. Read complete license at
http://www.gnu.org/licenses/lgpl.html or write to Free Software Foundation,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA

Simplicity and understandability is the primary philosophy followed while
writing this code. Sometimes at the expence of standard coding practices and
best practices. It is your responsibility to independantly assess and implement
coding practices that will satisfy safety and security necessary for your final
application.

This demo code demonstrates how to turn GPIOs to on (logic high) state and
demonstrates how to read an analog channel.

*/

#include "stdafx.h"
#include "windows.h"
#include "string.h"
#include <iostream>
#include <conio.h>

#define NO_PORTS 24
int _tmain(int argc, _TCHAR* argv[])
{

	HANDLE hComPort;
	char cmdBuffer[32];
	char responseBuffer[32];
	DWORD numBytesWritten;
	DWORD numBytesRead;
	int i = 0;
	int ports[NO_PORTS] = { 0 };

	/*
	Lookup the port name associated to your GPIO device and update the
	following line accordingly. The port name should be in the format
	"\\.\COM<port Number>". Notice the extra slashes to escape slashes
	themselves. Read http://en.wikipedia.org/wiki/Escape_sequences_in_C
	for more details.
	*/

	wchar_t PortName[] = L"\\\\.\\COM5";

	/*
	Open a handle to the COM port. We need the handle to send commands and
	receive results.
	*/

	hComPort = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	if (hComPort == INVALID_HANDLE_VALUE)
	{
		printf("Error: Unable to open the specified port\n");
		return 1;
	}
	DCB dcbSerialParams = { 0 }; // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	BOOL Status;
	Status = GetCommState(hComPort, &dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_256000;  // Setting BaudRate = 256000 

	SetCommState(hComPort, &dcbSerialParams);

	dcbSerialParams = { 0 }; // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(hComPort, &dcbSerialParams);

	strcpy(cmdBuffer, "gpio writeall FFFFFFFFFFFFFFFF");


	/* Append 0x0D to emulate ENTER key */
	cmdBuffer[30] = 0x0D;

	/* Write the command to the GPIO module. Total 11 bytes including 0x0D  */

	/*printf("Info: Writing command <adc read 01> to the GPIO module\n");*/

	if (!WriteFile(hComPort, cmdBuffer, 12, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		return 1;
	}

	
	/* Write a Carriage Return to make sure that any partial commands or junk
	data left in the command buffer is cleared. This step is optional.
	*/
	cmdBuffer[0] = 0x0D;

	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		return 1;
	}
	/* Flush the Serial port's RX buffer. This is a very important step*/
	Sleep(2);

	int pos;
	char read_com[12];
	char int_str[2];
	std::string port_number;

	while (i<100)
	{
		
		
		for (int j = 0; j < NO_PORTS; j++) {

			PurgeComm(hComPort, PURGE_RXCLEAR | PURGE_RXABORT);

			/* Copy the command to the command buffer */

			
			/*char read_com[12] = "adc read ";
			sprintf(int_str, "%02d", j);
			strcat(read_com, int_str);
			strcpy(cmdBuffer, read_com);*/
			strcpy(cmdBuffer, "adc read 01");

			/* Append 0x0D to emulate ENTER key */
			cmdBuffer[11] = 0x0D;

			/* Write the command to the GPIO module. Total 11 bytes including 0x0D  */

			/*printf("Info: Writing command <adc read 01> to the GPIO module\n");*/

			if (!WriteFile(hComPort, cmdBuffer, 12, &numBytesWritten, NULL))
			{
				CloseHandle(hComPort);
				printf("Error: Unable to write to the specified port\n");
				return 1;
			}

			/* printf("Info: <adc read 01> Command sent successfuly\n");*/
			Sleep(1);
			/*Read back the response*/
			if (!ReadFile(hComPort, responseBuffer, 17, &numBytesRead, NULL))
			{
				CloseHandle(hComPort);
				printf("Error: Unable to write to the specified port\n");
				return 1;
			}

			/* Add a null character at the end of the response so we can use the buffer
			with string manipulation functions.
			*/
			responseBuffer[numBytesRead] = '\0';
			
			/*char adc_response[4];
			pos = std::string(responseBuffer).find('\n', 13);
			if (pos != std::string::npos) {
				sprintf(adc_response, "%.*s", pos - 12, responseBuffer + 13);
			}
			else {
				sprintf(adc_response, "%.*s\n", 4, responseBuffer + 13);
			}
			ports[j] = std::atoi(adc_response);
			printf("%d ", ports[j]);*/
			
		}
		printf("%d\n",i);
		i++;
	}
	/* Close the comm port handle */
	CloseHandle(hComPort);
	return 0;
}