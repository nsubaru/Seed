#include "Seed.Core.Services.LoggerFactory.h"

using namespace Seed;
using namespace Seed::Core::Services;

LoggerFactoryService::DynamicLogger Seed::Core::Services::LoggerFactoryService::CreateLogger(
    String moduleName, 
    Core::Logger::LoggerLevel loggingLevel)
{
    auto logger = DynamicLogger(moduleName, loggingLevel);
    logger.This->loggerRef = This->activeLogers.AddLast(logger);
    logger.This->RefCount--;
    return logger;
}

void Seed::Core::Services::LoggerFactoryService::DynamicLogger::Dispose()
{
    This->RefCount = 2;
    This->loggerRef.Remove(false);
}

void Seed::Core::Services::LoggerFactoryService::DynamicLogger::ToLog(Logger::ELoggerLevel msgLevel, String&& msg)
{
    if (This->loggingLevel & msgLevel) {
        auto buffer = String(1Kb);
        SetMessageHeader(buffer, msgLevel);
        buffer + L"  ";
        buffer += msg;
        toCoreLog(buffer);
    }
}

void Seed::Core::Services::LoggerFactoryService::DynamicLogger::ToLog(Logger::ELoggerLevel msgLevel, const String& msg) {
    if (This->loggingLevel & msgLevel)
    {
        auto buffer = String(1Kb);
        SetMessageHeader(buffer, msgLevel);
        buffer + L"  ";
        buffer += msg;
        toCoreLog(buffer);
    }
}
