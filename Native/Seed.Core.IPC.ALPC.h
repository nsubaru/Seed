//#pragma once
//#include "Seed.Core.IPC.h"
//
//namespace Seed {
//	inline namespace Core {
//		namespace IPC {
//			namespace ALPC {
//				constexpr SizeT MaxMessageSize = 1Kb / 2;
//
//				class ALPCClient;
//
//				class ALPCServer {
//				public:
//					typedef bool(*Verify)(Byte(&Mesage)[MaxMessageSize]);
//				private:
//					struct Data {
//						Handle ServerPort;
//						String ALPCPortName;
//						SizeT MaxMsgSize;
//						SizeT ServerMemorySectionSize;
//						SizeT ClientMemorySectionSize;
//						Verify ClientConnectionVerificationFunction;
//						List<ALPCClient> Clients;
//					};
//
//				public:
//					ALPCServer(String ALPCPortName, SizeT MaxMessageSize, SizeT ServerMemorySectionSize, SizeT ClientMemorySectionSize)
//				};
//
//				class ALPCClient {
//				private:
//					friend ALPCServer;
//				};
//			}
//		}
//	}
//}