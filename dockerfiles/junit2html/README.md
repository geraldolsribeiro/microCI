# microCI junit2html image

This image converts JUnit XML results into HTML reports in **microCI**, packaging `junit2html` so test results can be rendered in a readable format without installing Python dependencies on the host.

## How to use with microCI

Use this image from a pipeline step and run the conversion command in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Render test report"
    docker: "intmain/microci_junit2html:0.11.0"
    plugin:
      name: bash
      bash: |
        junit2html results.xml report.html
```

## Related documentation

- **microCI** docs: https://microci.dev
