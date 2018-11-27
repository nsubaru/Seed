#pragma once
//#include "Seed.Core.Base.h"
//#include "Seed.Core.Exceptions.h"
//#include "Seed.Core.String.h"
//#include "Seed.Core.Array.h"
//
//namespace Seed{
//	inline namespace Core{
//		enum SecurityRights{
//			Stop,
//			StopHost,
//			Restart,
//			ChangeSecurityRight,
//			ModifySystemCoreData,
//		}
//
//		class SecurityExeption : public Exeption{
//		public:
//			SecurityExeption(ACharStr FileName, ACharStr FunctioName, UInt64 Line)
//				:Exeption(FileName, FunctionName, Line, L"Security error")
//			{}
//		}
//		
//		SRM_CODE Boolean VerifySecurityRight(SecurityRight RequireRight);	
//
//		SRM_CODE void Login(SecureString & Login, Array<Byte> Pswd);
//	}
//}