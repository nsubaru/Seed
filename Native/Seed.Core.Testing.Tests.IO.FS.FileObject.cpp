// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Seed.Core.IO.FS.h"

#include "Seed.Core.Testing.Tests.IO.FS.FileObject.h"


bool Seed::Core::Testing::Test::IO::FS::API::CreateFile::Excecute() {
	Core::IO::FS::RawCreateFile(".\\test", Core::IO::FS::File::ReadMode | Core::IO::FS::File::DeleteMode, true).Delete();
	return true;
}

bool Seed::Core::Testing::Test::IO::FS::API::DeleteFile::Excecute() {
	Core::IO::FS::RawCreateFile(".\\test", Core::IO::FS::File::ReadMode | Core::IO::FS::File::DeleteMode, true).Delete();
	return true;
}

bool Seed::Core::Testing::Test::IO::FS::API::OpenExist::Excecute() {
	{
		Core::IO::FS::RawCreateFile(".\\test", Core::IO::FS::File::ReadMode, true);
	}
	{
		Core::IO::FS::RawOpenFile(".\\test", Core::IO::FS::File::ReadMode | Core::IO::FS::File::DeleteMode).Delete();
	}
	return true;
}

bool Seed::Core::Testing::Test::IO::FS::API::WriteFile::Excecute() {
	auto file = Core::IO::FS::RawCreateFile(".\\test", Core::IO::FS::File::WriteMode | Core::IO::FS::File::DeleteMode, true);
	
	file.Write(L"test"S);
	MoveRValue(file).Delete();
	
	return true;
}

bool Seed::Core::Testing::Test::IO::FS::API::ReadFile::Excecute() {
	constexpr auto itemCount = 20;

	auto output = Core::Containers::Blob(itemCount);
	auto input = Core::Containers::Blob::Generate([] (SizeT index)
	{
		return static_cast<UInt8>(index);
	}, itemCount);
	
	auto file = Core::IO::FS::RawCreateFile(".\\test", Core::IO::FS::File::ReadMode | Core::IO::FS::File::WriteMode | Core::IO::FS::File::DeleteMode, true);
	
	file.Write(input);
	
	file.Read(output);
	
	MoveRValue(file).Delete();
	
	for (SizeT i = 0; i < itemCount; i++)
	{
		if (input[i] != output[i])
		{
			return false;
		}
	}
	return true;
}
