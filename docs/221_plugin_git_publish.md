# 🚢 git_publish plugin


## What it does

The `git_publish` plugin copies generated files into another Git repository and commits the result.

## Why it exists

Use `git_publish` when a pipeline produces deployable output that should become the contents of a separate repository, such as a docs site or a published frontend.

## Example

```yaml
steps:
  - name: "Publish to another Git repository"
    plugin:
      name: "git_publish"
      git_url: "git@your-server.com:awesome_deploy.git"
      copy_from: "site"
      copy_to: "/deploy"
      clean_before: true
```

## Notes

* Works well for generated sites and release artifacts.
* Keeps publishing logic inside the same pipeline definition.
