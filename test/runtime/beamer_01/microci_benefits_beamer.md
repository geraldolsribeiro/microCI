# What is microCI?

## A portable pipeline compiler

\textbf{microCI} lets teams describe automation once and generate plain, auditable scripts that can run across environments.

\vspace{0.8em}

\begin{center}
\begin{tikzpicture}[node distance=2.2cm, every node/.style={font=\large}]
  \node[draw, rounded corners, fill=blue!10, minimum width=3.2cm, minimum height=1cm] (yaml) {YAML Pipeline};
  \node[draw, rounded corners, fill=green!10, minimum width=3.2cm, minimum height=1cm, right of=yaml, xshift=3cm] (scripts) {Bash Scripts};
  \node[draw, rounded corners, fill=orange!10, minimum width=3.2cm, minimum height=1cm, right of=scripts, xshift=3cm] (runtime) {Docker Tasks};
  \draw[->, thick] (yaml) -- (scripts);
  \draw[->, thick] (scripts) -- (runtime);
\end{tikzpicture}
\end{center}

\vspace{0.8em}

\begin{center}
\Large Write once. Inspect everything. Run anywhere.
\end{center}

# Core Benefits

## Why use microCI?

\begin{columns}
\column{0.5\textwidth}
\textbf{For developers}

- Run the same pipeline locally
- Debug with ordinary shell tools
- Avoid CI-only behavior surprises
- Reduce feedback loop time

\column{0.5\textwidth}
\textbf{For teams}

- Standardize build automation
- Preserve execution transparency
- Reduce vendor lock-in
- Improve onboarding and review

\end{columns}

\vspace{1em}

\begin{center}
\Large microCI turns CI/CD into versioned, inspectable automation.
\end{center}

# Portability

## One workflow, many environments

microCI-generated scripts can be executed in multiple places without rewriting the pipeline model.

\vspace{0.6em}

\begin{center}
\begin{tabular}{lll}
\toprule
\textbf{Environment} & \textbf{Use case} & \textbf{Benefit} \\
\midrule
Developer machine & Local build/test & Fast feedback \\
CI runner & Automated verification & Consistency \\
Release server & Packaging/deployment & Controlled execution \\
Production support & Operational tasks & Repeatability \\
\bottomrule
\end{tabular}
\end{center}

\vspace{0.8em}

\begin{center}
\Large The pipeline is no longer a property of the CI provider.
\end{center}

# Auditability

## Generated scripts make behavior explicit

Instead of hiding execution inside plugins or opaque runner behavior, microCI generates scripts that can be inspected, reviewed, versioned, and executed directly.

\vspace{0.6em}

This improves:

- Security review
- Root-cause analysis
- Compliance evidence
- Long-term maintainability
- Trust in automation

\vspace{0.8em}

\begin{center}
\Large If the pipeline matters, the team must be able to read it.
\end{center}

# Reproducibility

## Docker-based task execution

microCI uses containerized task execution to reduce environment drift.

\vspace{0.6em}

\begin{columns}
\column{0.5\textwidth}
\textbf{Without reproducibility}

- Works on one machine only
- Hidden system dependencies
- Fragile release process
- Hard-to-debug failures

\column{0.5\textwidth}
\textbf{With microCI}

- Same image, same task
- Predictable tooling
- Repeatable builds
- Consistent execution path

\end{columns}

\vspace{0.8em}

\begin{center}
\Large Reproducibility is not a luxury; it is infrastructure hygiene.
\end{center}

# Vendor Independence

## CI/CD without platform lock-in

microCI separates pipeline intent from CI infrastructure.

\vspace{0.6em}

This makes it easier to:

- Move between GitHub Actions, GitLab CI, Jenkins, Buildkite, or self-hosted runners
- Keep automation alive when infrastructure changes
- Avoid rewriting pipelines for each provider
- Treat CI as an execution target, not the source of truth

\vspace{0.8em}

\begin{center}
\Large Your automation should outlive your CI vendor.
\end{center}

# Operational Simplicity

## Plain scripts reduce cognitive load

microCI favors simple generated artifacts over complex runtime magic.

\vspace{0.6em}


\begin{itemize}
\item No heavyweight orchestration model required
\item No hidden plugin lifecycle to understand
\item No special cloud service required to execute locally
\item No need to duplicate logic across local and remote workflows
\end{itemize}

\vspace{0.8em}

\begin{center}
\Large Simple tools scale because people can understand them.
\end{center}

# Security and Reliability

## Transparent automation is safer automation

microCI helps security-sensitive teams by keeping execution visible and reproducible.

\vspace{0.6em}

Security-oriented benefits:

- Reviewable generated scripts
- Clear command boundaries
- Containerized execution context
- Reduced reliance on third-party CI plugins
- Better evidence for audits and incident analysis

\vspace{0.8em}

\begin{center}
\Large Trust comes from visibility, not from magic.
\end{center}

# Developer Experience

## Better local feedback loops

A major advantage of microCI is that developers can run the same automation before pushing code.

\vspace{0.6em}

This reduces:

- CI failures caused by local/remote mismatch
- Slow push-wait-fix cycles
- Unclear pipeline behavior
- Dependency on remote runners for basic validation

\vspace{0.8em}

\begin{center}
\Large The best CI failure is the one found before the commit leaves the machine.
\end{center}

# Where microCI Fits

## Ideal use cases

microCI is especially useful when automation must be portable, reviewable, and reproducible.

\vspace{0.6em}

Good fit for:

- C++ and Rust builds
- Embedded software pipelines
- Container image builds
- Documentation generation
- Release packaging
- Internal operational workflows
- Self-hosted or hybrid CI environments

# Comparison

## Traditional CI vs. microCI

\begin{center}
\begin{tabular}{lll}
\toprule
\textbf{Dimension} & \textbf{Traditional CI} & \textbf{microCI} \\
\midrule
Source of truth & Provider config & Pipeline model + generated scripts \\
Local execution & Often limited & First-class \\
Auditability & Depends on platform/plugins & Script-based and explicit \\
Portability & Provider-specific & Infrastructure-independent \\
Debugging & Remote-runner centered & Shell/container centered \\
Vendor lock-in & Common & Reduced \\
\bottomrule
\end{tabular}
\end{center}

# Strategic Value

## microCI as automation infrastructure

microCI is not only a CI helper. It is a way to make automation durable.

\vspace{0.6em}

It helps organizations build pipelines that are:

- Portable across infrastructure
- Reproducible across environments
- Auditable by design
- Simple enough to maintain
- Independent from platform churn

\vspace{0.8em}

\begin{center}
\Large CI/CD should be boring, readable, and dependable.
\end{center}

# Closing Message

## Benefits in one sentence

\begin{center}
\Huge microCI makes automation portable, auditable, reproducible, and independent.
\end{center}

\vspace{1.5em}

\begin{center}
\Large Write your pipeline once. Run it anywhere.
\end{center}

