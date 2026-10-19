namespace Seed.XMLDocGen;

internal sealed class NetworkingPage : ISourcePage
{
    public string FileName => "networking.md";

    public string Title => "Networking, DNS and DHCP";

    public string Render(PageContext context) => """
        # Networking, DNS and DHCP

        Core networking uses NTAPI AFD sockets with synchronous and asynchronous TCP
        and UDP wrappers. NetworkService exposes interface and address information.
        TCP bind completion does not own a secondary transport handle; synchronous
        and asynchronous cleanup close only the socket handle they created. TCP test
        clients use Windows-assigned local ports so repeated connections do not reuse
        a client endpoint still retained by Windows.
        DnsClientService owns resolution, caching, configured DNS servers and transient
        server contributions.

        TcpByteStream supplies the same async byte interface for connected overlapped
        clients and accepted overlapped server sockets. Read returns actual received
        bytes and an empty blob at normal TCP closure. Write finishes every byte across
        partial operations within one deadline, retaining its own buffer and socket.
        A timed-out overlapped operation cancels and drains its exact native request
        before releasing completion state or buffers. Repeated accept timeouts and
        timed-out reads have a follow-up exchange regression test.
        Read capacities range from one byte to 64 KiB. Callers serialize reads and
        serialize writes; one read may overlap one write. TLS clients and servers use this
        transport. Cancellation prevents new reads/writes and cancels submitted operations;
        a racing operation can reach its bounded timeout before final ownership release. Tools/Test-SeedTlsIdentity.ps1 checks a fragmented 256 KiB round trip
        through both transport directions alongside certificate identity loading.

        DhcpService contains DHCP packet processing and lease acquisition, renewal,
        rebinding and release. Its transient IP addresses and routes use the native
        network service. Interface mutation can fail because of Windows permissions;
        ordinary DNS and HTTP startup must not request those permissions globally.

        LeaseMaintenanceService owns explicitly selected interface workers, their
        cancellation and shutdown order. It acquires leases, schedules renewal and
        rebinding, releases expired configuration and requests another acquisition.
        Missing or invalid server timers use normalized DHCP defaults; large finite
        lifetimes do not overflow, and infinite leases without renewal timers stay bound.
        Failed acquisition preserves its original error and retains any owned
        address/route cleanup. A route ledger tracks both gateways if reconfiguration
        fails between installing a new route and removing an old one. Stop can retry
        pending cleanup. DNS contributions are withdrawn from releasing leases.

        Isolated DHCP lifecycle and interface rollback validation remain unfinished.
        Do not use an unvalidated extraction to
        change a production interface. The initial native NSI layout baseline is Windows
        build 26100; supported system builds need explicit validation.

        No DHCP maintenance worker starts automatically. Use `network.interfaces` to
        identify an interface, then `network.dhcp.start <interface-index>` only in the
        isolated fixture. Acquisition requires a connected eligible interface without
        a usable IPv4 address. `network.dhcp.status` reports acquisition errors and
        cleanup state; `network.dhcp.stop <interface-index>` cancels and joins the
        selected worker and releases its owned configuration. The worker survives
        console disconnection, and runtime shutdown stops all workers before their
        dependencies are torn down. See [console commands](kms.md).

        The HTTP component provides an HTTP/1.1 parser, outbound client and raw/reflected
        JSON handlers. Both client and server use Seed TLS for HTTPS; see [TLS](tls.md)
        for negotiation, listener configuration and limits. The native client interoperates with independent .NET TLS
        1.2/1.3 servers using AES-GCM. Tools/Test-SeedTlsClient.ps1 also checks wrong
        DNS names and exact IPv4/IPv6 certificate names, redirects, chunked and
        informational responses, ambiguous framing, truncation, untrusted roots,
        expired certificates and incompatible versions,
        and verifies the native process loads only its executable and ntdll.dll.

        Sources are listed in [components](components.md); compiler API documentation
        is under [API reference](api/index.md).
        """;
}
