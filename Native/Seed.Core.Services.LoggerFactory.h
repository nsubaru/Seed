#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed 
{
	inline namespace Core 
	{
		namespace Services 
		{
			/// <summary>
			/// Сервіс реєстрації динамічних логгерів
			/// </summary>
			class LoggerFactoryService : protected IDisposable{

			public:
				/// <summary>
				/// Екземпляр динамічного логгера
				/// </summary>
				class DynamicLogger : protected IDisposable {
				private:
					struct Data
					{
						String moduleName;
						Core::Logger::LoggerLevel loggingLevel;
						Containers::ConcurencyList<DynamicLogger>::Iterator loggerRef;
						
						RefCountT RefCount;

					public:
						Data(String moduleName, Core::Logger::LoggerLevel loggingLevel) :
							moduleName(moduleName),
							loggingLevel(loggingLevel),
							loggerRef(),
							RefCount(1) {}
					};

				private:
					Data* This;

				protected:
					virtual void Dispose();

				private:
					void SetLogLevelName(String& msg, Logger::ELoggerLevel logLevel) {
						if (logLevel == Logger::ELoggerLevel::Critical)
							msg += L"Critical";
						else if (logLevel == Logger::ELoggerLevel::Verbose)
							msg += L"Verbose";
						else if (logLevel == Logger::ELoggerLevel::Debug)
							msg += L"Debug  ";
						else if (logLevel == Logger::ELoggerLevel::Error)
							msg += L"Error  ";
						else if (logLevel == Logger::ELoggerLevel::Warning)
							msg += L"Warning";
						else
							Throw(InvalidArgumentException(ExceptionPosition,L"logLevel", L"Anvalid argument"));
					}

				private:
					DynamicLogger(String moduleName, Core::Logger::LoggerLevel loggingLevel) : 
						This(new Data(moduleName, loggingLevel)) {
					}

				private:
					void SetMessageHeader(String& msg, Logger::ELoggerLevel logLevel) {
						msg += L"[";
						SetLogLevelName(msg, logLevel);
						msg += L" -> ";
						msg += This->moduleName;
						msg += L"] ";
					}

				public:
					/// <summary>
					/// Записати повідомлення в логи
					/// </summary>
					/// <param name="msgLevel">Тип повідомлення</param>
					/// <param name="msg">Повідомлення</param>
					void ToLog(Logger::ELoggerLevel msgLevel,String msg);

				public:
					RefCountClass(DynamicLogger, This);

				public:  
					friend class LoggerFactoryService;
				};

			private:

				struct Data
				{
					Containers::ConcurencyList<DynamicLogger> activeLogers;

					RefCountT RefCount;

				public:
					Data() : activeLogers(), RefCount(1) {}
				};

			private:
				Data* This;

			protected:
				virtual void Dispose() {}

			public:
				LoggerFactoryService() : This(new Data()) {}

			public:
				/// <summary>
				/// Створити новий динамічний логгер
				/// </summary>
				/// <param name="moduleName">Назва модуля логування</param>
				/// <param name="loggingLevel">Рівень логування</param>
				/// <returns></returns>
				DynamicLogger CreateLogger(String moduleName, Core::Logger::LoggerLevel loggingLevel);

			public:
				/// <summary>
				/// Отримати число активних логгерів
				/// </summary>
				/// <returns></returns>
				SizeT GetLoggersCount() const { return This->activeLogers.Count; }

			public:
				/// <summary>
				/// Кількість активних логгерів
				/// </summary>
				SizeT Property(GetLoggersCount) LoggersCount;

			public:
				RefCountClass(LoggerFactoryService, This);
			};
		}
	}
}