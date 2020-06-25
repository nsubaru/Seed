// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Seed.Core.FS.h"

#include "Seed.Core.Testing.Tests.FS.FileObject.h"

#include "Seed.Core.FS.h"

bool Seed::Core::Testing::Test::FS::API::CreateFile::Excecute() {
	Core::FS::RawCreateFile(".\\test", Core::FS::File::ReadMode | Core::FS::File::DeleteMode, true).Delete();
	return true;
}

bool Seed::Core::Testing::Test::FS::API::DeleteFile::Excecute() {
	Core::FS::RawCreateFile(".\\test", Core::FS::File::ReadMode | Core::FS::File::DeleteMode, true).Delete();
	return true;
}

bool Seed::Core::Testing::Test::FS::API::OpenExist::Excecute() {
	{
		Core::FS::RawCreateFile(".\\test", Core::FS::File::ReadMode, true);
	}
	{
		Core::FS::RawOpenFile(".\\test", Core::FS::File::ReadMode | Core::FS::File::DeleteMode).Delete();
	}
	return true;
}

bool Seed::Core::Testing::Test::FS::API::WriteFile::Excecute() {
	auto file = Core::FS::RawCreateFile(".\\test", Core::FS::File::WriteMode | Core::FS::File::DeleteMode, true);
	
	file.Write(L"test"S);
	MoveRValue(file).Delete();
	
	return true;
}

bool Seed::Core::Testing::Test::FS::API::ReadFile::Excecute() {
	constexpr auto itemCount = 20;

	auto output = Core::Containers::Blob(itemCount);
	auto input = Core::Containers::Blob::Generate([] (SizeT index)
	{
		return static_cast<UInt8>(index);
	}, itemCount);
	
	auto file = Core::FS::RawCreateFile(".\\test", Core::FS::File::ReadMode | Core::FS::File::WriteMode | Core::FS::File::DeleteMode, true);
	
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
