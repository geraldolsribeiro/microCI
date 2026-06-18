# The CI/CD Problem

## Modern pipelines are often trapped inside platforms

Most CI/CD workflows become tightly coupled to a specific provider, runner, plugin ecosystem, or hosted service.

\vspace{0.5em}

Common pain points:

- Pipeline logic is hidden behind platform-specific configuration
- Local reproduction is difficult or impossible
- Migration between CI vendors is expensive
- Debugging requires access to remote infrastructure
- Auditability depends on third-party execution details

\vspace{0.5em}

\begin{center}
\Large The pipeline works... until it must run somewhere else.
\end{center}

