#include "Client.h"
#include<regex>
#include <sys/stat.h>
#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"ibwsock32.a")
#pragma warning(disable:4996)
#include <sys\types.h> 
#include <sys\stat.h> 
#include <vector>
#include<string.h>
#include<fstream>
#include <algorithm>

__int64 getFileSize(const char* path)
{
	struct __stat64 fileStat;
	int err = _stat64(path, &fileStat);
	if (0 != err) return 0;
	return fileStat.st_size;
}


int main(int argc, char* argv[])
{
	string sDestiAdress = "127.0.0.1";
	string sPathStore = "./";
	int iBufferSize = 512;

	if (argc > 1)
	{
		string str = argv[1];
		str.erase(str.begin() + 0, str.begin() + 6);
		sPathStore = str;
	}
	Client client(sDestiAdress);
	client.isConnect();

	string sRecvType = (string)client.receiveData();
	//cout << recvType << endl;

	if (sRecvType.compare("SendText") == 0)
	{
		string sRecvText = (string)client.receiveData();
		cout << "Receive text success!" << endl;
		cout << "Message receive: " << sRecvText << endl;
	}

	if (sRecvType.compare("SendFile") == 0)
	{
		//recevie file name:
		string sFileName = (string) client.receiveData();
		//cout << fileName << endl;
	
		//recevie buffersize:
		iBufferSize = stoi(client.receiveData());

		//recevie file size:
		__int64 iFileSize = stoll(client.receiveData());
		__int64 iRemain = iFileSize;
		//cout << iRemain << endl;
		//cout << bufferSize << endl;
		client.setBufferSize(iBufferSize);
		ofstream MyRecvFile((sPathStore + sFileName).c_str(), std::ofstream::binary);
		string sBuffer;
		__int64 iCurrentSize;
		while (true)
		{
			streamsize size = (streamsize)min(iRemain, iBufferSize);

			//cout << size << endl;
			sBuffer = (string)client.receiveData();
			//cout << sBuffer << endl;
			MyRecvFile << sBuffer;
			/*char* cBuffer = new char[size];
			if (iRemain < iBufferSize)
			{
				client.setBufferSize(size);
			}
			strcpy(cBuffer, client.receiveData());
			cout << cBuffer << endl;
			if (!MyRecvFile.write(cBuffer, size))
			{
				cout << "error" << endl;
				break;
			}
			delete[] cBuffer;*/

			iRemain -= iBufferSize;
			iCurrentSize = getFileSize((sPathStore + sFileName).c_str());
			float per = (float)iCurrentSize / iFileSize * 100.0;
			cout << "\rReceive " << sFileName << ": " << per << "%";
			if (iRemain <= 0ll) break;
			
		}
		MyRecvFile.close();
		cout << endl;
		iCurrentSize = getFileSize((sPathStore + sFileName).c_str());
		float per = (float)iCurrentSize / iFileSize * 100.0;
		cout << "Receive " << sFileName << ": " << per << "%" << endl;
		cout << "Recevive " << sFileName << " success." << endl;
	}
	return 0;
}