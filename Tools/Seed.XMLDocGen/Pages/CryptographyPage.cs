namespace Seed.XMLDocGen;

internal sealed class CryptographyPage : ISourcePage
{
    public string FileName => "cryptography.md";

    public string Title => "Cryptography and certificates";

    public string Render(PageContext context) => """
        # Cryptography and certificates

        Core cryptography includes protected memory, randomness, hashes, AES/GCM,
        public-key algorithms and X.509 parsing. Algorithms are reusable Core facilities.
        TLS uses the native implementations rather than external native crypto libraries.

        The TLS-owned CertificateStoreService keeps parsed certificate records and DER
        payloads in memory, grouped by category and name. Insertion parses the certificate
        before locking and replaces a matching category/name atomically. Reads return
        owned snapshots.

        TlsServerIdentity imports an explicit leaf-first DER chain and a matching
        unencrypted RSA key. FromFiles accepts one DER certificate or up to eight
        strict ASCII PEM CERTIFICATE blocks, plus PKCS#1/PKCS#8 DER or single-block
        private-key PEM. Select the DER key format explicitly; PEM is the default.
        Certificate input is limited to 128 KiB total and 64 KiB per certificate;
        private files are limited to 16 KiB DER or 32 KiB PEM. Unrelated labels/text,
        malformed padding, duplicate certificates, wrong keys, expired certificates,
        unsuitable server purposes and invalid supplied chain links are rejected.
        The chain may end at an intermediate: local import does not establish client
        trust or install a root. Certificate times are recheckable before each handshake.

        Identity file handles deny write sharing while reading. Core File.Read's
        sensitive overload reads directly into SensitiveBlob, checks exact byte counts
        and clears its destination on failure. Private-key bytes never pass through an
        ordinary string or blob in this loading path. Identity snapshots copy public
        DER; signing uses Core RsaSigner and enforces RSA-PSS for TLS 1.3.
        The [TLS server](tls.md) uses this identity for handshake signatures and
        rechecks certificate time for each connection.

        Core RsaSigner imports unencrypted PKCS#1 and PKCS#8 DER from SensitiveBlob.
        Its PEM importer also takes sensitive ASCII bytes and accepts one PRIVATE KEY
        or RSA PRIVATE KEY block with surrounding whitespace. Extra text/blocks,
        mismatched labels and noncanonical Base64 padding are rejected. Private
        bytes must remain in sensitive storage when reading files or decoding text.
        Encrypted keys, PKCS#8 attributes, multiprime RSA and exponents other than
        65537 are unsupported; modulus sizes are 2048, 3072 and 4096 bits.

        Signing supports SHA-256/384/512 with PKCS#1 v1.5 or PSS. PSS uses the same
        hash for MGF1 and a fresh hash-sized salt. Private exponentiation has fixed
        padded loop bounds, fresh 128-bit exponent blinding and a public verification
        before returning the signature. Each operation owns a sensitive workspace;
        immutable identities can be shared between callers. CRT values are not used.
        Import checks structure, the prime product and a signing self-test; it does
        not certify primality. Public BigInteger/RsaVerifier arithmetic is variable-time
        and must not be used for private operations. Compiler changes require another
        review of the generated private arithmetic. Cryptographic release review remains pending.

        Signature/key formats follow [PKCS#1](https://www.rfc-editor.org/rfc/rfc8017),
        [PKCS#8](https://www.rfc-editor.org/rfc/rfc5208) and the labelled
        [text encoding](https://www.rfc-editor.org/rfc/rfc7468); the PEM parser's
        accepted text is deliberately narrower than general permissive PEM readers.

        TLS client code includes TLS 1.2 and 1.3 negotiation and peer verification.
        Certificate-chain and hostname validation defaults must remain enabled. Imported
        trust data needs its own provenance and redistribution notices. This checkout
        has not yet passed cryptographic or TLS release acceptance.

        The first release scope excludes HTTP/2, HTTP/3, QUIC, ACME, online revocation
        fetching, session resumption, 0-RTT and full mutual TLS.
        """;
}
