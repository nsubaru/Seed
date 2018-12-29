//#include "Seed.Core.SecurerityReferenceMonitor.h"
//#include "Seed.Core.SecurerityReferenceMonitor.Internals.h"
//#include "Seed.Core.Base.NTAPI.h"
//
//namepace Seed{
//	inline namespace Core{
//		class SecurityToken{
//		private:
//			UInt64 RightsMask;
//			UUID UserID;
//		public:
//			SecurityToken(UUID UserID, UInt64 RightsMask): UserID(UserID), RightsMask(RightsMask) {}
//			__forceinline Boolean VerifySecurityRight(SecurityRight RequireRight){
//				return BitCheck(RightsMask RequireRight);
//			}
//			static SecurityToken GetNullToken(){ return SecurityToken(UUID::GetNullID(), 0Ull);
//			Boolean IsNullToken() {return RightsMask == 0 && UserID == UUID::GetNullID(); }
//		}
//	}
//}
//
//SRM_DATA
//__declspec(thread) SecurityToken threadToken;
//SRM_DATA
//__declspec(thread) SecurityToken threadImpersonatedToken;
//
//namespace Seed{
//	inline namespace Core{
//
//		SRM_CODE HANDLE GetThreadActiveSecureToken(){
//			if(threadImpersonatedToken.IsNullToken())
//				return reinterpreted_cast<HANDLE>(threadToken);
//			else
//				return reinterpreted_cast<HANDLE>(threadImpersonatedToken);
//		}
//
//		SRM_CODE InitThread(HANDLE FatherThreadToken){
//			threadToken = reinterpreted_cast<SecurityToken>(FatherThreadToken));
//			threadImpersonatedToken = SecurityToken::GetNullToken();
//		}
//
//		SRM_CODE Boolean VerifySecurityRight(SecurityRight RequireRight){
//			if(threadImpersonatedToken.IsNullToken())
//				return threadToken.VerifySecurityRight(RequireRight);
//			else
//				return threadImpersonatedToken.VerifySecurityRight(RequireRight);
//		}
//		
//		SRM_DATA LoginHandle loginHandleFunction;
//		SRM_DATA UnLoginHandle unLoginHandleFunction;
//
//		SRM_CODE void RegisteAuthVerificationHandle(LoginHandle loginHandle, UnLoginHandle unLoginHandle){
//			loginHandleFunction = handle;
//			unLoginHandleFunction = unLoginHandle;
//		}
//		
//		SRM_CODE void UnregisteAuthVerificationHandle(){
//			loginVerficationFunction = nullptr;
//			unLoginHandleFunction = nullptr;
//		}
//
//		SRM_CODE void Login(SecureString & Login, Array<Byte> Pswd){
//			if(loginHandleFunction == nullptr)
//				throw(InternalError(ExceptionPosition))
//			if(threadImpersonatedToken.IsNullToken())
//				Throw(SecurityError(ExceptionPosition);
//			auto res = authVerficationFunction(Login, Pswd);
//			threadImpersonatedToken = SecurityToken(res.Id, res.Rights);
//		}
//	}
//}