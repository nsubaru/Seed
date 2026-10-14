# Contributing to Seed

Read [AGENTS.md](AGENTS.md), follow the owning subsystem's runtime patterns and
add meaningful XML documentation to public declarations. Preserve native source
encoding and keep registration carriers in the owning component.

Run focused tests, then the relevant acceptance checks. Include what was tested
and identify limitations in the change description. Native HTTP/TLS changes need
independent peers and negative protocol tests; compilation is insufficient.

Contributions are made under Apache-2.0 for Seed-owned code. Preserve third-party
licenses and provenance. Add a Developer Certificate of Origin sign-off to each
contribution with `git commit -s`, confirming that you have the right to submit
the code under the project license. See [DCO 1.1](https://developercertificate.org/).

Public release remains pending contribution rights, header provenance and full
runtime/protocol acceptance.
