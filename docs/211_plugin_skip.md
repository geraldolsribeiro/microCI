# ⚙️ Skip plugin

## What it does

The `skip` plugin marks a step as intentionally disabled.

## Why it exists

When a pipeline is defined in YAML, it is useful to keep the structure intact while disabling a step temporarily. `skip` lets you do that without editing the surrounding workflow.

## Use cases

* disable a step during debugging
* bypass an expensive stage during local testing
* keep a pipeline definition readable while changing execution behavior

## Example

```yaml
steps:
  - name: "Ignored step"
    plugin:
      name: skip
```

## Notes

* No parameters are required.
* The pipeline continues with the next step.
