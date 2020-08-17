#pragma once
namespace Seed {
	inline namespace Core {
		void Stage1(void* logFileStructData) noexcept;
		void Stage1Complete() noexcept;
		void Stage2() noexcept;
		void RuntimeEntryPoint();
	}
}