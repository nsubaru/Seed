namespace Seed.XMLDocGen;

internal sealed class TlsPage : ISourcePage
{
    public string FileName => "tls.md";

    public string Title => "HTTP over TLS";

    public string Render(PageContext context) => """
        # HTTP over TLS

        Seed performs TLS negotiation, certificate parsing, signing and record
        protection with its own native networking and cryptography. No external
        native HTTP, TLS or cryptography library is loaded into the runtime.
        Development and test peers run as separate .NET/OpenSSL processes.

        | Facility | Current implementation |
        |---|---|
        | Protocols | TLS 1.2 and TLS 1.3 full client/server handshakes |
        | Key exchange | X25519, with all-zero shared-secret rejection |
        | TLS 1.2 suites | ECDHE-RSA with AES-128/256-GCM and SHA-256/384 |
        | TLS 1.3 suites | AES-128-GCM-SHA256 and AES-256-GCM-SHA384 |
        | Server identity | Matching RSA certificate and unencrypted private key |
        | Server signatures | RSA-PSS SHA-256/384/512; TLS 1.2 also PKCS#1 |
        | Client verification | RSA/ECDSA, certificate chain, hostname and validity |
        | Application protocol | HTTP/1.1 ALPN; absent client ALPN permits HTTP/1.1 |
        | Retry | TLS 1.3 HelloRetryRequest for X25519, with transcript message_hash |
        | TLS 1.2 transcript | Extended master secret when offered; secure renegotiation signalling |
        | Closing | Authenticated close-notify; server rejects record truncation |

        TlsServerStream owns a common TcpByteStream and a retained immutable
        TlsServerIdentity. AuthenticateAsServer completes client Finished verification
        before application reads are enabled. It bounds records, reassembles fragmented
        handshakes and checks duplicate extensions, algorithm negotiation and retry
        consistency. Private and directional traffic secrets use sensitive memory and
        are released on authentication failure or close. A traffic key is limited to
        2^24 records. TLS 1.3 incoming KeyUpdate changes directional secrets and can
        request a reply; callers serialize stream operations.

        ConfigureTlsListener selects an identity and TLS policy before HTTPS endpoint
        registration starts the listener. Raw and reflected JSON routes are shared
        with plaintext HTTP. A failed TLS handshake does not send a plaintext HTTP
        error response. The listener retains connection tasks, cancels their transports
        and joins them before releasing sockets and routes. Endpoint selection copies
        a handler under its route lock and invokes it outside that lock.

        Demonstrations bind to 127.0.0.1, with HTTP on 8080 and HTTPS disabled by
        default on port 8443. Each listener allows 16 active connections, requests up
        to 16 MiB, 5-second transport operations and a 30-second overall connection
        deadline. It serves one request per connection and closes after its response.
        A racing transport operation may reach its timeout during cancellation.

        Generate a fresh local identity with Tools/New-SeedDevelopmentIdentity.ps1.
        The leaf is valid for one day and includes localhost and 127.0.0.1; its root
        is not installed into Windows trust. Configure host.tls.certificateFile and
        host.tls.privateKeyFile, then host.https.enabled. Save and reload explicitly.
        The private-key format defaults to pem; pkcs8-der and pkcs1-der are also
        accepted. See [configuration](configuration.md) and [cryptography](cryptography.md).

        Tools/Test-SeedTlsServer.ps1 uses fresh development identities and independent
        .NET clients for both versions, fragmented binary echo and reflected JSON,
        wrong hostnames, untrusted roots, unsupported ALPN and modified protected
        records. It checks reload with an incomplete handshake and ntdll-only native
        module loading. When OpenSSL is available, an additional matrix covers both
        key sizes, TLS 1.2 PKCS#1 signing and legacy master-secret derivation, and
        verifies successful TLS 1.3 retries by their wire marker.

        Client trust and local identity import are distinct. Local import validates
        the supplied chain links and matching key; it does not establish peer trust
        or provide a complete PKIX policy engine. Cryptographic release review and
        exhaustive malformed/invalid-Finished, signing and key-update acceptance
        tests remain pending. The checkout is not a validated release.

        HTTP/2/3, QUIC, ACME, online revocation fetching, session resumption,
        0-RTT and full mutual TLS are outside the first release scope.
        Protocol references: [TLS 1.3](https://www.rfc-editor.org/rfc/rfc8446),
        [TLS 1.2](https://www.rfc-editor.org/rfc/rfc5246) and
        [extended master secret](https://www.rfc-editor.org/rfc/rfc7627).
        """;
}
