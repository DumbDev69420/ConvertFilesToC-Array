#include <iostream>
#include <vector>
#include <Windows.h>
#include <thread>
#include <filesystem>
#include <fstream>

namespace Coolshit {
	void MoveCursorConsole(int x, int y) {

		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(hStdout, &consoleInfo);


		COORD newPosition;

		newPosition.X = consoleInfo.dwCursorPosition.X + x;


		newPosition.Y = consoleInfo.dwCursorPosition.Y + y;

		SetConsoleCursorPosition(hStdout, newPosition);

	}

	void outputFancy(std::string input, int Delay = 60) {
		auto mainThreadID = std::this_thread::get_id();
		bool mainThread = false;

		int Numb = 0;

		for (size_t i = 0; i <= input.size(); Numb++)
		{
			auto isNumb = !std::isalpha(input[i]);

			if (Numb > i)
			{
				MoveCursorConsole(-1, 0);

				std::cout << input[i];

				i++;
				Numb--;
			}
			else
			{
				auto isUpper = std::isupper(input[i]);
				if (isNumb) {
					if (input[i] >= 0 && input[i] <= 10)
					{
						std::cout << input[i];

						i++;
						Numb--;
					}
					else
						std::cout << "-";
				}
				else
				{
					std::cout << (char)(isUpper ? std::tolower(input[i]) : std::toupper(input[i]));
				}
			}

			Sleep(Delay);
		}
	}
	
	std::wstring TolowerW(std::wstring input) {
		std::wstring Copy = L"";

		for (size_t i = 0; i < input.size(); i++)
		{
			Copy += std::tolower(input[i]);
		}

		return Copy;
	}

	std::string Tolower(std::string input) {
		std::string Copy = "";

		for (size_t i = 0; i < input.size(); i++)
		{
			Copy += std::tolower(input[i]);
		}

		return Copy;
	}
}

class FileConverter
{
private:
	std::vector<std::filesystem::directory_entry> Entries;
	std::vector<std::filesystem::directory_entry> CleanedEntries;

public:
	FileConverter(std::wstring Path) {

		for (const auto& entry : std::filesystem::directory_iterator(Path)) {
			Entries.push_back(entry);
		}
	}

	int GetEntryCount() {
		return Entries.size();
	}

	int GetCleanedEntryCount() {
		return CleanedEntries.size();
	}

	void GetTypes(std::wstring Type) {
		using namespace Coolshit;


		if (Type == L"") {
			CleanedEntries = Entries;
		}
		else
		{
			if (Type.find(L".") == std::wstring::npos)
				Type = L"." + Type;

			auto LowerType = TolowerW(Type);

			for (size_t i = 0; i < Entries.size(); i++)
			{
				auto lowerstring = TolowerW(Entries[i].path().wstring());

				if (lowerstring.find(LowerType) != std::wstring::npos) {
					CleanedEntries.push_back(Entries[i]);
				}
			}
		}
		
	}

	void CreateData(std::wstring FolderName) {
		if (CreateDirectoryW(FolderName.c_str(), NULL)) {
			std::vector<std::wstring> Paths;
			std::vector<int> Numbs;

			for (size_t i = 0; i < CleanedEntries.size(); i++) {
				auto pathwstring = CleanedEntries[i].path().wstring();
				std::ifstream streamFile(pathwstring, std::ios::binary);

				if (streamFile.is_open()) {
					// Get the size of the file
					streamFile.seekg(0, std::ios::end);
					auto ByteNumb = streamFile.tellg();
					streamFile.seekg(0, std::ios::beg);

					// Resize the buffer to hold the entire file
					std::vector<char> buffer(ByteNumb);

					// Read the entire file into the buffer
					if (streamFile.read(buffer.data(), ByteNumb)) {
						auto Path_ = FolderName + LR"(\File)" + std::to_wstring(i) + L".h";
						Paths.push_back(Path_);
						std::ofstream FileOpened(Path_);
						if (FileOpened.is_open()) {
							Numbs.push_back(i);
							FileOpened << "#pragma once\n";
							FileOpened << "unsigned char rawData" << std::to_string(i) << "[" << ByteNumb << "] = {";
							for (size_t j = 0; j < ByteNumb; j++) {
								if (j % 11 == 0) {
									FileOpened << "\n" << "      ";
								}
								FileOpened << std::format("0x{:02x}", static_cast<unsigned char>(buffer[j])) << (j + 1 < ByteNumb ? ", " : "");
							}
							FileOpened << "\n};";
						}
					}
					else {
						// Error handling if reading the file fails
						// Handle the case where the file could not be read
					}
				}
				else {
					// Error handling if opening the file fails
					// Handle the case where the file could not be opened
				}
			}
			std::wofstream ResultFile(FolderName + L"/IncludeFile.h");

			ResultFile << L"#pragma once\n";

			ResultFile << L"#include <vector>\n\n";

			for (size_t i = 0; i < Paths.size(); i++)
			{
				ResultFile << LR"(#include ")" << Paths[i] << L'"' << L"\n";
			}

			ResultFile << L"\n  struct ByteData {\n  unsigned char* DataPointer;\n  size_t ByteSize;\n};";

			ResultFile << L"\n\nstd::vector<ByteData> GetBytes() {\n      std::vector<ByteData> byteArray;\n";

			for (size_t i = 0; i < Numbs.size(); i++)
			{
				ResultFile << L"\n      " << "byteArray.push_back({rawData" << std::to_wstring(Numbs[i]) << "," << "sizeof(rawData" << std::to_wstring(Numbs[i]) << ") }); ";
			}

			ResultFile << "\nreturn byteArray;\n}";

		}
	}



	
};


int main()
{
	using namespace Coolshit;

	std::wstring input = L"";

	FileConverter* FileConv = nullptr; 

	int FileCount = 0;

	bool failed = true;

	outputFancy("Enter the Path of the Folder: ", 30);

	std::getline(std::wcin, input);

	std::wstring Dir = input;

	FileConv = new FileConverter(input);

	system("cls");

	FileCount = FileConv->GetEntryCount();

	outputFancy( (FileCount ? std::string("File Count: " + std::to_string(FileCount) + "\n") : "No files were found!"), 30);
	
	if (FileCount) {

		Sleep(2000);

		system("cls");

		outputFancy("Enter the Type the Program should Convert (leave empty for all): ", 30);

		std::getline(std::wcin, input);

		FileConv->GetTypes(input);

		system("cls");

		FileCount = FileConv->GetCleanedEntryCount();

		outputFancy((FileCount ? std::string("File Count: " + std::to_string(FileCount) + "\n") : "No files were found!"), 30);

		if (FileCount) {

			system("cls");

			outputFancy("Convert Data?: ", 30);

			std::getline(std::wcin, input);

			if (auto Decision = TolowerW(input); Decision == L"yes") {

				system("cls");

				outputFancy("Enter the Output Folder Name: ", 30);

				std::getline(std::wcin, input);

				FileConv->CreateData(Dir + LR"(\)" + input);

				failed = false;
			}	
		}

	}

	system("cls");

	delete FileConv;

	if (failed) {
		std::cout << "Program has failed, womp womp restart Program";

		return -1;
	}

	return 0;
}