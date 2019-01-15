#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Containers.BitMap.h"

namespace Seed {
	inline namespace Core {
		namespace Security {
			//----------------------------------------------------------------//
			//Опис структур маркера доступу до об'єкта                        //
			//----------------------------------------------------------------//

			///<summary>Список можливих прав на об'єкт</summary>
			enum class ACE_Right {
				Read, Write, Execute, MaxRight
			};

			///<summary>Маска прав для об'єкта</summary>
			namespace { using ACEMask = BitMapT<static_cast<SizeT>(ACE_Right::MaxRight)>; }
			///<summary>Ідентифікатор користувача</summary>
			namespace { using UserIDType = UUID; }

			///<summary>Класс запису доступу</summary>
			enum class ACEClass {
				Control, Audit
			};

			///<summary>Тип запису доступу</summary>
			enum class ACEType {
				Allow, Denie
			};

			///<summary>Запису доступу</summary>
			struct ACE : virtual Memory::Allocator<Memory::SecuredHeap> {
				///<summary>Користувач</summary>
				UserIDType UserID;
				///<summary>Класс запису</summary>
				ACEClass Class;
				///<summary>Тип запису</summary>
				ACEType Type;
				///<summary>Маска дозволів запису</summary>
				ACEMask Mask;
			};

			class SecurableObject : public virtual Memory::Allocator<Memory::SecuredHeap> {
				struct Data {
					void * SecurityDescriptor;
					RefCountT RefCount;
				};
				Data * This;
			public:
				SizeT Property(GetACECount) ACECount;

				void AddACEToDescriptor(ACE & ace);
				void RemoveACEFromDescriptor(SizeT Index);
				const ACE & GetACEbyIndex(SizeT Index);
				SizeT GetACECount();
				void VerifyAcess(ACE_Right Right);
				void Dtor();
			public:
				SecurableObject(UserIDType & OwnerID);				
				SecurableObject & GetSecurityObjectDescriptor();
				SecurableObject(const SecurableObject & Orig);
				SecurableObject(SecurableObject && From);
				SecurableObject & operator=(const SecurableObject & Orig);
				SecurableObject & operator=(SecurableObject && From);
				~SecurableObject();
			};

			//----------------------------------------------------------------//
			//Опис прав токена та користувача                                 //
			//----------------------------------------------------------------//
			
			enum class UserRights {
				ReadSettings,
				SaveSettings,
				ReadCoreLog,
				WriteCoreToLog,
				ReadUserRights,
				ChangeUserRights,
				ReadTokenRights,
				ChangeTokenRights,
				ReadSecurityDescriptor,
				ModifySecurityDescriptor,
				MAX_RIGHT
			};

			void TryRequestRight(UserRights right, const Char * const Method);
			void RemoveRight(UserRights right);

			Boolean RequireUserRight(UserRights right, const AChar * const Method, const Char * const Description);

#define TryWhithRight(Right, WhithRight){\
Seed::Security::TryRequestRight(Right, __FUNCSIG__);\
WhithRight;\
Seed::Security::RemoveRight(Right)\
}
		}
	}
}