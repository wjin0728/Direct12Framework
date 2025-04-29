#pragma once
#include<iostream>
#include<fstream>


namespace BinaryReader
{
	template<typename Type>
	inline void ReadDateFromFile(std::ifstream& inFile, Type& date)
	{
		inFile.read(reinterpret_cast<char*>(&date), sizeof(Type));

		if (!inFile) {
			ZeroMemory(&date, sizeof(Type));
		}
	}

	template<>
	inline void ReadDateFromFile(std::ifstream& inFile, std::string& date)
	{
		if (!inFile) return;

		char strLen = 0;

		inFile.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));

		if (!inFile) {
			date.clear();
			return;
		}

		date.resize((size_t)strLen);
		inFile.read(&date[0], strLen);

		if (!inFile) {
			date.clear();
		}
	}

	inline std::wstring stringToWstring(const std::string& str) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
		return wstr;
	}
}
