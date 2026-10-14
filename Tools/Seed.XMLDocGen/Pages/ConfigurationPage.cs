namespace Seed.XMLDocGen;

internal sealed class ConfigurationPage : ISourcePage
{
    public string FileName => "configuration.md";

    public string Title => "Core and host configuration";

    public string Render(PageContext context) => """
        # Core and host configuration

        Core retains typed early-startup configuration and its compiled defaults.
        SEED_CONFIG_DIRECTORY selects a writable configuration directory. If absent,
        the runtime uses Seed.Data beside its executable. The current early-startup
        configuration file is core.v1.bin, containing a Seed identity, format version,
        payload size and configuration snapshot.

        A missing file uses compiled defaults. Invalid sizes, values, an incompatible
        version or an incorrect identity fail startup with a native diagnostic. Value
        validation covers heap and stack bounds, reservation counts, shutdown and timer
        ranges, logger masks, AES implementation values and boolean representations.
        Startup does
        not replace an invalid file or save defaults automatically. configurations.save
        validates an atomic-field snapshot before opening the existing file, and
        persists and flushes it only when explicitly requested.

        configurations.list describes the supported Core settings, their current
        values, units, ranges and when changes take effect. configurations.get takes
        one listed name; configurations.set takes a name and an unsigned decimal
        value. Values are checked before applying a change. The exposed controls cover
        new string/stack/queue reserve sizes, finish-event caching, shutdown timeout
        and timer coalescing. They need no runtime restart and are not saved implicitly.
        configurations.reset restores one exposed setting, or all exposed settings
        when no name is supplied, to this build's compiled defaults.

        The runtime loads reflected HostOptions after Core service initialization.
        The fixed filename host.v1.bin now contains host envelope version 2, with a payload size and
        new fixed schema identity independent of live RTTI addresses. The previous host
        envelope version is incompatible and rejected; there is no implicit migration. File size, string lengths, UTF-16 and logger
        counts are checked before native deserialization. Semantic validation rejects
        invalid IPv4 addresses, zero ports, non-local pipe names and duplicate or
        out-of-range logger overrides. An invalid file fails startup unchanged.

        | Name | Compiled default | Effect |
        |---|---|---|
        | host.http.enabled | true | Start or disable the demonstration HTTP listener |
        | host.http.bindAddress | 127.0.0.1 | Numeric IPv4 bind address |
        | host.http.port | 8080 | TCP port in 1..65535 |
        | host.https.enabled | false | Start HTTPS with explicit certificate/key files |
        | host.https.port | 8443 | HTTPS port, distinct from enabled HTTP |
        | host.tls.privateKeyFormat | pem | pem, pkcs8-der or pkcs1-der |
        | host.kms.pipeName | Seed.KMS.v1 | Local Seed. pipe name |
        | host.tls.trustBundleFile | empty | Additional DER trust root for native client requests |
        | host.tls.certificateFile | empty | Persisted server certificate location |
        | host.tls.privateKeyFile | empty | Persisted server private-key location |

        Host set/reset edits remain pending until explicit save and reload. Reload
        without save discards them. configurations.save validates and serializes the
        reflected host record, flushes a same-directory temporary file, replaces the
        host file, and separately saves Core configuration. These are two files;
        failure can leave only one saved, so the command reports a failure instead
        of claiming a joint transaction. Private-key bytes are never configuration
        values. The certificate/key locations are stored for server composition;
        HTTPS composition loads and validates them through TlsServerIdentity. Enable
        HTTPS after setting both paths; an invalid identity fails startup explicitly.

        logging.set applies a validated live mask and records its host override.
        configurations.save persists those overrides; reload applies saved masks
        after the dynamic loggers have been created. A full configurations.reset
        removes the pending overrides, with compiled logger defaults restored after
        save and reload. Use Seed.Console --pipe <name> for a changed local endpoint.
        status.kernel identifies each runtime composition so clients can wait for
        a requested reload to finish before checking the new listener or pipe.
        """;
}
