# 🤖 GitHub integration

Use GitHub Actions to run microCI on every push.

## Example workflow

```yaml
name: Execute microCI pipeline

on: [push]

jobs:
  execute-docker:
    runs-on: ubuntu-latest
    steps:
      - name: Check out repository
        uses: actions/checkout@v4

      - name: Run standard Docker container
        run: |
          curl -fsSL github.com/geraldolsribeiro/microci/releases/download/latest/microCI -o microCI
          chmod 755 ./microCI
          mkdir -p ~/.ssh/
          touch ~/.ssh/config
          ssh-keyscan github.com >> ~/.ssh/known_hosts
          ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519 -N "" -q
          ./microCI | bash

      - name: Save Multiple Outputs
        uses: actions/upload-artifact@v4
        with:
          name: multi-artifact
          path: |
            prog_c
            prog_cpp
```

## Reference project

This workflow is used in [`geraldolsribeiro/microCI-template-cpp`](https://github.com/geraldolsribeiro/microCI-template-cpp) at `.github/workflows/microci.yml`.

## Notes

* Generate the pipeline with `microCI`, then execute it with `bash`.
* Make sure the runner has SSH access if your pipeline pulls private repositories.
* Upload any build artifacts you want to keep after the job finishes.
