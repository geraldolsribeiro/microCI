# Install microCI

microCI is designed to stay lightweight: one YAML pipeline, one generated Bash script, and a small set of runtime requirements.

## Requirements

microCI relies on a small set of standard tools:

* `bash` — runs the generated pipeline script
* `docker` — provides isolated execution environments for steps
* `jq` — handles JSON data when pipelines need to inspect or transform structured output
* `yq` — handles YAML data when pipelines need to read or update configuration files

These tools are necessary because microCI generates Bash and expects a minimal runtime layer to execute portable steps consistently across machines.

At runtime, the simplicity stays the same: `microCI | bash`.

To run only specific steps, use `-O/--only` for a single named step or `-N/--number` with one or more comma-separated step numbers, such as `microCI --number 2,3,5`.

## Install

<div align="center" markdown="1">

![Install](images/microci-install.svg)</center>

</div>

```bash
curl -fsSL https://microci.dev/install.sh | bash
```
## Update

Keep the same workflow and refresh the binary:

<div align="center" markdown="1">

![Update](images/microci-update.svg)

</div>

```bash
microCI --update | bash
```

To track development builds instead:

```bash
microCI --update-dev | bash
```

## Remove

Remove microCI from your system:

<div align="center" markdown="1">

![Uninstall](images/microci-uninstall.svg)

</div>

```bash
microCI --uninstall | bash
```

