#pragma once
#ifndef __INTELLISENSE__
#ifndef DECLSPEC_GUARDNOCF
#if (_MSC_FULL_VER >= 170065501) || defined(_D1VERSIONLKG171_)
#define DECLSPEC_GUARDNOCF  __declspec(guard(nocf))
#else
#define DECLSPEC_GUARDNOCF
#endif
#endif

#ifndef DECLSPEC_GUARD_SUPPRESS
#if (_MSC_FULL_VER >= 181040116) || defined(_D1VERSIONLKG171_)
#define DECLSPEC_GUARD_SUPPRESS  __declspec(guard(suppress))
#else
#define DECLSPEC_GUARD_SUPPRESS
#endif
#endif
#else
#ifndef DECLSPEC_GUARDNOCF
#define DECLSPEC_GUARDNOCF
#endif

#ifndef DECLSPEC_GUARD_SUPPRESS
#define DECLSPEC_GUARD_SUPPRESS
#endif
#endif // !__INTELLISENSE__