//#pragma once
//#include "Seed.Core.Security.h"
//
//namespace Seed {
//	inline namespace Core {
//		namespace Security {
//			namespace Manage {
//				namespace User {
//					void CreateUser(
//						const SecuredString & Name,
//						const SecuredString & FullName,
//						Array<GroupIDType> MemberGroups,
//						Array<Byte> HashedPswd
//					);
//
//					void AddUserToGroup(UserIDType User, GroupIDType Group);
//
//					void ChangeUserName(UserIDType User, SecuredString & NewName);
//
//					void ChangeFullUserName(UserIDType User, SecuredString & NewFullName);
//
//					void DisableUser(UserIDType User);
//
//					void DeleteUser(UserIDType User);
//				}
//
//				namespace Group {
//					void CreateGroup(
//						const SecuredString & Name,
//						const SecuredString & FullName,
//						Array<GroupIDType> MemberOfGroups,
//						Array<UserIDType> Members
//					);
//
//					void AddGroupToGroup(GroupIDType MemberGroup, GroupIDType Group);
//
//					void ChangeGroupName(GroupIDType Group, SecuredString & NewName);
//
//					void ChangeFullGroupName(GroupIDType Group, SecuredString & NewFullName);
//
//					void DisableGroup(GroupIDType Group);
//
//					void DeleteGroup(GroupIDType Group);
//				}
//			}
//		}
//	}
//}