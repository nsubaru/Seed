#pragma once
#include "Seed.Core.IO.FS.Base.h"
#include "Seed.Core.IO.FS.Exceptions.h"
#include "Seed.Core.IO.FS.File.h"
#include "Seed.Core.IO.FS.AsyncFile.h"

namespace Seed::Core::IO::FS
{
    class Directory final: public IDisposable {
    private:
        struct Data {
            Handle hDir;
            String path;
            OpenDisposition openDisposition;
            OpeningMode openMode;
            bool shareWithOs;

            RefCountT RefCount;

            Data(Handle hDir,
                 String path,
                 OpeningMode openMode,
                 OpenDisposition openDisposition,
                 bool shareWithOs
            ) :
                hDir(hDir),
                path(path),
                openMode(openMode),
                openDisposition(openDisposition),
                shareWithOs(shareWithOs),
                RefCount(1)
            {}
        };

    private:
        Data* This;

    private:
        virtual void Dispose() override;

    public:
        /// <summary>
        /// Creates the directory object and opens it or creates file in system
        /// </summary>
        /// <param name="path">A path to file that can be in "NT" or "Win32" format</param>
        /// <param name="mode">File mode</param>
        /// <param name="policy">File opening policy</param>
        /// <param name="ShareWithOs">The file sharing flag with the rest of system</param>
        Directory(String&& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs);

        /// <summary>
        /// Creates the directory object and opens it or creates file in system
        /// </summary>
        /// <param name="path">A path to file that can be in "NT" or "Win32" format</param>
        /// <param name="mode">File mode</param>
        /// <param name="policy">File opening policy</param>
        /// <param name="ShareWithOs">The file sharing flag with the rest of system</param>
        Directory(const String& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs) : Directory(String(path), mode, policy, ShareWithOs) {}

    public:
        /// <summary>
        /// Returns full directory path
        /// </summary>
        /// <returns>Full directory path</returns>
        String GetFullDirectoryPath() const;

    public:
        RefCountClass(Directory);
    };
}