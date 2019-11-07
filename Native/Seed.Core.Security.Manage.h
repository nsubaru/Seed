//#pragma once
//#include "Seed.Core.Security.h"
//
//namespace Seed {
//	inline namespace Core {
//		namespace Security {
//			namespace Manage {
//				namespace User {
//					void CreateUser(
//						const SensitiveString & Name,
//						const SensitiveString & FullName,
//						Array<GroupIDType> MemberGroups,
//						Array<Byte> HashedPswd
//					);
//
//					void AddUserToGroup(UserIDType User, GroupIDType Group);
//
//					void ChangeUserName(UserIDType User, SensitiveString & NewName);
//
//					void ChangeFullUserName(UserIDType User, SensitiveString & NewFullName);
//
//					void DisableUser(UserIDType User);
//
//					void DeleteUser(UserIDType User);
//				}
//
//				namespace Group {
//					void CreateGroup(
//						const SensitiveString & Name,
//						const SensitiveString & FullName,
//						Array<GroupIDType> MemberOfGroups,
//						Array<UserIDType> Members
//					);
//
//					void AddGroupToGroup(GroupIDType MemberGroup, GroupIDType Group);
//
//					void ChangeGroupName(GroupIDType Group, SensitiveString & NewName);
//
//					void ChangeFullGroupName(GroupIDType Group, SensitiveString & NewFullName);
//
//					void DisableGroup(GroupIDType Group);
//
//					void DeleteGroup(GroupIDType Group);
//				}
//			}
//		}
//	}
//}