#include "server.h"
#include<regex>
#include <sys/stat.h>
#include <sstream>
#pragma comment(lib,"ws2_32.lib")
#include <vector>
#include<string.h>
//#pragma comment(lib,"ibwsock32.a")
#pragma warning(disable:4996)
#include <sys\types.h> 
#include <sys\stat.h> 
#include<fstream>
#include <algorithm>
using namespace std;

vector<string> SplitString(string cmd)
{
	vector <string> split{};
	stringstream ss(cmd); // Khởi tạo stringstream từ xâu S
	string token;
	while (ss >> token) { // Đọc lần lượt các phần của xâu. Các phần tách nhau bởi dấu cách hoặc xuống dòng.
		split.push_back(token);
	}
	return split;
}

string getFileName(string const& path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

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
	string sPath = "";
	string sBufferSize = "512";
	string sDataType = "";
	string sText = "";
	if (argc > 1)
	{
		sDestiAdress = (string) argv[1];
	}
	else
	{
		cout << " No destination adress";
		return 0;
	}
	cout << "\n waiting \n";
	Server server(sDestiAdress);
	server.isConnect();
	cout << "Step 2:" << endl;
	string sCmd;
	getline(cin, sCmd);
	
	vector <string> vCmdSplit{};
	vCmdSplit = SplitString(sCmd);
	//cout << cmd;

	//send loai du lieu:
	sDataType = vCmdSplit[0];
	char* cDataType = new char[(sDataType.length())];
	strcpy(cDataType, sDataType.c_str());
	server.sendData(cDataType);
	delete[] cDataType;
	if (sDataType.compare("SendText") == 0)
	{
		sCmd.erase(sCmd.begin() + 0, sCmd.begin() + 9);
		sText = sCmd;
		//cout << Text;
		
		int newBufferSize = sText.size();
		if (newBufferSize > 256)
		{
			cout << "Character limit exceeded." << endl;
			return 0;
		}

		char *cText = new char[sText.length()];
		strcpy(cText, sText.c_str());
		server.sendData(cText);
		cout << "Send text success!"<<endl;
		delete[] cText;
	}
	else if (sDataType.compare("SendFile") == 0)
	{
		sPath = vCmdSplit[1];
		sBufferSize = vCmdSplit[2];
		string sFileName = getFileName(sPath);
		//cout << bufferSize << endl;

		//send name:
		char *cFileName = new char [sFileName.length()];
		strcpy(cFileName, sFileName.c_str());
		server.sendData(cFileName);
		delete[] cFileName;

		//send buffersize:
		char *cBufferSize = new char[sBufferSize.length()];
		strcpy(cBufferSize, sBufferSize.c_str());
		server.sendData(cBufferSize);
		delete[] cBufferSize;

		//send size:
		__int64 iFileSize = getFileSize(sPath.c_str());
		string sFileSize = to_string(iFileSize);
		char *cFileSize = new char[sFileSize.length()];
		strcpy(cFileSize, sFileSize.c_str());
		server.sendData(cFileSize);
		//cout << cFileSize << endl;
		delete[] cFileSize;


		//read file:
		int iBufferSize = stoi(sBufferSize);
		server.setBufferSize(iBufferSize);
		__int64 iRemain = iFileSize;
		ifstream MyReadFile(sPath.c_str(), std::ifstream::binary);

		while (true)
		{
			streamsize size = (streamsize)min(iRemain, iBufferSize);
			//cout << size << endl;
			char* cBuffer = new char[size];
			MyReadFile.read(cBuffer, size);
			/*cout << size << endl;
			cout << cBuffer << endl;*/
			server.sendData(cBuffer);
			delete[] cBuffer;

			iRemain -= (iBufferSize);
			if (iRemain <= 0)
				break;
		}
		cout << "Send " << sFileName << " success." << endl;
		MyReadFile.close();
	}
	return 0;
}